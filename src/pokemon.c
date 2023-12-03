#include <stdlib.h>
#include "pokemon.h"
#include "tipo.h"
#include <stdio.h>
#include "ataque.h"
#include <string.h>
#include <stdbool.h>

#define MAX_LONG 20
#define MAX_ATAQUES 3
#define MAX_LINEA 50

const char SEPARADOR = ';';

struct pokemon {
	char nombre[MAX_LONG];
	enum TIPO tipo;
	struct ataque ataques[MAX_ATAQUES];
	int cant_ataques;
};

struct info_pokemon {
	pokemon_t *pokemon;
	int cant_poke;
};

bool cuenta_separadores(const char *string, int cant_valida)
{
	int separadores = 0;
	bool valido = false;
	for (int i = 0; string[i] != '\0'; i++) {
		if (string[i] == SEPARADOR)
			separadores++;
	}
	if (separadores == cant_valida)
		valido = true;

	return valido;
}

enum TIPO definir_tipo(char type)
{
	if (type == 'N')
		return NORMAL;
	else if (type == 'F')
		return FUEGO;
	else if (type == 'P')
		return PLANTA;
	else if (type == 'E')
		return ELECTRICO;
	else if (type == 'R')
		return ROCA;
	else if (type == 'A')
		return AGUA;
	else
		return -1;
}

pokemon_t *llenar_pokemon(const char *string, pokemon_t *pokemon)
{
	if (!string || !pokemon || !cuenta_separadores(string, 1)) {
		free(pokemon);
		return NULL;
	}

	char type;
	int cant_leidos = sscanf(string, "%[^;];%c", pokemon->nombre, &type);
	if (cant_leidos != 2) {
		free(pokemon);
		return NULL;
	}
	pokemon->tipo = definir_tipo(type);
	if (pokemon->tipo == -1) {
		free(pokemon);
		return NULL;
	}

	return pokemon;
}

pokemon_t *cargar_ataque(const char *string, pokemon_t *poke)
{
	if (!string || !poke) {
		free(poke);
		return NULL;
	}

	if (!cuenta_separadores(string, 2)) {
		free(poke);
		return NULL;
	}

	struct ataque *atq = &(poke->ataques[poke->cant_ataques]);

	char type;
	int cant_leidos = sscanf(string, "%[^;];%c;%u", atq->nombre, &type,
				 &(atq->poder));

	if (cant_leidos != 3) {
		free(poke);
		return NULL;
	}
	enum TIPO tipo = definir_tipo(type);

	if (tipo == -1) {
		free(poke);
		return NULL;
	}

	atq->tipo = tipo;
	poke->cant_ataques++;
	return poke;
}

pokemon_t *realloc_poke(informacion_pokemon_t *info)
{
	if (!info)
		return NULL;
	unsigned long int pokes;
	pokes = (unsigned long int)(info->cant_poke + 1);
	pokemon_t *pokemon = realloc(info->pokemon, sizeof(pokemon_t) * pokes);
	if (!pokemon) {
		free(info->pokemon);
		return NULL;
	}
	return pokemon;
}

informacion_pokemon_t *agrandar_pokemon(informacion_pokemon_t *info)
{
	if (!info)
		return NULL;
	info->pokemon = realloc_poke(info);
	if (!info->pokemon) {
		free(info->pokemon);
		return NULL;
	}

	return info;
}

informacion_pokemon_t *ordenar_pokemones(informacion_pokemon_t *info)
{
	for (int j = 0; j < info->cant_poke - 1; j++) {
		for (int i = 0; i < info->cant_poke - 1 - j; i++) {
			if (strcmp(info->pokemon[i].nombre,
				   info->pokemon[i + 1].nombre) > 0) {
				pokemon_t auxiliar = info->pokemon[i + 1];
				info->pokemon[i + 1] = info->pokemon[i];
				info->pokemon[i] = auxiliar;
			}
		}
	}
	return info;
}

informacion_pokemon_t *pokemon_cargar_archivo(const char *path)
{
	if (!path)
		return NULL;
	FILE *archivo = fopen(path, "r");
	if (!archivo) {
		return NULL;
	}

	char linea[MAX_LINEA];
	int contador_lineas = 0;
	informacion_pokemon_t *info = calloc(1, sizeof(informacion_pokemon_t));
	if (!info) {
		fclose(archivo);
		return NULL;
	}
	pokemon_t *poke_aux = calloc(1, sizeof(pokemon_t));
	if (!info || !poke_aux) {
		free(info);
		free(poke_aux);
		fclose(archivo);
		return NULL;
	}
	while (fgets(linea, sizeof(linea), archivo) != NULL) {
		if (contador_lineas % 4 == 0) {
			poke_aux = llenar_pokemon(linea, poke_aux);
			if (!poke_aux && info->cant_poke == 0) {
				free(info);
				fclose(archivo);
				return NULL;
			}
			if (!poke_aux) {
				fclose(archivo);
				return info;
			}

		} else {
			poke_aux = cargar_ataque(linea, poke_aux);

			if (!poke_aux && info->cant_poke == 0) {
				free(info);
				fclose(archivo);
				return NULL;
			} else if (!poke_aux) {
				fclose(archivo);
				return info;
			}

			if (poke_aux->cant_ataques == 3) {
				agrandar_pokemon(info);
				info->pokemon[info->cant_poke] = *poke_aux;
				free(poke_aux);
				poke_aux = calloc(1, sizeof(pokemon_t));
				if (!poke_aux) {
					free(poke_aux);
					fclose(archivo);
					return info;
				}
				if (&(info->pokemon[info->cant_poke])) {
					info->cant_poke++;
				}
			}
		}
		contador_lineas++;
	}

	fclose(archivo);

	if (info->cant_poke > 0) {
		info = ordenar_pokemones(info);
		free(poke_aux);
	} else if (!info) {
		free(info);
		free(poke_aux);
	}
	return info;
}

pokemon_t *pokemon_buscar(informacion_pokemon_t *ip, const char *nombre)
{
	if (!ip || !nombre)
		return NULL;

	for (int i = 0; i < ip->cant_poke; i++) {
		if (strcmp(ip->pokemon[i].nombre, nombre) == 0)
			return &(ip->pokemon[i]);
	}
	return NULL;
}

int pokemon_cantidad(informacion_pokemon_t *ip)
{
	if (!ip)
		return 0;

	return ip->cant_poke;
}

const char *pokemon_nombre(pokemon_t *pokemon)
{
	if (!pokemon)
		return NULL;
	return pokemon->nombre;
}

enum TIPO pokemon_tipo(pokemon_t *pokemon)
{
	if (!pokemon)
		return NORMAL;
	return pokemon->tipo;
}

const struct ataque *pokemon_buscar_ataque(pokemon_t *pokemon,
					   const char *nombre)
{
	if (!pokemon || !nombre)
		return NULL;
	for (int i = 0; i < pokemon->cant_ataques; i++) {
		if (strcmp(pokemon->ataques[i].nombre, nombre) == 0)
			return &(pokemon->ataques[i]);
	}
	return NULL;
}

int con_cada_pokemon(informacion_pokemon_t *ip, void (*f)(pokemon_t *, void *),
		     void *aux)
{
	if (!ip || !aux || !f)
		return 0;

	for (int i = 0; i < ip->cant_poke; i++) {
		(*f)(&(ip->pokemon[i]), aux);
	}

	return ip->cant_poke;
}

int con_cada_ataque(pokemon_t *pokemon,
		    void (*f)(const struct ataque *, void *), void *aux)
{
	if (!pokemon || !aux || !f)
		return 0;

	for (int i = 0; i < pokemon->cant_ataques; i++)
		(*f)(&(pokemon->ataques[i]), aux);
	return pokemon->cant_ataques;
}

void pokemon_destruir_todo(informacion_pokemon_t *ip)
{
	if (!ip)
		return;
	if (ip->cant_poke > 0) {
		free(ip->pokemon);
		free(ip);
	}
}