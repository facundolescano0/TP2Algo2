#include <stdlib.h>
#include <string.h>

#include "hash.h"

#define FACTOR_CARGA_MAXIMO 0.7
const size_t CAPACIDAD_MINIMA = 3;

struct par {
	char *clave;
	void *valor;
	struct par *siguiente;
};

struct hash {
	struct par **tabla;
	size_t capacidad;
	size_t cantidad;
};

hash_t *hash_crear(size_t capacidad)
{
	hash_t *hash = calloc(1, sizeof(hash_t));
	if (!hash)
		return NULL;
	if (capacidad <= CAPACIDAD_MINIMA)
		hash->capacidad = CAPACIDAD_MINIMA;
	else
		hash->capacidad = capacidad;

	hash->tabla = calloc(hash->capacidad, sizeof(struct par *));
	if (!hash->tabla) {
		free(hash);
		return NULL;
	}

	return hash;
}

unsigned long int funcion_hash(const char *clave)
{
	long unsigned int clave_hash = 0;
	while (*clave) {
		clave_hash = 31 * clave_hash + (long unsigned int)*clave;
		clave++;
	}
	return clave_hash;
}

void hash_insertar_par(hash_t *hash, struct par *par)
{
	size_t posicion = funcion_hash(par->clave) % hash->capacidad;

	if (!hash->tabla[posicion]) {
		hash->tabla[posicion] = par;
		return;
	}

	struct par *par_actual = hash->tabla[posicion];
	while (par_actual) {
		if (!par_actual->siguiente) {
			par_actual->siguiente = par;
			return;
		}
		par_actual = par_actual->siguiente;
	}
}

void rehashear(hash_t *hash_chico)
{
	hash_t *hash_nuevo = hash_crear(hash_chico->capacidad * 2);

	if (!hash_nuevo) {
		hash_destruir(hash_chico);
		return;
	}

	for (int i = 0; i < hash_chico->capacidad; i++) {
		struct par *par_actual = hash_chico->tabla[i];
		while (par_actual) {
			struct par *siguiente = par_actual->siguiente;
			par_actual->siguiente = NULL;
			hash_insertar_par(hash_nuevo, par_actual);
			par_actual = siguiente;
		}
	}
	free(hash_chico->tabla);
	hash_chico->tabla = hash_nuevo->tabla;
	hash_chico->capacidad = hash_nuevo->capacidad;
	free(hash_nuevo);
}

char *crear_copia(const char *clave_original)
{
	char *clave_copia = malloc(strlen(clave_original) + 1);
	if (!clave_copia)
		return NULL;
	strcpy(clave_copia, clave_original);

	return clave_copia;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;

	float factor_carga = (float)hash->cantidad / (float)hash->capacidad;
	if (factor_carga > FACTOR_CARGA_MAXIMO) {
		rehashear(hash);
	}

	size_t posicion = funcion_hash(clave) % hash->capacidad;

	if (!hash->tabla[posicion]) {
		struct par *nuevo_par = calloc(1, sizeof(struct par));
		nuevo_par->clave = crear_copia(clave);
		if (!nuevo_par->clave) {
			free(nuevo_par);
			return NULL;
		}
		nuevo_par->valor = elemento;
		hash->tabla[posicion] = nuevo_par;
		hash->cantidad++;
		if (anterior)
			*anterior = NULL;
		return hash;
	}

	struct par *par_actual = hash->tabla[posicion];
	while (par_actual) {
		if (strcmp(par_actual->clave, clave) == 0) {
			if (anterior)
				*anterior = par_actual->valor;
			par_actual->valor = elemento;
			return hash;
		}
		par_actual = par_actual->siguiente;
	}

	struct par *nuevo_par = calloc(1, sizeof(struct par));
	if (!nuevo_par)
		return NULL;
	nuevo_par->clave = crear_copia(clave);
	if (!nuevo_par->clave) {
		free(nuevo_par);
		return NULL;
	}
	nuevo_par->valor = elemento;
	struct par *par_corriente = hash->tabla[posicion];
	while (par_corriente) {
		if (!par_corriente->siguiente) {
			par_corriente->siguiente = nuevo_par;
			hash->cantidad++;
			if (anterior)
				*anterior = NULL;
			return hash;
		}
		par_corriente = par_corriente->siguiente;
	}

	return NULL;
}

struct par *buscar_par_anterior(struct par *par_actual, const char *clave_copia)
{
	if (!par_actual)
		return NULL;

	while (par_actual->siguiente) {
		if (strcmp(par_actual->siguiente->clave, clave_copia) == 0)
			return par_actual;
		par_actual = par_actual->siguiente;
	}

	return NULL;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = funcion_hash(clave) % hash->capacidad;
	if (!hash->tabla[posicion])
		return NULL;
	struct par *par_actual = hash->tabla[posicion];
	if (par_actual && strcmp(par_actual->clave, clave) == 0) {
		void *elemento_eliminado = par_actual->valor;
		hash->tabla[posicion] = hash->tabla[posicion]->siguiente;
		free(par_actual->clave);
		free(par_actual);
		hash->cantidad--;
		return elemento_eliminado;
	}

	struct par *par_anterior = buscar_par_anterior(par_actual, clave);
	if (!par_anterior)
		return NULL;
	struct par *par_auxiliar = par_anterior->siguiente;
	void *elemento_eliminado = par_auxiliar->valor;
	par_anterior->siguiente = par_auxiliar->siguiente;
	free(par_auxiliar->clave);
	free(par_auxiliar);
	hash->cantidad--;

	return elemento_eliminado;
}

struct par *obtener_par(hash_t *hash, const char *clave)
{
	size_t posicion = funcion_hash(clave) % hash->capacidad;
	if (!hash->tabla[posicion])
		return NULL;
	struct par *par_actual = hash->tabla[posicion];
	while (par_actual) {
		if (strcmp(par_actual->clave, clave) == 0) {
			return par_actual;
		}
		par_actual = par_actual->siguiente;
	}
	return NULL;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	struct par *par_clave = obtener_par(hash, clave);
	if (par_clave)
		return par_clave->valor;
	return NULL;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return false;
	struct par *par_clave = obtener_par(hash, clave);
	if (par_clave)
		return true;
	return false;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
	if (!hash)
		return;
	for (int i = 0; i < hash->capacidad; i++) {
		struct par *par_actual = hash->tabla[i];
		while (par_actual) {
			struct par *siguiente = par_actual->siguiente;
			free(par_actual->clave);
			free(par_actual);
			par_actual = siguiente;
		}
	}
	free(hash->tabla);
	free(hash);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;
	if (!destructor) {
		hash_destruir(hash);
		return;
	}

	for (int i = 0; i < hash->capacidad; i++) {
		struct par *par_actual = hash->tabla[i];
		while (par_actual) {
			struct par *siguiente = par_actual->siguiente;
			destructor(par_actual->valor);
			free(par_actual->clave);
			free(par_actual);
			par_actual = siguiente;
		}
	}
	free(hash->tabla);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	size_t iterados = 0;
	if (!hash || !f)
		return iterados;
	bool seguir = true;
	for (int i = 0; i < hash->capacidad && seguir; i++) {
		struct par *par_actual = hash->tabla[i];
		while (par_actual && seguir) {
			seguir = f(par_actual->clave, par_actual->valor, aux);
			iterados++;
			par_actual = par_actual->siguiente;
		}
	}
	return iterados;
}