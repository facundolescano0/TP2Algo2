#include "menu.h"
#include <stdlib.h>
#include "hash.h"
#include "juego.h"
#include <string.h>

struct menu {
	hash_t *comandos;
};

struct informacion_comando {
	char *comando;
	char *descripcion;
	bool (*funcion)(void *);
};

menu_t *menu_crear()
{
	menu_t *menu = malloc(sizeof(menu_t));
	if (!menu)
		return NULL;
	menu->comandos = hash_crear(3);
	if (!menu->comandos) {
		free(menu);
		return NULL;
	}
	return menu;
}

void menu_agregar_comando(menu_t *menu, char *comando, char *descripcion,
			  bool (*f)(void *))
{
	if (!menu || !comando || !descripcion || !f)
		return;
	struct informacion_comando *info =
		malloc(sizeof(struct informacion_comando));
	if (!info)
		return;
	info->comando = comando;
	info->descripcion = descripcion;
	info->funcion = f;
	hash_t *insertado = hash_insertar(menu->comandos, comando, info, NULL);
	if (!insertado) {
		free(info);
		return;
	}
}

size_t menu_cantidad_comandos(menu_t *menu)
{
	if (!menu)
		return 0;
	return hash_cantidad(menu->comandos);
}

MENU_RESULTADO menu_ejecutar_comando(menu_t *menu, char *comando,
				     void *contexto)
{
	if (!menu || !comando || !contexto)
		return MENU_ERROR;
	struct informacion_comando *info =
		hash_obtener(menu->comandos, comando);
	if (!info)
		return MENU_INEXISTENTE;
	if (strcmp(comando, "cargar") == 0 || strcmp(comando, "elegir") == 0) {
		if (info->funcion(contexto))
			return MENU_AVANZAR;
		else
			return MENU_ERROR;
	}
	if (info->funcion(contexto))
		return MENU_OK;
	return MENU_ERROR;
}

void menu_quitar_comando(menu_t *menu, char *comando)
{
	if (!menu || !comando)
		return;
	void *elemento = hash_obtener(menu->comandos, comando);
	free(elemento);
	hash_quitar(menu->comandos, comando);
}

void destructor(void *valor)
{
	free(valor);
}

void menu_destruir(menu_t *menu)
{
	if (!menu)
		return;
	hash_destruir_todo(menu->comandos, destructor);
	free(menu);
}