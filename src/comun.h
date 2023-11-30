#ifndef COMUN_H_
#define COMUN_H_

#include "juego.h"
#include "lista.h"
#include "tipo.h"
#include <stdbool.h>
#include "pokemon.h"
#include "ataque.h"
#include "hash.h"
#include <stdlib.h>
#include <string.h>

#define MAX_POKEMONES 3
#define OPONENTE 2

struct recursos{
	pokemon_t *pokemon;
	hash_t *ataques_usados;
};

struct ataque_cargar{
	const struct ataque *ataque;
	hash_t *ataques_usados;
};

bool seleccionar_pokemones(lista_t *, const char *,
				     const char *, const char *,pokemon_t **);

struct ataque_cargar verificar_jugada(struct recursos recursos[MAX_POKEMONES], jugada_t jugada);


#endif // COMUN_H_