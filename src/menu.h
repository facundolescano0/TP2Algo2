#ifndef MENU_H_
#define MENU_H_
#include <stdbool.h>
#include <stdio.h>

typedef struct menu menu_t;

typedef enum {
	MENU_OK,
	MENU_INEXISTENTE,
	MENU_ERROR,
	MENU_AVANZAR
} MENU_RESULTADO;

menu_t *menu_crear();

void menu_agregar_comando(menu_t *, char *, char *, bool (*f)(void *));

MENU_RESULTADO menu_ejecutar_comando(menu_t *, char *, void *);

size_t menu_cantidad_comandos(menu_t *);

void menu_quitar_comando(menu_t *, char *);

void menu_destruir(menu_t *);

#endif // MENU_H_