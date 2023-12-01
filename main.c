#include "src/pokemon.h"
#include "src/ataque.h"
#include "src/juego.h"
#include "src/lista.h"
#include "src/hash.h"
#include "src/adversario.h"
#include <stdio.h>
#include "src/menu.h"
#include <string.h>

#define CANT_POKEMONES 3

struct juego_estado{
	juego_t *juego;
	bool continuar;
	menu_t *menu;
	lista_t *pokemones;
	lista_t *comandos_mostrar;
	adversario_t *adversario;
};


bool cargar_archivo(void *estado){
	if(!estado)
		return true;
	struct juego_estado *estado_j=estado;
	char linea[200];
	char *leido;
	printf("Ingrese el nombre del archivo: ");
	leido = fgets(linea,200,stdin);
	if(!leido)
		return false;
	size_t tope = strlen(linea);
	linea[tope-1] = 0;
	JUEGO_ESTADO archivo_cargado;
	archivo_cargado = juego_cargar_pokemon(estado_j->juego, linea);
	if(archivo_cargado != TODO_OK )
		return false;
	estado_j->pokemones = juego_listar_pokemon(estado_j->juego);	
	if(lista_vacia(estado_j->pokemones))
		return false;
	return true;
	
}

bool salir_juego(void *estado_juego){
	if(!estado_juego)
		return false;
	struct juego_estado *estado = estado_juego;
	estado->continuar =false;
	return true;
}

bool mostrar_poke(void *pokemon, void *contexto){
	if(!pokemon)
		return false;
	pokemon_t *poke = pokemon;
	printf("Nombre: %s \n", pokemon_nombre(poke));
	return true;
}

bool listar_pokemones(void *estado_juego){
	if(!estado_juego)
		return false;
	struct juego_estado *estado = estado_juego;
	if(!estado->pokemones)
		return false;
	lista_con_cada_elemento(estado->pokemones,mostrar_poke, NULL);
	return true;
}

bool mostrar_comando(void *info_comando,void* contexto){
	if(!info_comando)
		return false;
	char *comando = info_comando;
	printf("%s\n", comando);
	return true;
}

bool mostrar_ayuda (void *estado_juego){
	if(!estado_juego)
		return false;
	struct juego_estado *estado = estado_juego;
	size_t iterados= lista_con_cada_elemento(estado->comandos_mostrar, mostrar_comando, NULL);
	if(iterados == menu_cantidad_comandos(estado->menu))
		return true;
	return false;
}

JUEGO_ESTADO seleccionar_pokemones_adv(struct juego_estado *estado){
	if(!estado)
		return ERROR_GENERAL;
	char *pokemon1, *pokemon2, *pokemon3;
	bool electos = false;
	electos = adversario_seleccionar_pokemon(estado->adversario, &pokemon1,
				       &pokemon2,&pokemon3);
	if(!electos)
		return ERROR_GENERAL;
	JUEGO_ESTADO seleccion;
	seleccion = juego_seleccionar_pokemon(estado->juego, JUGADOR2, pokemon1,
				  pokemon2, pokemon3);
	if(seleccion != TODO_OK)
		return ERROR_GENERAL;
	return TODO_OK;
}

bool pedir_pokemones(void *estado_juego){
	
	if(!estado_juego)
		return false;
	struct juego_estado *estado = estado_juego;
	if(lista_vacia(estado->pokemones)) 
		return false;
	listar_pokemones(estado);
	printf("Elige tres pokemon de la lista:  ");
	char linea[200];
	char *leido;
	leido = fgets(linea,200,stdin);
	if(!leido)
		return false;
	size_t tope = strlen(linea);
	linea[tope-1] = 0;
	char nombre1[20], nombre2[20], nombre3[20];
	int cant_leidos = sscanf(linea, "%s %s %s", nombre1, nombre2,
				nombre3);
	if(cant_leidos != CANT_POKEMONES)
		return false;
	
	JUEGO_ESTADO seleccion;
	seleccion = juego_seleccionar_pokemon(estado->juego, JUGADOR1,
				       nombre1, nombre2, nombre3);
	
	if(seleccion != TODO_OK)
		return false;
	adversario_t *adversario = adversario_crear(estado->pokemones);
	if(!adversario)
		return false;
	estado->adversario = adversario;
	JUEGO_ESTADO seleccion_adv;
	seleccion_adv = seleccionar_pokemones_adv(estado);
	if(seleccion_adv != TODO_OK)
		return false;	
	return true;	
}

/*
prueba juego 
ejemplos/correcto.txt
Cacnea Charmander Floatzel

bool jugar_turno(void *estado_juego){
	if(!estado_juego)
		return false;
	struct juego_estado *estado = estado_juego;

	return false;
}*/


int main(int argc, char *argv[])
{
	juego_t *juego = juego_crear();
	lista_t *comandos_mostrar = lista_crear();	
	menu_t *menu = menu_crear();
	if(!juego || !comandos_mostrar||!menu ){
		juego_destruir(juego);
		lista_destruir(comandos_mostrar);
		menu_destruir(menu);
		return 0;
	}
		
	
	struct juego_estado estado;
	estado.juego = juego;
	estado.continuar = true;
	estado.pokemones = NULL;
	estado.adversario = NULL;
	
	menu_agregar_comando(menu, "cargar", "cargar archivo",cargar_archivo);
	lista_insertar(comandos_mostrar, "\n'cargar'-> cargar archivo\n");
	menu_agregar_comando(menu, "salir", "salir del juego",salir_juego);
	lista_insertar(comandos_mostrar, "'salir'-> salir del juego\n");
	menu_agregar_comando(menu,"listar", "ver lista pokemones",listar_pokemones);
	lista_insertar(comandos_mostrar, "'listar'-> ver lista pokemones\n");
	menu_agregar_comando(menu,"ayuda", "mostrar comandos",mostrar_ayuda);
	lista_insertar(comandos_mostrar, "'ayuda'-> mostrar comandos\n");
	menu_agregar_comando(menu,"seleccionar", "seleccionar pokemones",pedir_pokemones);
	lista_insertar(comandos_mostrar, "'seleccionar'-> seleccionar_pokemones\n");
	//menu_agregar_comando(menu,"jugar", "jugar turno",jugar_turno);
	//lista_insertar(comandos_mostrar, "'jugar'-> jugar turno\n");

	estado.comandos_mostrar=comandos_mostrar;
	estado.menu = menu;


	printf("\nBienvenido! para comenzar  ingrese 'ayuda' para conocer los comandos\n\n");

	while(!juego_finalizado(juego) && estado.continuar){

		printf("COMANDO>  ");
		char linea[200];
		char *leido;
		leido = fgets(linea,200,stdin);
		if(!leido)
			continue;
		size_t tope = strlen(linea);
		linea[tope-1] = 0;

		MENU_RESULTADO ejecucion = menu_ejecutar_comando(estado.menu, linea, &estado);
		if(ejecucion == MENU_ERROR)
			printf("\nEl comando tuvo un error al ser ejecutado\n\n");
		else if(ejecucion == MENU_INEXISTENTE)
			printf("\nEste comando no existe. Podes probar con 'ayuda'\n\n");
	}

	lista_destruir(estado.comandos_mostrar);
	if(estado.adversario)
		adversario_destruir(estado.adversario);
	juego_destruir(juego);
	menu_destruir(estado.menu);

}