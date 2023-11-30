#ifndef MENU_H_
#define MENU_H_
#include <stdbool.h>

typedef struct menu menu_t;

typedef enum {MENU_OK, MENU_INEXISTENTE, MENU_ERROR} MENU_RESULTADO;

menu_t *menu_crear();

bool menu_agregar_comando(menu_t *,char *, char *,bool(*f)(void*));

MENU_RESULTADO menu_ejecutar_comando(menu_t *, char *, void *);

void destruir_menu(menu_t *);


#endif // MENU_H_