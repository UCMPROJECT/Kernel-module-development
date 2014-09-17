#include "tester.h"

/** Variables globales */
int fd_fichero;
int fd_fichero_config;

int no_flag;

//Opciones
int activar_mode;
int desactivar_mode;
int threshold_mode;
int period_mode;

void inline limpiar_flags(void)
{    
    desactivar_mode = 0;
    activar_mode = 0;
    threshold_mode = 0;
    period_mode = 0;
}

/** Programa de testeo:
 * 
 * 1) Abre el fichero definido en funciones_ioctl.h y reserva un buffer
 * 2) Consume los flags de entrada rwlRmh
 * 3) Espera como entrada las siguientes opciones
 *      ./app -r
 *      ./app -R
 *      ./app -h
 *      ./app -s {numero}
 *		./app -c {numero}
 *		./app -e {numero}
 *      ./app -w cadena
 *      ./app -m numero
 * 4) cierra el fichero
 */
int main (int argc, char **argv)
{
    int optc;
    limpiar_flags();


    // abrimos el fichero /proc/modconfig
    fd_fichero_config = open(RUTA_FICHERO_CONFIG, O_RDWR);
    if (fd_fichero_config == -1){
        perror("error : inicio, abrir fichero config"),exit(-1);
    }

    // abrimos el fichero dev/modtimer
    fd_fichero = open(RUTA_FICHERO, O_RDWR);
    if (fd_fichero == -1){
        perror("error : inicio, abrir fichero modtimer"),exit(-1);
    }

    
    if(argc == 1){
        no_flag = 1;
    }

    // Consumimos flags
    while ((optc = getopt (argc, argv, "pthad")) != -1){
        switch (optc){
			case 'p': // Operacion Con Timer_period
                period_mode = 1;
				break;
            case 't': // Operacion Con Emergency_threshold
                threshold_mode = 1;
                break;
            case 'h': // HELP
                mostrar_ayuda();
                break;
            case 'a': // Activar Temporizador
                activar_mode = 1;
                break;
            case 'd': // Desactivar Temporizador
                desactivar_mode = 1;
                break;
			default : 
                no_flag = 1;
				break;
		}
    }

    // optind contiene el numero de argumentos no flags
    switch (argc - optind){
        case 0:	// $> tester_app [opciones] 
            if (no_flag){
                consultar();            
            }else if(activar_mode){
                activarTimer();
            }
            else if(desactivar_mode){
                desactivarTimer();
            }            
            else {
                warnx("opcion incorrecta \n");
                exit(-3);
            }
            break;

        case 1:	// $> test [opciones] argumento
            if(threshold_mode){
                modificar(1, (int) atoi(argv[2]));
            } 
            else if(period_mode){
                modificar(2, (int) atoi(argv[2]));            
            }else {
                warnx("opcion incorrecta \n");
                exit(-4);
            };
            break;

        default:	
            warnx ("operandos incorectos\n");
            exit(-5);
   }
	
	// Se cierran los ficheros
    close(fd_fichero);
    close(fd_fichero_config);

    // Salir de la aplicacion
    exit(0);
}

/** Funciones */

static void mostrar_ayuda(void)
{
    printf("uso: [OPCIONES] [ARGUMENTO]\n");
    printf("./tester_app -t {numero} : Fija el emergency_threshold a {numero}. 0 < numero < 100\n");    
    printf("./tester_app -p {numero} : Fija el timer_period a {numero}\n");    
    printf("./tester_app -h : Mostrar ayuda.\n");
    printf("./tester_app -a : Activa el timer.\n");
    printf("./tester_app -d : Desactiva el timer.\n");
    printf("------------------------------------------------------------- \n");
}


static void activarTimer(){
    ioctl(fd_fichero, IOCTL_ACTIVAR, NULL);
    printf("Se ha activado el timer\n");
    return;
}

static void desactivarTimer(){
    ioctl(fd_fichero, IOCTL_DESACTIVAR, NULL);
    printf("Se ha desactivado el timer\n");
    return;
}

/**
 * Imprime el valor de emergency_threshold y de timer_period
 */    
static void consultar()
{    
    int tamanyoBuffer = 256;
    int valor, respuesta;
    char resultado[tamanyoBuffer];
    char imprimir[tamanyoBuffer];
    respuesta = read(fd_fichero_config, resultado, tamanyoBuffer);
    snprintf(imprimir,respuesta, resultado);
    printf("%s\n", imprimir);
    return;
}



/**
 * Modifica el valor indicado
 */    
static void modificar(int modo, int valor)
{    
    char * texto;
    snprintf(texto, 256, "%i;%i.\n", modo, valor);
    write(fd_fichero_config, texto, strlen(texto));    
    return;
}




