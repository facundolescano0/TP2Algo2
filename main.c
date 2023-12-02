#include "src/pokemon.h"
#include "src/ataque.h"
#include "src/juego.h"
#include "src/lista.h"
#include "src/hash.h"
#include "src/adversario.h"
#include <stdio.h>
#include "src/menu.h"
#include <string.h>
#include <stdlib.h>


#define CANT_POKEMONES 3
#define MAX_LINEA 200
#define MAX_NOMBRE 20
#define CANT_JUGADA 2
#define OPONENTE 2

struct juego_estado{
	juego_t *juego;
	bool continuar;
	menu_t *menu;
	lista_t *pokemones;
	adversario_t *adversario;
	pokemon_t **pokes_elegidos;
};

int compara_nombres(void *pokemon, void *nombre){
	if(!pokemon || !nombre)
		return -1;
	pokemon_t *poke = pokemon;
	char *nombre_poke = nombre;
	return strcmp(pokemon_nombre(poke),nombre_poke);
}

void guardar_pokemones(struct juego_estado *estado,char *nombre1
					,char *nombre2,char *nombre3,JUGADOR jugador){
	if(!estado || !nombre1 ||!nombre2 || !nombre3)
		return;
	pokemon_t *pokes[CANT_POKEMONES];
	pokes[0] = lista_buscar_elemento(estado->pokemones, 
						compara_nombres,nombre1);
	pokes[1] = lista_buscar_elemento(estado->pokemones, 
						compara_nombres,nombre2);
	pokes[2] = lista_buscar_elemento(estado->pokemones, 
						compara_nombres,nombre3);
	if(jugador == JUGADOR1){
		for(int i = 0; i<CANT_POKEMONES ; i++){
			if(i!=OPONENTE)
				estado->pokes_elegidos[i] = pokes[i]; 
			else
				estado->pokes_elegidos[i+CANT_POKEMONES] = pokes[i]; 
		}
		return;
	}
	for(int i = 0; i<CANT_POKEMONES ; i++){
		if(i!=OPONENTE)
			estado->pokes_elegidos[i+CANT_POKEMONES] = pokes[i]; 
		else
			estado->pokes_elegidos[i] = pokes[i]; 
	}
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
	guardar_pokemones(estado,pokemon1,pokemon2,pokemon3,JUGADOR2);
	return TODO_OK;
}

/*
void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto);
*/

bool inicializar_juego(struct juego_estado *estado){
	if(!estado)
		return NULL;
	estado->pokemones = juego_listar_pokemon(estado->juego);
	if(lista_vacia(estado->pokemones))
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

bool cargar_archivo(void *estado){
	if(!estado)
		return true;
	struct juego_estado *estado_j=estado;
	char linea[MAX_LINEA];
	char *leido;
	printf("Ingrese el nombre del archivo: ");
	leido = fgets(linea,MAX_LINEA,stdin);
	if(!leido)
		return false;
	size_t tope = strlen(linea);
	linea[tope-1] = 0;
	JUEGO_ESTADO archivo_cargado;
	archivo_cargado = juego_cargar_pokemon(estado_j->juego, linea);
	if(archivo_cargado != TODO_OK )
		return false;
	bool inicializar = inicializar_juego(estado_j);
	if(!inicializar)
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
	printf(">>> %s \n", pokemon_nombre(poke));
	return true;
}

bool listar_pokemones(void *estado_juego){
	if(!estado_juego)
		return false;
	struct juego_estado *estado = estado_juego;
	if(!estado->pokemones)
		return false;
	printf("Lista de pokemones:\n");
	lista_con_cada_elemento(estado->pokemones,mostrar_poke, NULL);
	return true;
}


bool mostrar_ayuda1(void *contexto){
	const char *ayuda = "\n>>>Para comenzar, deberas cargar un archivo a continuacion.\n"
			"deberas hacerlo mediante su respectivo comando,\n"
			"como toda accion en este juego(o casi toda)\n"
			"los comandos actuales son :\n"
			"'cargar'->para cargar archivo,\n" 
			"'ayuda'->para mostrar ayuda,\n"
			"'salir'(sin las comillas)->para salir del juego\n"
			"inserte comando,luego lo guiara la consola\n"
			"un archivo valido es ejemplos/correcto.txt\n";
	printf("%s\n",ayuda);
	return true;
}
bool mostrar_ayuda2(void *contexto){
	const char *ayuda = "\n>>>Ahora, deberas elegir tus pokemones(o algo asi,ya veras...).\n"
			"deberas hacerlo mediante su respectivo comando,\n"
			"el fomato para hacerlo es :Charmander Pikachu Larvitar\n"
			"los comandos actuales son :\n"
			"'elegir'->para seleccionar los 3 pokemones,\n" 
			"'listar'->para la lista de pokemones disponibles,\n"
			"'ayuda'->para mostrar ayuda,\n"
			"'salir'(sin las comillas)->para salir del juego\n";
	printf("%s\n",ayuda);
	return true;
}

bool mostrar_ayuda3(void *contexto){
	const char *ayuda = "\n>>>A jugar! deberas realizar la jugada que desees.\n"
			"son 9 turnos, deberas elegir un ataque de un pokemon para cada jugada,\n"
			"deberas hacerlo mediante su respectivo comando('jugar'),\n"
			"el formato para elegir jugada es :Togepi Megapuño\n"
			"los comandos actuales son :\n"
			"'jugar':para realizar una jugada,\n" 
			"'ayuda':para mostrar ayuda,\n"
			"'salir'(sin las comillas):para salir del juego\n";
	printf("%s\n",ayuda);
	return true;
}

bool pedir_pokemones(void *estado_juego){
	
	if(!estado_juego)
		return false;
	struct juego_estado *estado = estado_juego;
	if(lista_vacia(estado->pokemones)) 
		return false;
	listar_pokemones(estado);
	printf("Elige tres pokemon de la lista:  ");
	char linea[MAX_LINEA];
	char *leido;
	leido = fgets(linea,MAX_LINEA,stdin);
	if(!leido)
		return false;
	size_t tope = strlen(linea);
	linea[tope-1] = 0;

	char nombre1[MAX_NOMBRE], nombre2[MAX_NOMBRE], nombre3[MAX_NOMBRE];
	int cant_leidos = sscanf(linea, "%s %s %s ", nombre1, nombre2,
				nombre3);
	if(cant_leidos != CANT_POKEMONES)
		return false;

	JUEGO_ESTADO seleccion;
	seleccion = juego_seleccionar_pokemon(estado->juego, JUGADOR1,
				       nombre1, nombre2, nombre3);
	
	if(seleccion != TODO_OK)
		return false;
	guardar_pokemones(estado,nombre1,nombre2,nombre3,JUGADOR1);
	adversario_pokemon_seleccionado(estado->adversario, nombre1,
				     nombre2, nombre3);

	return true;	
}

/*
prueba juego 
ejemplos/correcto.txt
Cacnea Charmander Floatzel
}*/

bool jugar_turno(void *estado_juego){
	if(!estado_juego)
		return false;
	struct juego_estado *estado = estado_juego;
	printf("Elige un ataque de uno de tus pokemones\n");
	printf("Por ejemplo:Charmander Furia\n>>>");
	char linea[MAX_LINEA];
	char *leido;
	leido = fgets(linea,MAX_LINEA,stdin);
	if(!leido)
		return false;
	size_t tope = strlen(linea);
	linea[tope-1] = 0;

	char pokemon[MAX_NOMBRE], ataque[MAX_NOMBRE];
	int cant_leidos = sscanf(linea, "%s %s", pokemon, ataque);
	if(cant_leidos != CANT_JUGADA)
		return false;
	jugada_t jugada_jugador1;
	strcpy(jugada_jugador1.pokemon,pokemon);
	strcpy(jugada_jugador1.ataque,ataque);
	jugada_t jugada_jugador2 = adversario_proxima_jugada(estado->adversario);
	resultado_jugada_t resultado =juego_jugar_turno(estado->juego, 
						jugada_jugador1, jugada_jugador2);
	if(resultado.jugador1 ==  ATAQUE_ERROR){
		printf("error jugador 1\n");
		return false;
	}
		
	if(resultado.jugador2 ==  ATAQUE_ERROR){
			printf("error jugador2\n");
			return false;			
	}
			
	if(resultado.jugador1 == ATAQUE_EFECTIVO)
		printf("El resultado de tu ataque fue EFECTIVO\n");
	else if(resultado.jugador1 == ATAQUE_REGULAR)
		printf("El resultado de tu ataque fue REGULAR\n");
	else if(resultado.jugador1 == ATAQUE_INEFECTIVO)
		printf("El resultado de tu ataque fue INEFECTIVO\n");
	

	return true;
}

void renovar_comandos(menu_t *menu, int nivel_nuevo){
	if(!menu)
		return;
	if(nivel_nuevo==1){
		menu_quitar_comando(menu, "cargar");
		menu_quitar_comando(menu, "ayuda");
		menu_agregar_comando(menu,"ayuda","mostrar ayuda", mostrar_ayuda2);
		menu_agregar_comando(menu,"elegir", "elegir pokemones",pedir_pokemones);
		menu_agregar_comando(menu,"listar", "listar pokemones" ,listar_pokemones);
		return;
	}
	menu_quitar_comando(menu, "seleccionar");
	menu_quitar_comando(menu, "ayuda");
	menu_quitar_comando(menu, "elegir");
	menu_agregar_comando(menu,"ayuda","mostrar ayuda", mostrar_ayuda3);
	menu_agregar_comando(menu,"jugar", "comenzar a jugar" ,jugar_turno);
}

void cargar_comandos(menu_t *menu){
	menu_agregar_comando(menu, "cargar", "cargar archivo",cargar_archivo);
	menu_agregar_comando(menu, "salir", "salir del juego" ,salir_juego);
	menu_agregar_comando(menu,"ayuda", "mostrar ayuda" ,mostrar_ayuda1);
}

void mostrar_ataque(const struct ataque *ataque, void *pokemon){
	if(!ataque || !pokemon)
		return;
	struct ataque* atq = (struct ataque*)ataque;
	const char *poke = pokemon;
	printf("%s, ataque: %s, poder: %u\n", poke, atq->nombre,atq->poder);
}

void mostrar_pokemones(struct juego_estado *estado){
	if(!estado)
		return;
	printf("lista de ataques:\n");
	for(int i= 0;i<CANT_POKEMONES;i++){
		const char *poke;
		poke = pokemon_nombre(estado->pokes_elegidos[i]);
		con_cada_ataque(estado->pokes_elegidos[i],
								mostrar_ataque, (void*)poke);
	}
	int puntaje1=juego_obtener_puntaje(estado->juego, JUGADOR1);
	int puntaje2=juego_obtener_puntaje(estado->juego, JUGADOR2);
	printf("Tu puntaje : %i, Puntaje adversario %i\n",puntaje1,puntaje2);

}

int main(int argc, char *argv[])
{
	juego_t *juego = juego_crear();
	menu_t *menu = menu_crear();
	pokemon_t **poke = malloc(CANT_POKEMONES*2*sizeof(pokemon_t*));
	if(!juego||!menu ){
		juego_destruir(juego);
		menu_destruir(menu);
		free(poke);
		return 0;
	}
		
	
	struct juego_estado estado;
	estado.juego = juego;
	estado.pokes_elegidos = poke;
	estado.continuar = true;
	estado.pokemones = NULL;
	estado.adversario = NULL;
	
	cargar_comandos(menu);
	estado.menu = menu;

	int nivel = 0;

	printf("\nBienvenido! para comenzar lea las instrucciones ingresando 'ayuda' (sin comillas)\n\n");

	while(!juego_finalizado(juego) && estado.continuar){
		if(nivel == 0)
			printf("Debes cargar un archivo para avanzar. escribir 'ayuda' para obtener ayuda\n");
		else if(nivel == 1)
			printf("Ahora debes seleccionar tus pokemones para avanzar. escribir 'ayuda' para obtener ayuda\n");
		else{
			printf("A jugar! realiza tus jugadas. Recuerda el formato para realizar un ataque es :Pikachu Rayo\n");
			mostrar_pokemones(&estado);
		}
			

		printf("COMANDO>  ");
		char linea[MAX_LINEA];
		char *leido;
		leido = fgets(linea,MAX_LINEA,stdin);
		if(!leido)
			continue;
		size_t tope = strlen(linea);
		linea[tope-1] = 0;

		MENU_RESULTADO ejecucion = menu_ejecutar_comando(estado.menu, linea, &estado);
		if(ejecucion == MENU_ERROR)
			printf("\nEl comando tuvo un error al ser ejecutado\n\n");
		else if(ejecucion == MENU_INEXISTENTE)
			printf("\nEste comando no existe. Podes probar con 'ayuda'\n\n");
		else if(ejecucion == MENU_AVANZAR){
			nivel++;
			renovar_comandos(estado.menu, nivel);
		}
	}

	if(estado.adversario)
		adversario_destruir(estado.adversario);
	juego_destruir(juego);
	free(estado.pokes_elegidos);
	menu_destruir(estado.menu);

}