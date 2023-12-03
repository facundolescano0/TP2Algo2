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
#include <time.h>
#include <ctype.h>

#define CANT_POKEMONES 3
#define MAX_LINEA 200
#define MAX_NOMBRE 20
#define CANT_JUGADA 2
#define OPONENTE 2
#define MAX_USADOS 9

struct info_main {
	lista_t *pokemones;
	pokemon_t **pokes_elegidos;
	char *usados[MAX_USADOS + 1];
	int tope_usados;
	bool continuar;
};

struct info_juego {
	juego_t *juego;
	menu_t *menu;
	adversario_t *adversario;
	struct info_main *info_main;
};

int compara_nombres(void *pokemon, void *nombre)
{
	if (!pokemon || !nombre)
		return -1;
	pokemon_t *poke = pokemon;
	char *nombre_poke = nombre;
	return strcmp(pokemon_nombre(poke), nombre_poke);
}

void guardar_pokemones(struct info_juego *info, char *nombre1, char *nombre2,
		       char *nombre3, JUGADOR jugador)
{
	if (!info || !nombre1 || !nombre2 || !nombre3)
		return;
	pokemon_t *pokes[CANT_POKEMONES];
	pokes[0] = lista_buscar_elemento(info->info_main->pokemones,
					 compara_nombres, nombre1);
	pokes[1] = lista_buscar_elemento(info->info_main->pokemones,
					 compara_nombres, nombre2);
	pokes[2] = lista_buscar_elemento(info->info_main->pokemones,
					 compara_nombres, nombre3);
	if (!pokes[0] || !pokes[1] || !pokes[2])
		return;
	if (jugador == JUGADOR1) {
		for (int i = 0; i < CANT_POKEMONES; i++) {
			if (i != OPONENTE)
				info->info_main->pokes_elegidos[i] = pokes[i];
			else
				info->info_main
					->pokes_elegidos[i + CANT_POKEMONES] =
					pokes[i];
		}
		return;
	}
	for (int i = 0; i < CANT_POKEMONES; i++) {
		if (i != OPONENTE)
			info->info_main->pokes_elegidos[i + CANT_POKEMONES] =
				pokes[i];
		else
			info->info_main->pokes_elegidos[i] = pokes[i];
	}
}

JUEGO_ESTADO seleccionar_pokemones_adv(struct info_juego *info)
{
	if (!info)
		return ERROR_GENERAL;
	char *pokemon1, *pokemon2, *pokemon3;
	bool electos = false;
	electos = adversario_seleccionar_pokemon(info->adversario, &pokemon1,
						 &pokemon2, &pokemon3);
	if (!electos)
		return ERROR_GENERAL;
	JUEGO_ESTADO seleccion;
	seleccion = juego_seleccionar_pokemon(info->juego, JUGADOR2, pokemon1,
					      pokemon2, pokemon3);
	if (seleccion != TODO_OK)
		return ERROR_GENERAL;
	guardar_pokemones(info, pokemon1, pokemon2, pokemon3, JUGADOR2);
	return TODO_OK;
}

bool inicializar_juego(struct info_juego *info)
{
	if (!info)
		return NULL;
	info->info_main->pokemones = juego_listar_pokemon(info->juego);
	if (lista_vacia(info->info_main->pokemones))
		return false;
	adversario_t *adversario = adversario_crear(info->info_main->pokemones);
	if (!adversario)
		return false;
	info->adversario = adversario;
	JUEGO_ESTADO seleccion_adv;
	seleccion_adv = seleccionar_pokemones_adv(info);
	if (seleccion_adv != TODO_OK)
		return false;
	return true;
}

bool cargar_archivo(void *info_juego)
{
	if (!info_juego)
		return true;
	struct info_juego *info = info_juego;

	char linea[MAX_LINEA];
	char *leido;
	printf("Ingrese el nombre del archivo: ");
	leido = fgets(linea, MAX_LINEA, stdin);
	if (!leido)
		return false;
	size_t tope = strlen(linea);
	linea[tope - 1] = 0;
	JUEGO_ESTADO archivo_cargado;
	archivo_cargado = juego_cargar_pokemon(info->juego, linea);
	if (archivo_cargado != TODO_OK)
		return false;

	bool inicializar = inicializar_juego(info);
	if (!inicializar)
		return false;
	return true;
}

bool salir_juego(void *info_juego)
{
	if (!info_juego)
		return false;
	struct info_juego *info = info_juego;
	info->info_main->continuar = false;
	return true;
}

bool mostrar_poke(void *pokemon, void *contexto)
{
	if (!pokemon)
		return false;
	pokemon_t *poke = pokemon;
	printf(">>> %s \n", pokemon_nombre(poke));
	return true;
}

bool listar_pokemones(void *info_juego)
{
	if (!info_juego)
		return false;
	struct info_juego *info = info_juego;
	if (!info->info_main->pokemones)
		return false;
	printf("Lista de pokemones:\n");
	lista_con_cada_elemento(info->info_main->pokemones, mostrar_poke, NULL);
	printf("\n");
	return true;
}

bool mostrar_ayuda1(void *contexto)
{
	const char *ayuda =
		"\n>>>Para comenzar, deberas cargar un archivo a continuacion.\n"
		"Deberas hacerlo mediante su respectivo comando,\n"
		"Como toda accion en este juego(o casi toda)\n"
		"Los comandos actuales son:\n"
		"'cargar'->para cargar archivo,\n"
		"'ayuda'->para mostrar ayuda,\n"
		"'salir'->para salir del juego\n"
		"Inserte comando,luego lo guiara la consola\n";
	printf("%s\n", ayuda);
	return true;
}
bool mostrar_ayuda2(void *contexto)
{
	const char *ayuda =
		"\n>>>Ahora, deberas 'elegir' tus pokemones(o algo asi,ya veras...).\n"
		"Deberas hacerlo mediante su respectivo comando,\n"
		"El fomato para hacerlo es :Charmander Pikachu Larvitar\n"
		"Los comandos actuales son :\n"
		"'elegir'->para seleccionar los 3 pokemones,\n"
		"'listar'->para la lista de pokemones disponibles,\n"
		"'ayuda'->para mostrar ayuda,\n"
		"'salir'->para salir del juego\n";
	printf("%s\n", ayuda);
	return true;
}

bool mostrar_ayuda3(void *contexto)
{
	const char *ayuda =
		"\n>>>A jugar! deberas realizar la jugada que desees.\n"
		"son 9 turnos, deberas elegir un ataque de un pokemon para cada jugada,\n"
		"deberas hacerlo mediante su respectivo comando('jugar'),\n"
		"el formato para elegir jugada es :Togepi Megapuño\n"
		"los comandos actuales son :\n"
		"'jugar':para realizar una jugada,\n"
		"'ayuda':para mostrar ayuda,\n"
		"'salir'(sin las comillas):para salir del juego\n";
	printf("%s\n", ayuda);
	return true;
}

void parsear_string(char *palabra)
{
	if (!palabra)
		return;
	if (palabra[0] != '\0') {
		palabra[0] = (char)toupper(palabra[0]);
		for (int i = 1; palabra[i] != '\0'; i++) {
			palabra[i] = (char)tolower(palabra[i]);
		}
	}
}

bool parsear_pokemones(char linea[MAX_LINEA], char *nombre1, char *nombre2,
		       char *nombre3)
{
	int cant_leidos = sscanf(linea, "%s %s %s ", nombre1, nombre2, nombre3);
	if (cant_leidos != CANT_POKEMONES)
		return false;
	parsear_string(nombre1);
	parsear_string(nombre2);
	parsear_string(nombre3);
	return true;
}

bool pedir_pokemones(void *info_juego)
{
	if (!info_juego)
		return false;
	struct info_juego *info = info_juego;
	if (lista_vacia(info->info_main->pokemones))
		return false;
	listar_pokemones(info);
	printf("Ejemplo:Charmander Pikachu Larvitar\n\n");
	printf("Elige tres pokemon de la lista:  ");

	char linea[MAX_LINEA];
	char *leido;
	leido = fgets(linea, MAX_LINEA, stdin);
	if (!leido)
		return false;
	size_t tope = strlen(linea);
	linea[tope - 1] = 0;

	char nombre1[MAX_NOMBRE], nombre2[MAX_NOMBRE], nombre3[MAX_NOMBRE];

	bool parseados = parsear_pokemones(linea, nombre1, nombre2, nombre3);

	if (!parseados)
		return false;

	JUEGO_ESTADO seleccion;
	seleccion = juego_seleccionar_pokemon(info->juego, JUGADOR1, nombre1,
					      nombre2, nombre3);

	if (seleccion != TODO_OK)
		return false;
	guardar_pokemones(info, nombre1, nombre2, nombre3, JUGADOR1);
	adversario_pokemon_seleccionado(info->adversario, nombre1, nombre2,
					nombre3);

	return true;
}

jugada_t parsear_jugada(char linea[MAX_LINEA])
{
	jugada_t jugada = { .ataque = "", .pokemon = "" };
	char pokemon[MAX_NOMBRE], ataque[MAX_NOMBRE];
	int cant_leidos = sscanf(linea, "%s %s", pokemon, ataque);
	if (cant_leidos != CANT_JUGADA)
		return jugada;

	parsear_string(pokemon);
	parsear_string(ataque);

	strcpy(jugada.pokemon, pokemon);
	strcpy(jugada.ataque, ataque);

	return jugada;
}

void informar_resultado(RESULTADO_ATAQUE resultado, jugada_t jugada1,
			jugada_t jugada2)
{
	if (resultado == ATAQUE_EFECTIVO)
		printf("ataque %s de %s fue EFECTIVO contra %s de %s\n\n",
		       jugada1.ataque, jugada1.pokemon, jugada2.ataque,
		       jugada2.pokemon);
	else if (resultado == ATAQUE_REGULAR)
		printf("ataque %s de %s fue REGULAR contra %s de %s\n\n",
		       jugada1.ataque, jugada1.pokemon, jugada2.ataque,
		       jugada2.pokemon);
	else if (resultado == ATAQUE_INEFECTIVO)
		printf("ataque %s de %s fue INEFECTIVO contra %s de %s\n\n",
		       jugada1.ataque, jugada1.pokemon, jugada2.ataque,
		       jugada2.pokemon);
}

bool jugar_turno(void *info_juego)
{
	if (!info_juego)
		return false;
	struct info_juego *info = info_juego;

	printf("\nElige un ataque de uno de tus pokemones\n");
	printf("Por ejemplo:Charmander Furia\n\nInserte su jugada>>>");

	char linea[MAX_LINEA];
	char *leido;
	leido = fgets(linea, MAX_LINEA, stdin);
	if (!leido)
		return false;

	size_t tope = strlen(linea);
	linea[tope - 1] = 0;

	jugada_t jugada_jugador1 = parsear_jugada(linea);

	jugada_t jugada_jugador2 = adversario_proxima_jugada(info->adversario);
	resultado_jugada_t resultado = juego_jugar_turno(
		info->juego, jugada_jugador1, jugada_jugador2);

	if (resultado.jugador1 == ATAQUE_ERROR)
		return false;

	if (resultado.jugador2 == ATAQUE_ERROR)
		return false;

	adversario_informar_jugada(info->adversario, jugada_jugador1);
	printf("\n>>>>>>>>>RESULTADO: Tu ");
	informar_resultado(resultado.jugador1, jugada_jugador1,
			   jugada_jugador2);
	printf("\n>>>>>>>>>RESULTADO DE OPONENTE: El ");
	informar_resultado(resultado.jugador2, jugada_jugador2,
			   jugada_jugador1);
	int tope_usados = info->info_main->tope_usados;
	strcpy(info->info_main->usados[tope_usados], jugada_jugador1.ataque);
	info->info_main->tope_usados++;
	return true;
}

void renovar_comandos(menu_t *menu, int nivel_nuevo)
{
	if (!menu)
		return;
	if (nivel_nuevo == 1) {
		menu_quitar_comando(menu, "cargar");
		menu_quitar_comando(menu, "ayuda");
		menu_agregar_comando(menu, "ayuda", "mostrar ayuda",
				     mostrar_ayuda2);
		menu_agregar_comando(menu, "elegir", "elegir pokemones",
				     pedir_pokemones);
		menu_agregar_comando(menu, "listar", "listar pokemones",
				     listar_pokemones);
		return;
	}
	menu_quitar_comando(menu, "seleccionar");
	menu_quitar_comando(menu, "ayuda");
	menu_quitar_comando(menu, "elegir");
	menu_agregar_comando(menu, "ayuda", "mostrar ayuda", mostrar_ayuda3);
	menu_agregar_comando(menu, "jugar", "comenzar a jugar", jugar_turno);
}

void cargar_comandos(menu_t *menu)
{
	if (!menu)
		return;
	menu_agregar_comando(menu, "cargar", "cargar archivo", cargar_archivo);
	menu_agregar_comando(menu, "salir", "salir del juego", salir_juego);
	menu_agregar_comando(menu, "ayuda", "mostrar ayuda", mostrar_ayuda1);
}

void mostrar_ataque(const struct ataque *ataque, void *pokemon)
{
	if (!ataque || !pokemon)
		return;
	struct ataque *atq = (struct ataque *)ataque;
	const char *poke = pokemon;
	printf(">>%s, ataque: %s, poder: %u\n", poke, atq->nombre, atq->poder);
}

void mostrar_info_jugar(struct info_juego *info)
{
	if (!info)
		return;
	printf("lista de ataques:\n");
	for (int i = 0; i < CANT_POKEMONES; i++) {
		const char *poke;
		poke = pokemon_nombre(info->info_main->pokes_elegidos[i]);
		con_cada_ataque(info->info_main->pokes_elegidos[i],
				mostrar_ataque, (void *)poke);
	}
	int puntaje1 = juego_obtener_puntaje(info->juego, JUGADOR1);
	int puntaje2 = juego_obtener_puntaje(info->juego, JUGADOR2);
	printf("Tu puntaje : %i, Puntaje adversario %i\n\n", puntaje1,
	       puntaje2);
	int tope_usados = info->info_main->tope_usados;
	if (tope_usados > 1) {
		for (int i = 0; i < tope_usados; i++)
			printf("%s ", info->info_main->usados[i]);
		printf("\n");
	}
}

void informar_ganador(juego_t *juego)
{
	if (!juego)
		return;
	int puntaje1 = juego_obtener_puntaje(juego, JUGADOR1);
	int puntaje2 = juego_obtener_puntaje(juego, JUGADOR2);
	printf("Tu puntaje : %i, Puntaje adversario %i\n\n", puntaje1,
	       puntaje2);
	if (puntaje1 == puntaje2)
		printf("Que peleado!! El juego termino en un empate. Gracias por jugar!\n\n");
	else if (puntaje1 > puntaje2)
		printf("Felicidades!! ganaste el juego por %i puntos. Gracias por jugar!\n\n",
		       puntaje1 - puntaje2);
	else
		printf("La proxima será... Perdiste por %i puntos. Gracias por jugar!\n\n",
		       puntaje2 - puntaje1);
}

void destruir_main(struct info_juego *info)
{
	if (!info)
		return;
	if (info->adversario)
		adversario_destruir(info->adversario);
	for (int i = 0; i < MAX_USADOS + 1; i++)
		free(info->info_main->usados[i]);
	juego_destruir(info->juego);
	free(info->info_main->pokes_elegidos);
	menu_destruir(info->menu);
	free(info->info_main);
}

int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));
	juego_t *juego = juego_crear();
	menu_t *menu = menu_crear();
	pokemon_t **pokes = malloc(CANT_POKEMONES * 2 * sizeof(pokemon_t *));
	struct info_main *info_main = calloc(1, sizeof(struct info_main));
	bool fallo = false;

	for (int i = 0; i < MAX_USADOS + 1; i++) {
		info_main->usados[i] = malloc((MAX_NOMBRE) * sizeof(char));
		if (!info_main->usados[i])
			fallo = true;
	}
	if (!juego || !menu || fallo || !pokes || !info_main) {
		juego_destruir(juego);
		menu_destruir(menu);
		free(pokes);
		for (int i = 0; i < MAX_USADOS + 1; i++)
			free(info_main->usados[i]);
		free(info_main);

		return -1;
	}

	struct info_juego info;
	info.juego = juego;
	info.info_main = info_main;
	strcpy(info_main->usados[0], "Ataques usados:");
	info.info_main->tope_usados = 1;
	info.info_main->pokes_elegidos = pokes;
	info.info_main->continuar = true;
	info.info_main->pokemones = NULL;
	info.adversario = NULL;
	cargar_comandos(menu);
	info.menu = menu;

	int nivel = 0;

	printf("\nBienvenido! para comenzar lea las instrucciones ingresando 'ayuda' (sin comillas)\n\n");

	while (!juego_finalizado(juego) && info.info_main->continuar) {
		if (nivel == 0)
			printf("Debes 'cargar' un archivo para avanzar. Escribir 'ayuda' para obtener ayuda\n\n");
		else if (nivel == 1)
			printf("\nAhora debes 'elegir' tus pokemones para avanzar. Escribir 'ayuda' para obtener ayuda\n"
			       "Observacion: no puedes elegir dos veces el mismo poke!\n\n");
		else {
			printf("\nA jugar! realiza tus jugadas con el comando 'jugar', recuerda que no puedes repetir ataques \n"
			       "El formato para realizar un ataque es:Pikachu Rayo\n"
			       "Escribir 'ayuda' para obtener ayuda\n\n");
			mostrar_info_jugar(&info);
		}

		printf("COMANDO>  ");
		char linea[MAX_LINEA];
		char *leido;
		leido = fgets(linea, MAX_LINEA, stdin);
		if (!leido)
			continue;
		size_t tope = strlen(linea);
		linea[tope - 1] = 0;

		MENU_RESULTADO ejecucion =
			menu_ejecutar_comando(info.menu, linea, &info);
		if (ejecucion == MENU_ERROR)
			printf("\nEl comando tuvo un error al ser ejecutado\n\n");
		else if (ejecucion == MENU_INEXISTENTE)
			printf("\nEste comando no existe. Podes probar con 'ayuda'\n\n");
		else if (ejecucion == MENU_AVANZAR) {
			nivel++;
			renovar_comandos(info.menu, nivel);
		}
	}
	if (juego_finalizado(juego))
		informar_ganador(juego);
	else
		printf("Gracias por jugar! (o intentar jeje)\n");

	destruir_main(&info);
	return 0;
}