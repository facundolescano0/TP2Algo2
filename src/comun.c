#include "comun.h"
#include "lista.h"
#include <stdbool.h>
#include "pokemon.h"
#include "ataque.h"
#include "hash.h"
#include <string.h>

int comparador_nombres(void *pokemon, void *nombre)
{
	if (!pokemon || !nombre)
		return -1;
	pokemon_t *poke = (pokemon_t *)pokemon;
	const char *nombre_poke = (const char *)nombre;
	return strcmp(pokemon_nombre(poke), nombre_poke);
}

bool seleccionar_pokemones(lista_t *lista_poke, const char *nombre1,
			   const char *nombre2, const char *nombre3,
			   pokemon_t **selecionados)
{
	void *pokemon1 = lista_buscar_elemento(lista_poke, comparador_nombres,
					       (void *)nombre1);
	void *pokemon2 = lista_buscar_elemento(lista_poke, comparador_nombres,
					       (void *)nombre2);
	void *pokemon3 = lista_buscar_elemento(lista_poke, comparador_nombres,
					       (void *)nombre3);
	if (!pokemon1 || !pokemon2 || !pokemon3)
		return false;

	selecionados[0] = (pokemon_t *)pokemon1;
	selecionados[1] = (pokemon_t *)pokemon2;
	selecionados[2] = (pokemon_t *)pokemon3;

	return true;
}

struct ataque_cargar verificar_jugada(struct recursos recursos[MAX_POKEMONES],
				      jugada_t jugada)
{
	bool ataque_disponible = false;
	const struct ataque *ataque = NULL;
	hash_t *usados_poke = NULL;
	for (int i = 0; i < MAX_POKEMONES && !ataque_disponible; i++) {
		if (strcmp(pokemon_nombre(recursos[i].pokemon),
			   jugada.pokemon) == 0) {
			ataque = pokemon_buscar_ataque(recursos[i].pokemon,
						       jugada.ataque);
			if (ataque) {
				usados_poke = recursos[i].ataques_usados;
				bool ataque_usado = hash_contiene(
					usados_poke, jugada.ataque);
				if (!ataque_usado)
					ataque_disponible = true;
			}
		}
	}
	struct ataque_cargar ataque_cargar;
	ataque_cargar.ataques_usados = NULL;
	ataque_cargar.ataque = NULL;
	if (!ataque_disponible)
		return ataque_cargar;

	ataque_cargar.ataques_usados = usados_poke;
	ataque_cargar.ataque = ataque;
	return ataque_cargar;
}