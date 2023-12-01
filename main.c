#include "src/pokemon.h"
#include "src/ataque.h"
#include "src/juego.h"
#include "src/lista.h"
#include "src/hash.h"
#include "src/adversario.h"
#include <stdio.h>
#include "src/menu.h"
#include <string.h>


struct juego_estado{
	juego_t *juego;
	bool continuar;
	menu_t *menu;
	lista_t *pokemones;
	lista_t *comandos_mostrar;
};


bool cargar_archivo(void *estado){
	if(!estado)
		return true;
	struct juego_estado *estado_j=estado;
	char linea[200];
	char *leido;
	leido = fgets(linea,200,stdin);
	if(!leido)
		return false;
	size_t tope = strlen(linea);
	linea[tope-1] = 0;
	JUEGO_ESTADO archivo_cargado;
	archivo_cargado = juego_cargar_pokemon(estado_j->juego, linea);
	if(archivo_cargado == TODO_OK )
		return true;
	return false;
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

	if(!estado->pokemones){
		estado->pokemones = juego_listar_pokemon(estado->juego);	
		if(lista_vacia(estado->pokemones)){
			estado->pokemones = NULL;
			return false;	
		}
	}
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

int main(int argc, char *argv[])
{
	juego_t *juego = juego_crear();
	lista_t *comandos_mostrar = lista_crear();
	menu_t *menu = menu_crear();
	if(!juego || !comandos_mostrar||!menu){
		juego_destruir(juego);
		lista_destruir(comandos_mostrar);
		menu_destruir(menu);
		return 0;
	}
		
	
	struct juego_estado estado;
	estado.juego = juego;
	estado.continuar = true;
	estado.pokemones = NULL;
	
	menu_agregar_comando(menu, "cargar", "cargar archivo",cargar_archivo);
	lista_insertar(comandos_mostrar, "\n'cargar'-> cargar archivo\n");
	menu_agregar_comando(menu, "salir", "salir del juego",salir_juego);
	lista_insertar(comandos_mostrar, "'salir'-> salir del juego\n");
	menu_agregar_comando(menu,"listar", "ver lista pokemones",listar_pokemones);
	lista_insertar(comandos_mostrar, "'listar'-> ver lista pokemones\n");
	menu_agregar_comando(menu,"ayuda", "mostrar comandos",mostrar_ayuda);
	lista_insertar(comandos_mostrar, "'ayuda'-> mostrar comandos\n");
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
	juego_destruir(juego);
	menu_destruir(estado.menu);

}