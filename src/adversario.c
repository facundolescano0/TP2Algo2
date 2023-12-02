#include "lista.h"
#include "juego.h"
#include "hash.h"
#include "adversario.h"
#include "pokemon.h"
#include "ataque.h"
#include <stdlib.h>
#include <string.h>	
#include "comun.h"

#define MAX_ATAQUE 20

struct ultimo_ataque{
	int pos_recurso;
	char ataque[MAX_ATAQUE];
};

struct iterar_ataques{
	struct recursos *recurso_actual;
	jugada_t jugada;
	bool jugada_lista;
};

struct adversario{
	JUGADOR jugador;
	lista_t *lista_poke;
	struct recursos recursos[MAX_POKEMONES];
	struct recursos jugador1[MAX_POKEMONES];
	struct ultimo_ataque ultimo_ataque;
};


adversario_t *adversario_crear(lista_t *pokemon)
{
	if(!pokemon)
		return NULL;
	adversario_t *adversario = calloc(1,sizeof(adversario_t));
	adversario->jugador = JUGADOR2;
	bool fallo = false;
	for(int i=0;i<MAX_POKEMONES;i++){
		adversario->recursos[i].ataques_usados = hash_crear(3);
		adversario->jugador1[i].ataques_usados = hash_crear(3);
		if(!adversario->recursos[i].ataques_usados ||
				!adversario->jugador1[i].ataques_usados)
			fallo = true;
	}
	if(fallo){
		for(int i=0;i<MAX_POKEMONES;i++){
		hash_destruir(adversario->recursos[i].ataques_usados);
		hash_destruir(adversario->jugador1[i].ataques_usados);
		}
		free(adversario);
		return NULL;
	}
	adversario->lista_poke = pokemon;
	return adversario;
		
}

bool adversario_seleccionar_pokemon(adversario_t *adversario, char **nombre1,
				    char **nombre2, char **nombre3)
{
	if(!adversario || !nombre1|| !nombre2|| !nombre3)
		return false;
	lista_t *lista_poke = adversario->lista_poke;
	int tope = (int)lista_tamanio(lista_poke);
	
	int aleatorio1 = rand() % tope+0;
	int aleatorio2 = rand() % tope+0;
	int aleatorio3 = rand() % tope+0;
	while(aleatorio1 == aleatorio2)
		aleatorio2 = rand() % tope+0;
	while(aleatorio3 == aleatorio1 || aleatorio3 == aleatorio2)
		aleatorio3 = rand() % tope+0;

	pokemon_t *poke1 = lista_elemento_en_posicion(lista_poke,(size_t)aleatorio1);
	pokemon_t *poke2 = lista_elemento_en_posicion(lista_poke,(size_t)aleatorio2);
	pokemon_t *poke3 = lista_elemento_en_posicion(lista_poke,(size_t)aleatorio3);
	*nombre1 = (char *)pokemon_nombre(poke1);
	*nombre2 = (char *)pokemon_nombre(poke2);
	*nombre3 = (char *)pokemon_nombre(poke3);
	if(!(*nombre1) ||!(*nombre2) ||!(*nombre3))
		return false;
	return true;
	
	adversario->recursos[0].pokemon = (pokemon_t *)poke1;
	adversario->recursos[1].pokemon = (pokemon_t *)poke2;
	adversario->jugador1[2].pokemon = (pokemon_t *)poke3;

}

bool adversario_pokemon_seleccionado(adversario_t *adversario, char *nombre1,
				     char *nombre2, char *nombre3)
{
	if(!adversario || !nombre1 || !nombre2 || !nombre3)
		return false;
	if(strcmp(nombre1,nombre2)==0 || strcmp(nombre1,nombre3)==0 || 
						strcmp(nombre2,nombre3)==0)
		return false;

	pokemon_t ** pokemones=malloc(3*sizeof(pokemon_t*));
	bool existen = seleccionar_pokemones(adversario->lista_poke, nombre1,
				     nombre2, nombre3,pokemones);
	if(!existen){
		free(pokemones);
		return false;
	}
	for(int i=0; i<MAX_POKEMONES; i++){
		adversario->jugador1[i].pokemon = pokemones[i];	
		if(i==OPONENTE)	
			adversario->recursos[i].pokemon = pokemones[i];
	}
	
	free(pokemones);
	return true;
}


void cargar_jugada(const struct ataque *ataque, void *iteradora){
	struct iterar_ataques *herramientas = (struct iterar_ataques *)iteradora;
	struct ataque *ataque_actual =(struct ataque*)ataque;
	if(herramientas->jugada_lista)
		return;
	hash_t *ataques_usados = herramientas->recurso_actual->ataques_usados;
	if(!hash_contiene(ataques_usados, ataque_actual->nombre)){
		strcpy(herramientas->jugada.ataque,ataque_actual->nombre);
		strcpy(herramientas->jugada.pokemon,pokemon_nombre
					(herramientas->recurso_actual->pokemon));
		herramientas->jugada_lista = true;
	}
}

jugada_t adversario_proxima_jugada(adversario_t *adversario)
{
	jugada_t jugada = { .ataque = "", .pokemon = "" };
	if(!adversario)
		return jugada;

	bool jugada_lista =false;
	struct iterar_ataques *iteradora = malloc(sizeof(struct iterar_ataques));
	iteradora->jugada = jugada;
	iteradora->jugada_lista = false;

	for(int i=0; i<MAX_POKEMONES && !jugada_lista; i++){
		iteradora->recurso_actual = &adversario->recursos[i];
		con_cada_ataque(adversario->recursos[i].pokemon,
		    cargar_jugada, iteradora);
		if(iteradora->jugada_lista){
			jugada_lista = true;
			adversario->ultimo_ataque.pos_recurso = i;
			strcpy(adversario->ultimo_ataque.ataque, iteradora->jugada.ataque);
			strcpy(jugada.ataque,iteradora->jugada.ataque);
			strcpy(jugada.pokemon,iteradora->jugada.pokemon);
			free(iteradora);
			return jugada;
		}
	}
	free(iteradora);
	return jugada;
}


void adversario_informar_jugada(adversario_t *adversario, jugada_t jugada)
{
	if(!adversario)
		return;

	struct ataque_cargar ataque_cargar = verificar_jugada(adversario->recursos,jugada);
	if(!ataque_cargar.ataque)
		return;

	const struct ataque *ataque = ataque_cargar.ataque;
	hash_t *usados_usuario = ataque_cargar.ataques_usados;

	int pos_recurso = adversario->ultimo_ataque.pos_recurso;
	hash_t *usados_adv = adversario->recursos[pos_recurso].ataques_usados;
	char ultimo_ataque[MAX_ATAQUE];
	strcpy(ultimo_ataque, adversario->ultimo_ataque.ataque);
	hash_insertar(usados_adv, ultimo_ataque, NULL, NULL);
	hash_insertar(usados_usuario, ataque->nombre, NULL, NULL);

}

void adversario_destruir(adversario_t *adversario)
{
	if(!adversario)
		return;
	for(int i = 0; i < MAX_POKEMONES;i++){
		hash_destruir(adversario->recursos[i].ataques_usados);
		hash_destruir(adversario->jugador1[i].ataques_usados);
	}
	free(adversario);
}