#ifndef TESTER_H
#define TESTER_H

#include <stdio.h>              /* printf */
#include <stdlib.h>             /* malloc */
#include <unistd.h>             /* exit */
#include <fcntl.h>              /* open */
#include <sys/stat.h>           /* stat */
#include <sys/ioctl.h>          /* ioctl */
#include <err.h>                /* perror */
#include <getopt.h>             /* getopt */
#include "../funciones_ioctl.h" /* funciones ioctl */
#include <string.h>

/* Declaracion de funciones */
static inline void modificar_tamanyo_pila(int cantidad);
static void mostrar_ayuda(void);
static int consumirElementos(int count);

int main (int argc, char **argv);

#endif //TESTER_H

