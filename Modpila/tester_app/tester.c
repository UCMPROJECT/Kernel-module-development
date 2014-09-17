#include "tester.h"

/** Variables globales */
char * buffer;
int fd_fichero;
int posicion;

/** Flags */
int modify_mode;
int consum_mode;

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
    int tam_fichero;
    limpiar_flags();

    // abrimos el fichero
    fd_fichero = open(RUTA_FICHERO, O_RDWR);
    if (fd_fichero == -1){
        perror("error : inicio, abrir fichero"),exit(-1);
    }

    // Consumimos flags
    while ((optc = getopt (argc, argv, "krwsceRmh")) != -1){
        switch (optc){
            case 'm': // Modificar tamaño buffer
                modify_mode = 1;
                break;
            case 'k': //Consumir elementos
                consum_mode = 1;
                break;
            case 'h': // HELP
                mostrar_ayuda();
                return;
			default : 
				break;
		}
    }

    // optind contiene el numero de argumentos no flags
    switch (argc - optind){
        case 1:	// $> test [opciones] argumento
            if (modify_mode) {
                modificar_tamanyo_pila((int) atoi(argv[2]));
            }
            else if(consum_mode){
                consumirElementos((int) atoi(argv[2]));
            }else {
                warnx("opcion incorrecta \n");
                exit(-4);
            };
            break;

        default:	
            warnx ("operandos incorectos\n");
            exit(-5);
   }

	// Liberamos el buffer
	free(buffer);
	
	// Cerrar el fichero
    close(fd_fichero);

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

