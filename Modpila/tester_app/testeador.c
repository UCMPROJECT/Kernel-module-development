#include "testeador.h"

/** Variables globales */
char * buffer;
int fd_fichero;
int posicion;

/** Flags */
int modify_mode;
int consum_mode;


#define NUM_ITERACIONES 10
#define TAM_BUFFER 1024

void inline limpiar_flags(void)
{
    consum_mode =  0;
    modify_mode = 0;
}

/** Programa de testeo:
 * 
 * 1) Abre el fichero definido en funciones_ioctl.h y reserva un buffer
 * 2) Consume los flags de entrada rwlRmh
 * 3) Espera como entrada las siguientes opciones
 *      ./tester_app -k {numero}
 *		./tester_app -m {numero}
 * 4) cierra el fichero
 */
int main (int argc, char **argv)
{
    int optc;
    struct stat stat_fichero;
    limpiar_flags();
    posicion = 0;

    // abrimos el fichero
    // fd_fichero = open(RUTA_FICHERO, O_RDWR);
    // if (fd_fichero == -1){
    //     perror("error : inicio, abrir fichero"),exit(-1);
    // }

    // Crear un buffer de tam_fichero 
    buffer = (char *) malloc(TAM_BUFFER);
    if(buffer==NULL){
        perror ("error : inicio, malloc"),exit(-2);
    }

    int contador;
    for (contador = 0; contador < NUM_ITERACIONES; contador++){
        fd_fichero = open(RUTA_FICHERO, O_RDWR);
        if (fd_fichero == -1){
            perror("error : inicio, abrir fichero"),exit(-1);
        }

        char elementos[TAM_BUFFER];
        int len;
        int sz;
        int contador2;

        sz = snprintf(elementos, 4, "%i;", contador);
        snprintf(elementos+sz, 2,"%i", contador);
        printf("Escrito: %s\n",elementos);

        escribir_fichero(elementos);
    }

    printf("----------------------------------------------------\n");

    posicion = 0;

    for (contador = 0; contador < NUM_ITERACIONES; contador++){   
        fd_fichero = open(RUTA_FICHERO, O_RDWR);
        if (fd_fichero == -1){
            perror("error : inicio, abrir fichero"),exit(-1);
        }

        leer_fichero(10);
        sleep(1);
        close(fd_fichero);
    }
    //consumirElementos(3);

	// Cerrar el fichero
    //close(fd_fichero);

    // Salir de la aplicacion
    exit(0);
}

/** Funciones */

static void mostrar_ayuda(void)
{
    printf("uso: [OPCIONES] [ARGUMENTO]\n");
    printf("./tester_app -k {numero} : Consumir primeros {numero} elementos\n");    
    printf("./tester_app -m {numero} : Modifica el buffer al tam indicado \n");   
    printf("------------------------------------------------------------- \n");
}


/** 
 * Consume los "count" primeros elementos. No se eliminan los repetidos
 */
static inline int consumirElementos(int count)
{
    int tmpBufSize = 256;
    char tmpBuf[tmpBufSize];
    int respuesta = 0;   
    char message [100];
    snprintf(tmpBuf, tmpBufSize,"%d\n",count);
    respuesta = ioctl(fd_fichero, IOCTL_CONSUMIR, tmpBuf);    

    //Se imprimen los elementos consumidos
    int i;
    for (i = 0; i < count; i++)   {
        printf("%i;", tmpBuf[i]); 
    }
    printf("\n"); 

    return respuesta;
} 



static inline void modificar_tamanyo_pila(int cantidad)
{
    ioctl(fd_fichero, IOCTL_MAX_STACK_SIZE, cantidad);
    printf("Nuevo tamanyo pila => %d \n", cantidad);
	return;
}



/**
 * Escribe el texto de entrada en el fichero
 */
static inline void escribir_fichero(char * elementos)
{
    int respuesta;
  
    respuesta = write(fd_fichero,elementos,strlen(elementos));

    if(respuesta < 0){
        printf("error : ejecucion, escribir fichero texto=>%s\n", elementos);
        exit(-6);
    }

    return;
}


/** 
 * Imprime el fichero
 */
static int leer_fichero(int count)
{    
    char bufTmp[TAM_BUFFER];
    int respuesta = 0;   
    
    respuesta = read(fd_fichero,bufTmp,count);
    //respuesta = read(fd_fichero,buffer,count);
    posicion += respuesta;
    
    if(respuesta < 0){
        printf("error : ejecucion, lectura fichero \n");
        exit(-7);
    }   

    printf("Leido: %s\n", bufTmp);
    return respuesta;
}   

