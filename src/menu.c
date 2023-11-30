#include "menu.h"
#include <stdlib.h>
#include "hash.h"
#include "juego.h"

struct menu{
    hash_t *comandos;
};

struct informacion_comando{
    char *comando;
    char *descripcion;
    bool (*funcion)(void *);
};

struct juego_estado{
	juego_t *juego;
	bool continuar;
	menu_t *menu;
};


menu_t *menu_crear(){
    menu_t *menu = malloc(sizeof(menu_t));
    if(!menu)
        return NULL;
    menu->comandos = hash_crear(3);
    if(!menu->comandos){
        free(menu);
        return NULL;
    }
    return menu;
}

bool menu_agregar_comando(menu_t *menu, char *comando, char *descripcion, bool(*f)(void*)){
    if(!menu || !comando || !descripcion || !f)
        return false;
    struct informacion_comando *info= malloc(sizeof(struct informacion_comando));
    if(!info)
        return false;
    info->comando = comando;
    info->descripcion = descripcion;
    info->funcion = f;
    hash_t *insertado = hash_insertar(menu->comandos, comando, info, NULL);
    if(!insertado){
        free(info);
        return false;
    }
    return true;
}

MENU_RESULTADO menu_ejecutar_comando(menu_t *menu, char *comando, void *contexto){
    if(!menu || !comando || !contexto)
        return MENU_ERROR;
    struct informacion_comando *info= hash_obtener(menu->comandos, comando);
    if(!info)
        return MENU_INEXISTENTE;
    if(info->funcion(contexto))
        return MENU_OK;
    return MENU_ERROR;
}

void destruir_menu(menu_t *menu){
    if(!menu)
        return;
    hash_destruir(menu->comandos);
    free(menu);
}