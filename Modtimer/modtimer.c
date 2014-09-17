#include "modtimer.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Sanzo");

module_init(iniciarModulo);
module_exit(limpiarModulo);

//PARAMETROS TIMER
struct timer_list exp_timer;
static int timer_period = 300;


//PARAMETROS DISPOSITIVOS
int dispositivoTimerAbierto;
int dispositivoParesAbierto;
int dispositivoImparesAbierto;
int num_dispositivos = 1;
static dev_t num_mayor_menorTimer; 
static dev_t num_mayor_menorPares; 
static dev_t num_mayor_menorImpares; 
int num_mayor_Timer;
int num_mayor_Pares;
int num_mayor_Impares;
static struct cdev * driver_structTimer;
static struct class * driver_classTimer;
static struct cdev * driver_structPares;
static struct class * driver_classPares;
static struct cdev * driver_structImpares;
static struct class * driver_classImpares;


//LOG
size_t discosize;
static char * logBuffer; 
static int posicionlogBuffer;
char * buffPtr;

struct list_head listaAleatorios; 
struct list_head listaImpares; 
struct list_head listaPares; 


/* Integer list nodes */
struct list_item_t {
    int data;
    struct list_head links;
};


//PARAMETROS BUFFER CIRCULAR
buffer_circular_t bufferCircular;
static int emergency_threshold = 60;


static int activo;

/*  Init list*/
LIST_HEAD(listaAleatorios);
LIST_HEAD(listaPares);
LIST_HEAD(listaImpares);

DEFINE_SEMAPHORE(semaforoAleatoriosEscritura);
//DEFINE_SEMAPHORE(semaforoAleatoriosLectura); No hace falta. Por enunciado, es un único proceso el que lee

struct workqueue_struct * cola_inserciones;


static const struct file_operations fopsTimer = {
	.owner = THIS_MODULE,
	.read = modtimer_read,
	.write = modtimer_write,
	.open = modtimer_open,
	.release = modtimer_release,
    .llseek = modtimer_lseek,
    .unlocked_ioctl = modtimer_ioctl
};

static const struct file_operations fopsPares = {
	.owner = THIS_MODULE,
	.read = modtpares_read,
	.write = modtpares_write,
	.open = modtpares_open,
	.release = modtpares_release,
    .llseek = modtpares_lseek,
    .unlocked_ioctl = modtimer_ioctl
};

static const struct file_operations fopsImpares = {
	.owner = THIS_MODULE,
	.read = modtimpares_read,
	.write = modtimpares_write,
	.open = modtimpares_open,
	.release = modtimpares_release,
    .llseek = modtimpares_lseek,
    .unlocked_ioctl = modtimer_ioctl
};


static void do_something(unsigned long data)
{
	


	int ultimo;
	if (activo){
		//printk(KERN_INFO "Your timer expired and app has been called\n");  
		int num = jiffies & 0xff;
		//printk(KERN_INFO "Número aleatorio: %d\n", num);

		if (down_interruptible(&semaforoAleatoriosEscritura)){
			return -EINTR;
		}



		int sz;
		sz = snprintf(logBuffer + posicionlogBuffer, 256, "Generado: %d\n", num);
		posicionlogBuffer += sz;
		QueuePut(&bufferCircular, num);
		if ( ! encolar_tarea(cola_inserciones, emergency_threshold, &bufferCircular, &listaAleatorios, &listaPares, &listaImpares) ){
			printk(KERN_INFO "Error al encolar numero: %d", num);
		}		
	}



	up(&semaforoAleatoriosEscritura);

	setTimer();	

	return;
}


static void setTimer(){
	init_timer_on_stack(&exp_timer);

    exp_timer.expires = jiffies + timer_period ;// * HZ;
    exp_timer.data = 0;
    exp_timer.function = do_something;

    add_timer(&exp_timer);
}

/**
 * ESCRIBIR INFO
 */
int iniciarModulo(void)
{
    int error = 0; 
    printk(KERN_INFO "EN MODULO INIT DE CLIPBOARD, nombre = %s \n", __this_module.name);   
     
 	//Creacion buffer circular
 	QueueInit(&bufferCircular);
    setTimer();

    
    //Se activa el temporizador    
    activo = 1; 
    // Inicializamos la cola de tareas
	cola_inserciones = create_workqueue("cola_inserciones");


	// Se reserva memoria para el log	
    discosize = (size_t) (32*PAGE_SIZE);    
    logBuffer = kmalloc(discosize, GFP_KERNEL);
    posicionlogBuffer = 0;

        
    crearProcFiles();

    crearDispositivos();

    
    return 0;
}


/**
 * 
 INSERTAR INFO
 */
void limpiarModulo(void)
{
	activo = 0;  	

	//Eliminacion dispositivos modtimer, modtpares y modtimpares
	eliminarDispositivos();
	//Eliminacion archivos /proc/modtimer y /proc/modconfig
	eliminarProcFiles();

  	delete_list(&listaAleatorios);
  	delete_list(&listaPares);	
  	delete_list(&listaImpares);


	flush_workqueue(cola_inserciones);
    destroy_workqueue(cola_inserciones);  	

	del_timer(&exp_timer);  
    printk(KERN_INFO "Exit called\n");
}


//Operaciones sobre archivos /proc

int leerLog(char *bufferSalida, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data){
	int terminado;

   
    printk(KERN_INFO "Leer log.");
    /*
	 * Number of bytes actually written to the buffer 
	 */
	int bytes_read;
	bytes_read = sprintf(bufferSalida, logBuffer);

    return bytes_read;
}

int escribirLog(struct file *file, const char *buffer, unsigned long count, void *data)
{
	//No se puede escribir al log, solo leer.
    return -EINVAL;
}


int leerModConfig(char *bufferSalida, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data){
	int terminado;

   
    printk(KERN_INFO "Leer ModConfig.");

	int tmpBufSize = 64;
	char tmpBuf[tmpBufSize];
	int sz;
	sz = snprintf(tmpBuf, tmpBufSize,"Timer period: %d\nEmergency threshold: %d\n",timer_period, emergency_threshold);   	

	//Bytes leidos realmente
	int bytes_read;	
	//Se copia al buffer de salida
	bytes_read = sprintf(bufferSalida, tmpBuf);

    return bytes_read;
}


int escribirModConfig(struct file *file, const char *buffer, unsigned long count, void *data)
{	
	int mode, value;
	sscanf(buffer, "%i;%i", &mode, &value);
	printk(buffer);
	printk("Mode: %i; Value: %i", mode, value);
	if (mode == 1){
		emergency_threshold = value;
	}
	else if (mode == 2){
		timer_period = value;
	}
	else{
		return -EINVAL;	
	} 
	return 1;   
}


//FUNCIONES FOPS

static ssize_t modtimer_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{	

    struct list_item_t *elementoSiguiente;
    int tmpBufSize = 256;
    int sz, numberOcurrences;
    char tmpBuf[tmpBufSize];

    
    if (dispositivoTimerAbierto){

    	elementoSiguiente = deleteTopOfStack(&listaAleatorios);

    	int elementoSiguienteNumero = elementoSiguiente->data;
    	vfree(elementoSiguiente); //Es un nuevo elemento, para poder borrar el ultimo de la cola

    	sz = snprintf(tmpBuf, tmpBufSize,"%d\n",elementoSiguienteNumero);        	
    	int bytes_leidos = sz;
    	sz = snprintf(logBuffer + posicionlogBuffer, 30, "Leido de lista general: %d\n", elementoSiguienteNumero);
    	buffPtr = logBuffer;
    	posicionlogBuffer += sz;  
    	memcpy(buf, tmpBuf, sz);
        
    	dispositivoTimerAbierto = 0;  

    	return bytes_leidos;          
    }

    return 0;    
}

static ssize_t modtimer_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{       
    return lbuf;
}

static int modtimer_open(struct inode *inode, struct file *file)
{
    dispositivoTimerAbierto = 1;
    return 0;
}

static int modtimer_lseek(struct inode *inode, struct file *file)
{
    return 0;
}

static int modtimer_release(struct inode *inode, struct file *file)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
static ssize_t modtpares_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{	

    struct list_item_t *elementoSiguiente;
    int tmpBufSize = 256;
    int sz, numberOcurrences;
    char tmpBuf[tmpBufSize];

    
    if (dispositivoParesAbierto){

    	elementoSiguiente = deleteTopOfStack(&listaPares);

    	int elementoSiguienteNumero = elementoSiguiente->data;
		vfree(elementoSiguiente); //Es un nuevo elemento, para poder borrar el ultimo de la cola

    	sz = snprintf(tmpBuf, tmpBufSize,"%d\n",elementoSiguienteNumero);  
    	int bytes_leidos = sz;
    	sz = snprintf(logBuffer + posicionlogBuffer, 30, "Leido de pares: %d\n", elementoSiguienteNumero);
    	buffPtr = logBuffer;
    	posicionlogBuffer += sz;      	
    	memcpy(buf, tmpBuf, sz);
        
        
    	dispositivoParesAbierto = 0;  

    	return bytes_leidos;          
    }

    return 0;    
}

static ssize_t modtpares_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{    
    return lbuf;
}

static int modtpares_open(struct inode *inode, struct file *file)
{
    dispositivoParesAbierto = 1;
    return 0;
}

static int modtpares_lseek(struct inode *inode, struct file *file){
	return 0;
}

static int modtpares_release(struct inode *inode, struct file *file)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
static ssize_t modtimpares_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{	

    struct list_item_t *elementoSiguiente;
    int tmpBufSize = 256;
    int sz, numberOcurrences;
    char tmpBuf[tmpBufSize];

    
    if (dispositivoImparesAbierto){

		elementoSiguiente = deleteTopOfStack(&listaImpares);
    	int elementoSiguienteNumero = elementoSiguiente->data;
    	vfree(elementoSiguiente); //Es un nuevo elemento, para poder borrar el ultimo de la cola

    	sz = snprintf(tmpBuf, tmpBufSize,"%d\n",elementoSiguienteNumero);   
    	int bytes_leidos = sz;
    	sz = snprintf(logBuffer + posicionlogBuffer, 22, "Leido de impares: %d\n", elementoSiguienteNumero);
    	buffPtr = logBuffer;
    	posicionlogBuffer += sz;
    	memcpy(buf, tmpBuf, sz);
        
        
    	dispositivoImparesAbierto = 0;  

    	return bytes_leidos;          
    }

    return 0;    
}


static int modtimpares_lseek(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t modtimpares_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{    
    return lbuf;
}

static int modtimpares_open(struct inode *inode, struct file *file)
{
    dispositivoImparesAbierto = 1;
    return 0;
}

static int modtimpares_release(struct inode *inode, struct file *file){
	return 0;
}

//FUNCIONES AUXILIARES

void crearProcFiles(){

	//              /proc/modtimer
	struct proc_dir_entry * entradaModTimer = create_proc_entry(NOMBRE_PROC_TIMER,  S_IFREG | S_IRUGO | S_IWOTH | S_IWGRP | S_IWUSR, NULL);
	entradaModTimer->read_proc = leerLog;
	entradaModTimer->write_proc = escribirLog;
	entradaModTimer->uid = 0; // id usuario 
    entradaModTimer->gid = 0; // id grupo

    //           /proc/modconfig
    struct proc_dir_entry * entradaModConfig = create_proc_entry(NOMBRE_PROC_CONFIG,  S_IFREG | S_IRUGO | S_IWOTH | S_IWGRP | S_IWUSR, NULL);
	entradaModConfig->read_proc = leerModConfig;
	entradaModConfig->write_proc = escribirModConfig;
	entradaModConfig->uid = 0; // id usuario 
    entradaModConfig->gid = 0; // id grupo
}

void eliminarProcFiles(){
	remove_proc_entry(NOMBRE_PROC_TIMER, NULL);
	remove_proc_entry(NOMBRE_PROC_CONFIG, NULL);
}


void crearDispositivos(){
	printk(KERN_INFO "Creando dispositivos.");

	//********************modtimer*********************************
    
    // 2) registramos numero mayor y menor	
    alloc_chrdev_region(&num_mayor_menorTimer, 0, num_dispositivos, NOMBRE_DEV_TIMER);
	
    // 3) inicializacion struct cdev *driver_struct
    driver_structTimer = cdev_alloc();
	cdev_init(driver_structTimer, &fopsTimer);
	cdev_add(driver_structTimer, num_mayor_menorTimer, num_dispositivos);
	
    // 4) Crear nodo de dispositivo
    driver_classTimer = class_create(THIS_MODULE, NOMBRE_DEV_TIMER);
    device_create(driver_classTimer, NULL, num_mayor_menorTimer, NULL, NOMBRE_DEV_TIMER);	

    num_mayor_Timer = MAJOR(num_mayor_menorTimer);

    
    //********************modtpares*********************************

    // 2) registramos numero mayor y menor	
    alloc_chrdev_region(&num_mayor_menorPares, 0, num_dispositivos, NOMBRE_DEV_PARES);
	
    // 3) inicializacion struct cdev *driver_struct
    driver_structPares = cdev_alloc();
	cdev_init(driver_structPares, &fopsPares);
	cdev_add(driver_structPares, num_mayor_menorPares, num_dispositivos);
	
    // 4) Crear nodo de dispositivo
    driver_classPares = class_create(THIS_MODULE, NOMBRE_DEV_PARES);
    device_create(driver_classPares, NULL, num_mayor_menorPares, NULL, NOMBRE_DEV_PARES);	

    num_mayor_Pares = MAJOR(num_mayor_menorPares);


    //********************modtimpares*********************************

    // 2) registramos numero mayor y menor	
    alloc_chrdev_region(&num_mayor_menorImpares, 0, num_dispositivos, NOMBRE_DEV_IMPARES);
	
    // 3) inicializacion struct cdev *driver_struct
    driver_structImpares = cdev_alloc();
	cdev_init(driver_structImpares, &fopsImpares);
	cdev_add(driver_structImpares, num_mayor_menorImpares, num_dispositivos);
	
    // 4) Crear nodo de dispositivo
    driver_classImpares = class_create(THIS_MODULE, NOMBRE_DEV_IMPARES);
    device_create(driver_classImpares, NULL, num_mayor_menorImpares, NULL, NOMBRE_DEV_IMPARES);	

    num_mayor_Impares = MAJOR(num_mayor_menorImpares);
}

void eliminarDispositivos(){


	printk(KERN_INFO "Eliminando dispositivos.");

	//********************modtimer*********************************

	// 1) borramos struct cdev *driver_struct
	cdev_del(driver_structTimer);
    
    // 2) des-registramos numero mayor y menor 
	unregister_chrdev_region(num_mayor_menorTimer, num_dispositivos);

    // 3) destruir en nodo de dispositivo
    device_destroy(driver_classTimer, num_mayor_menorTimer);
    class_destroy(driver_classTimer);


    //********************modtpares*********************************

    // 1) borramos struct cdev *driver_struct
	cdev_del(driver_structPares);
    
    // 2) des-registramos numero mayor y menor 
	unregister_chrdev_region(num_mayor_menorPares, num_dispositivos);

    // 3) destruir en nodo de dispositivo
    device_destroy(driver_classPares, num_mayor_menorPares);
    class_destroy(driver_classPares);

    //********************modtimpares*********************************

    // 1) borramos struct cdev *driver_struct
	cdev_del(driver_structImpares);
    
    // 2) des-registramos numero mayor y menor 
	unregister_chrdev_region(num_mayor_menorImpares, num_dispositivos);

    // 3) destruir en nodo de dispositivo
    device_destroy(driver_classImpares, num_mayor_menorImpares);
    class_destroy(driver_classImpares);

    return; 
}




/* *************** */
/* Funciones IOCTL */
/* *************** */


extern int modtimer_ioctl
(struct file * file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    int char_leidos;
    loff_t * ppos = vmalloc(sizeof(int));
    char * buffer_entrada;
    unsigned int long_buffer;

    char_leidos = 0;    
    *ppos = 0;
    buffer_entrada = NULL;
    switch(ioctl_num){    
        case IOCTL_MODIFICAR_THRESHOLD: 
            modificar_threshold((int) ioctl_param);
            break;

        case IOCTL_MODIFICAR_TIMER_PERIOD:
            modificar_timer_period((int) ioctl_param);
            break;
		
        case IOCTL_CONSULTAR_VALOR:
            return consultar((int) ioctl_param);
            break;

        case IOCTL_DESACTIVAR:
            modtimer_desactivar();
            break;
        case IOCTL_ACTIVAR:
            modtimer_activar();
            break;

        default: return -ENOTTY;
    }

    return 0;
}

static void modtimer_desactivar(){
	activo = 0;
	int sz;
	sz = snprintf(logBuffer + posicionlogBuffer, 256, "Se ha desactivado el timer.\n");
	posicionlogBuffer += sz;
	printk(KERN_INFO "Se ha desactivado el timer.");
}

static void modtimer_activar(){
	activo = 1;
	int sz;
	sz = snprintf(logBuffer + posicionlogBuffer, 256, "Se ha activado el timer.\n");
	posicionlogBuffer += sz;
	printk(KERN_INFO "Se ha activado el timer.");
}

static void modificar_threshold(int nuevoValor){
	int sz;
	sz = snprintf(logBuffer + posicionlogBuffer, 256, "Se ha cambiado el emergency_threshold.\n");
	posicionlogBuffer += sz;
	sz = snprintf(logBuffer + posicionlogBuffer, 256, "Antes era: %i.", emergency_threshold);
	posicionlogBuffer += sz;	
	emergency_threshold = nuevoValor;
	sz = snprintf(logBuffer + posicionlogBuffer, 256, "Ahora es: %i", emergency_threshold);
	posicionlogBuffer += sz;
}
static void modificar_timer_period(int nuevoValor){
	int sz;
	sz = snprintf(logBuffer + posicionlogBuffer, 256, "Se ha cambiado el timer_period.\n");
	posicionlogBuffer += sz;
	sz = snprintf(logBuffer + posicionlogBuffer, 256, "Antes era: %i.", timer_period);
	posicionlogBuffer += sz;	
	timer_period = nuevoValor;	
	sz = snprintf(logBuffer + posicionlogBuffer, 256, "Ahora es: %i.", timer_period);
	posicionlogBuffer += sz;	
}

static int consultar(int modo){
	if (modo == 1){
		return timer_period;
	}
	else{
		return emergency_threshold;
	}
}
