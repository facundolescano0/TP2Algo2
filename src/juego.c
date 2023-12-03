#include "juego.h"
#include "lista.h"
#include "tipo.h"
#include <stdbool.h>
#include "pokemon.h"
#include "ataque.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include "comun.h"

const int CANTIDAD_MINIMA = 4;

struct jugador {
	JUGADOR jugador;
	struct recursos recursos[MAX_POKEMONES];
	int puntaje;
};

struct juego {
	informacion_pokemon_t *ip;
	lista_t *lista_poke;
	struct jugador *jugador1;
	struct jugador *jugador2;
	size_t rondas_jugadas;
};

juego_t *juego_crear()
{
	juego_t *juego = calloc(1, sizeof(juego_t));
	if (!juego)
		return NULL;

	juego->jugador1 = calloc(1, sizeof(struct jugador));
	juego->jugador2 = calloc(1, sizeof(struct jugador));
	lista_t *lista_poke = lista_crear();
	if (!juego->jugador1 || !juego->jugador2 || !lista_poke) {
		free(juego->jugador1);
		free(juego->jugador2);
		free(lista_poke);
		free(juego);
		return NULL;
	}
	bool fallo = false;
	for (int i = 0; i < MAX_POKEMONES; i++) {
		juego->jugador1->recursos[i].ataques_usados = hash_crear(3);
		juego->jugador2->recursos[i].ataques_usados = hash_crear(3);
		if (!juego->jugador1->recursos[i].ataques_usados ||
		    !juego->jugador2->recursos[i].ataques_usados)
			fallo = true;
	}
	if (fallo) {
		for (int i = 0; i < MAX_POKEMONES; i++) {
			hash_destruir(
				juego->jugador1->recursos[i].ataques_usados);
			hash_destruir(
				juego->jugador2->recursos[i].ataques_usados);
		}
		lista_destruir(lista_poke);
		free(juego->jugador1);
		free(juego->jugador2);
		free(juego);
		return NULL;
	}
	juego->lista_poke = lista_poke;

	return juego;
}

JUEGO_ESTADO juego_cargar_pokemon(juego_t *juego, char *archivo)
{
	if (!juego || !archivo)
		return ERROR_GENERAL;
	informacion_pokemon_t *ip = pokemon_cargar_archivo(archivo);
	if (!ip)
		return ERROR_GENERAL;
	if (pokemon_cantidad(ip) < CANTIDAD_MINIMA) {
		pokemon_destruir_todo(ip);
		return POKEMON_INSUFICIENTES;
	}
	juego->ip = ip;
	return TODO_OK;
}

void listar_pokemon(pokemon_t *poke, void *lista)
{
	if (!poke || !lista)
		return;
	lista_t *lista_poke = (lista_t *)lista;
	lista_insertar(lista_poke, poke);
}

lista_t *juego_listar_pokemon(juego_t *juego)
{
	if (!juego)
		return NULL;
	lista_t *lista_poke = juego->lista_poke;
	int iterados = con_cada_pokemon(juego->ip, listar_pokemon, lista_poke);
	if (iterados != pokemon_cantidad(juego->ip)) {
		return NULL;
	}

	juego->lista_poke = lista_poke;
	return juego->lista_poke;
}

JUEGO_ESTADO juego_seleccionar_pokemon(juego_t *juego, JUGADOR jugador,
				       const char *nombre1, const char *nombre2,
				       const char *nombre3)
{
	if (!juego || !nombre1 || !nombre2 || !nombre3)
		return ERROR_GENERAL;
	if (strcmp(nombre1, nombre2) == 0 || strcmp(nombre1, nombre3) == 0 ||
	    strcmp(nombre2, nombre3) == 0)
		return POKEMON_REPETIDO;

	pokemon_t **pokemones = malloc(3 * sizeof(pokemon_t *));
	if (!pokemones)
		return ERROR_GENERAL;
	bool existen = seleccionar_pokemones(juego->lista_poke, nombre1,
					     nombre2, nombre3, pokemones);
	if (!existen) {
		free(pokemones);
		return POKEMON_INEXISTENTE;
	}

	if (jugador == JUGADOR1) {
		struct jugador *jugador_nuevo = juego->jugador1;
		jugador_nuevo->jugador = jugador;
		for (int i = 0; i < MAX_POKEMONES; i++) {
			if (i == OPONENTE)
				juego->jugador2->recursos[i].pokemon =
					pokemones[i];
			else
				jugador_nuevo->recursos[i].pokemon =
					pokemones[i];
		}
		jugador_nuevo->puntaje = 0;
		free(pokemones);
		return TODO_OK;
	}
	struct jugador *jugador_nuevo = juego->jugador2;
	jugador_nuevo->jugador = jugador;
	for (int i = 0; i < MAX_POKEMONES; i++) {
		if (i == OPONENTE)
			juego->jugador1->recursos[i].pokemon = pokemones[i];
		else
			jugador_nuevo->recursos[i].pokemon = pokemones[i];
	}
	jugador_nuevo->puntaje = 0;
	free(pokemones);
	return TODO_OK;
}

int determinar_puntos(const struct ataque *ataque1,
		      const struct ataque *ataque2)
{
	if (!ataque1 || !ataque2)
		return 0;
	enum TIPO tipo1 = ataque1->tipo;
	enum TIPO tipo2 = ataque2->tipo;
	int poder1 = (int)ataque1->poder;
	if (tipo1 == FUEGO) {
		if (tipo2 == PLANTA)
			return 3 * poder1;
		if (tipo2 == AGUA)
			return (poder1 + 1) / 2;
		return poder1;
	}

	if (tipo1 == PLANTA) {
		if (tipo2 == ROCA)
			return 3 * poder1;
		if (tipo2 == FUEGO)
			return (poder1 + 1) / 2;
		return poder1;
	}

	if (tipo1 == ROCA) {
		if (tipo2 == ELECTRICO)
			return 3 * poder1;
		if (tipo2 == PLANTA)
			return (poder1 + 1) / 2;
		return poder1;
	}

	if (tipo1 == ELECTRICO) {
		if (tipo2 == AGUA)
			return 3 * poder1;
		if (tipo2 == ROCA)
			return (poder1 + 1) / 2;
		return poder1;
	}

	if (tipo1 == AGUA) {
		if (tipo2 == FUEGO)
			return 3 * poder1;
		if (tipo2 == ELECTRICO)
			return (poder1 + 1) / 2;
		return poder1;
	}
	return poder1;
}

RESULTADO_ATAQUE determinar_eficacia_at(int puntaje, unsigned int poder)
{
	if (puntaje > poder)
		return ATAQUE_EFECTIVO;
	if (puntaje < poder)
		return ATAQUE_INEFECTIVO;
	return ATAQUE_REGULAR;
}

resultado_jugada_t determinar_resultado(juego_t *juego,
					const struct ataque *ataque1,
					const struct ataque *ataque2)
{
	resultado_jugada_t resultado;
	resultado.jugador1 = ATAQUE_ERROR;
	resultado.jugador2 = ATAQUE_ERROR;

	if (!juego || !ataque1 || !ataque2)
		return resultado;

	int puntaje1 = determinar_puntos(ataque1, ataque2);
	int puntaje2 = determinar_puntos(ataque2, ataque1);

	resultado.jugador1 = determinar_eficacia_at(puntaje1, ataque1->poder);
	resultado.jugador2 = determinar_eficacia_at(puntaje2, ataque2->poder);

	juego->jugador1->puntaje += puntaje1;
	juego->jugador2->puntaje += puntaje2;

	return resultado;
}

resultado_jugada_t fallo_ataques(const struct ataque *ataque1,
				 const struct ataque *ataque2)
{
	resultado_jugada_t resultado;
	resultado.jugador1 = ATAQUE_ERROR;
	resultado.jugador2 = ATAQUE_ERROR;

	if (!ataque1 && !ataque2)
		return resultado;
	if (!ataque1) {
		resultado.jugador2 = ATAQUE_REGULAR;
		return resultado;
	}
	resultado.jugador1 = ATAQUE_REGULAR;
	return resultado;
}

resultado_jugada_t juego_jugar_turno(juego_t *juego, jugada_t jugada_jugador1,
				     jugada_t jugada_jugador2)
{
	resultado_jugada_t resultado;
	resultado.jugador1 = ATAQUE_ERROR;
	resultado.jugador2 = ATAQUE_ERROR;
	if (!juego)
		return resultado;

	struct jugador *jugador1 = juego->jugador1;
	struct jugador *jugador2 = juego->jugador2;

	struct ataque_cargar ataque_cargar1 =
		verificar_jugada(jugador1->recursos, jugada_jugador1);
	struct ataque_cargar ataque_cargar2 =
		verificar_jugada(jugador2->recursos, jugada_jugador2);

	const struct ataque *ataque1 = ataque_cargar1.ataque;
	const struct ataque *ataque2 = ataque_cargar2.ataque;

	if (!ataque1 || !ataque2)
		return fallo_ataques(ataque1, ataque2);

	hash_t *usados_poke1 = ataque_cargar1.ataques_usados;
	hash_t *usados_poke2 = ataque_cargar2.ataques_usados;

	resultado = determinar_resultado(juego, ataque1, ataque2);
	hash_insertar(usados_poke1, ataque1->nombre, NULL, NULL);
	hash_insertar(usados_poke2, ataque2->nombre, NULL, NULL);
	juego->rondas_jugadas++;
	return resultado;
}

int juego_obtener_puntaje(juego_t *juego, JUGADOR jugador)
{
	if (!juego)
		return 0;
	if (jugador == JUGADOR1)
		return juego->jugador1->puntaje;
	return juego->jugador2->puntaje;
}

bool juego_finalizado(juego_t *juego)
{
	if (!juego || juego->rondas_jugadas == 9)
		return true;
	return false;
}

void juego_destruir(juego_t *juego)
{
	if (!juego)
		return;
	for (int i = 0; i < MAX_POKEMONES; i++) {
		hash_destruir(juego->jugador1->recursos[i].ataques_usados);
		hash_destruir(juego->jugador2->recursos[i].ataques_usados);
	}
	lista_destruir(juego->lista_poke);
	pokemon_destruir_todo(juego->ip);
	free(juego->jugador1);
	free(juego->jugador2);
	free(juego);
}