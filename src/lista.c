#include "lista.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	nodo_t *inicio;
	nodo_t *final;
	size_t cantidad;
};

struct lista_iterador {
	nodo_t *actual;
	lista_t *lista;
};

lista_t *lista_crear()
{
	lista_t *lista = calloc(1, sizeof(lista_t));
	lista->cantidad = 0;
	if (!lista)
		return NULL;
	return lista;
}

nodo_t *crear_nodo(void *elemento)
{
	nodo_t *nodo = malloc(sizeof(nodo_t));
	if (!nodo)
		return NULL;
	nodo->elemento = elemento;

	nodo->siguiente = NULL;
	return nodo;
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (!lista)
		return NULL;

	nodo_t *nodo = crear_nodo(elemento);
	nodo->siguiente = NULL;

	if (lista->cantidad > 0) {
		lista->final->siguiente = nodo;
		lista->final = nodo;
		lista->final->siguiente = NULL;

	} else {
		lista->inicio = nodo;

		lista->final = nodo;
	}

	lista->cantidad++;

	return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista)
		return NULL;

	if (posicion >= lista->cantidad || lista_vacia(lista)) {
		return lista_insertar(lista, elemento);
	}

	else if (posicion == 0) {
		nodo_t *nodo = crear_nodo(elemento);
		nodo->siguiente = lista->inicio;
		lista->inicio = nodo;
		lista->cantidad++;
	} else if (posicion < lista->cantidad) {
		nodo_t *nodo = crear_nodo(elemento);
		size_t i = 0;
		nodo_t *aux = lista->inicio;
		while (i < posicion - 1) {
			aux = aux->siguiente;
			i++;
		}
		nodo->siguiente = aux->siguiente;
		aux->siguiente = nodo;
		lista->cantidad++;
	}

	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (!lista || lista_vacia(lista))
		return NULL;

	void *elemento = lista->final->elemento;
	size_t i = 1;
	nodo_t *actual = lista->inicio;
	nodo_t *aux = lista->final;
	while (i < lista->cantidad - 1) {
		actual = actual->siguiente;
		i++;
	}
	actual->siguiente = NULL;
	lista->final = actual;
	free(aux);
	lista->cantidad--;

	return elemento;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || lista_vacia(lista))
		return NULL;

	if (posicion == 0) {
		void *elemento = lista->inicio->elemento;
		nodo_t *aux = lista->inicio;
		lista->inicio = aux->siguiente;
		free(aux);
		lista->cantidad--;
		return elemento;
	} else if (posicion < lista->cantidad) {
		size_t i = 1;
		nodo_t *actual = lista->inicio;
		nodo_t *aux;
		nodo_t *posterior;

		while (i < posicion) {
			actual = actual->siguiente;
			i++;
		}
		void *elemento = actual->siguiente->elemento;
		aux = actual->siguiente;
		posterior = aux->siguiente;
		actual->siguiente = posterior;
		free(aux);
		lista->cantidad--;
		return elemento;
	} else if (posicion > lista->cantidad) {
		return lista_quitar(lista);
	}

	return NULL;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || posicion >= lista->cantidad)
		return NULL;
	nodo_t *actual = lista->inicio;
	size_t i = 0;
	while (i < posicion) {
		actual = actual->siguiente;
		i++;
	}
	return actual->elemento;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (!lista || !comparador)
		return NULL;
	bool encontrado = false;
	nodo_t *actual = lista->inicio;
	size_t i = 0;
	while (i < lista->cantidad && !encontrado) {
		if ((comparador)(actual->elemento, contexto) == 0) {
			encontrado = true;
			return actual->elemento;
		} else {
			actual = actual->siguiente;
		}
		i++;
	}
	return NULL;
}

void *lista_primero(lista_t *lista)
{
	if (!lista || lista->cantidad == 0)
		return NULL;

	return lista->inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if (!lista || lista->cantidad == 0)
		return NULL;

	return lista->final->elemento;
}

bool lista_vacia(lista_t *lista)
{
	if (!lista || lista->cantidad == 0)
		return true;
	else
		return false;
}

size_t lista_tamanio(lista_t *lista)
{
	if (!lista || lista->cantidad == 0)
		return 0;
	return lista->cantidad;
}

void lista_destruir(lista_t *lista)
{
	if (!lista)
		return;

	if (lista->cantidad > 0) {
		nodo_t *actual = lista->inicio;
		nodo_t *aux;
		for (int i = 0; i < lista->cantidad; i++) {
			aux = actual->siguiente;
			free(actual);
			actual = aux;
		}
	}

	free(lista);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista)
		return;

	nodo_t *actual = lista->inicio;
	nodo_t *aux;
	for (int i = 0; i < lista->cantidad; i++) {
		aux = actual->siguiente;
		if (funcion) {
			(funcion)(actual->elemento);
		}

		free(actual);
		actual = aux;
	}
	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista)
		return NULL;
	lista_iterador_t *it = malloc(sizeof(lista_iterador_t));
	it->actual = lista->inicio;
	it->lista = lista;

	return it;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (!iterador || lista_vacia(iterador->lista) || !iterador->actual)
		return false;
	return true;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->actual)
		return false;
	if (iterador->actual->siguiente == NULL) {
		iterador->actual = NULL;
		return false;
	}
	iterador->actual = iterador->actual->siguiente;
	return true;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->actual)
		return NULL;

	return iterador->actual->elemento;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	if (!iterador)
		return;
	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (!lista || !funcion || lista_tamanio(lista) == 0)
		return 0;

	nodo_t *actual = lista->inicio;
	size_t contador = 0;
	bool iterar = true;
	while (iterar && contador < lista->cantidad) {
		if (!funcion(actual->elemento, contexto))
			iterar = false;
		if (actual->siguiente != NULL)
			actual = actual->siguiente;

		contador++;
	}

	return contador;
}
