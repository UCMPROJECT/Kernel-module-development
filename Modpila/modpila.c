#include "modpila.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Sanzo");





static dev_t num_mayor_menor; 
int num_mayor;
static unsigned int num_dispositivos = 1; 
static int num_menor = 0;
static struct cdev * driver_struct;
static struct class * driver_class;

static const struct file_operations driver_fops = {
	.owner = THIS_MODULE,
	.read = modpila_read,
	.write = modpila_write,
	.open = modpila_open,
	.release = modpila_release,
    .llseek = modpila_lseek,
    .unlocked_ioctl = modpila_ioctl
    //.compat_ioctl = modpila_ioctl kernel~2.6
};



DEFINE_SEMAPHORE(semaforoPilaLectura);
DEFINE_SEMAPHORE(semaforoPilaEscritura);
/*  Init list*/
LIST_HEAD(integer_list);
    
/* Linked list */
struct list_head integer_list; 
static int maximoNumeroElementos = 15;

/* Integer list nodes */
struct list_item_t {
    int data;
    struct list_head links;
};
static int abierto;

// wait queues
static wait_queue_head_t myreadq;
static wait_queue_head_t mywriteq;
static int tamanyoActualPila = 0;



/* ********************* */
/* Funciones init y exit */
/* ********************* */

static int __init modpila_iniciar(void)
{

    // inside the module_init function
    init_waitqueue_head(&myreadq); 
    init_waitqueue_head(&mywriteq);

    
    // 2) registramos numero mayor y menor	
    alloc_chrdev_region(&num_mayor_menor, num_menor, num_dispositivos, NOMBRE_DEV);
	
    // 3) inicializacion struct cdev *driver_struct
    driver_struct = cdev_alloc();
	cdev_init(driver_struct, &driver_fops);
	cdev_add(driver_struct, num_mayor_menor, num_dispositivos);
	
    // 4) Crear nodo de dispositivo
    driver_class = class_create(THIS_MODULE, NOMBRE_DEV);
    device_create(driver_class, NULL, num_mayor_menor, NULL, NOMBRE_DEV);

    // 5) inicializamos variables    
    num_mayor = MAJOR(num_mayor_menor);

    
    
    printk(KERN_INFO "Dispostivo de caracteres %s \n", NOMBRE_DEV);
	return 0;
}

static void __exit modpila_eliminar(void)
{
    // 1) borramos struct cdev *driver_struct
	cdev_del(driver_struct);
    
    // 2) des-registramos numero mayor y menor 
	unregister_chrdev_region(num_mayor_menor, num_dispositivos);

    // 3) destruir en nodo de dispositivo
    device_destroy(driver_class, num_mayor_menor);
    class_destroy(driver_class);

    printk(KERN_INFO "\nliberado registro de caracteres\n");
    return; 
}


/* ************************* */
/* Funciones file_operations */
/* ************************* */

static int modpila_open(struct inode *inode, struct file *file)
{
    abierto = 1;

    return 0;
}

static int modpila_release(struct inode *inode, struct file *file)
{
    abierto = 0;
	return 0;
}

/** 
 * @param struct file * file: fichero del que leemos
 * @param char * buf: buffer del que leemos
 * @param size_t lbuf: longitud del buffer
 * @param loff_t * ppos: posicion donde empezamos a leer
 */
static ssize_t modpila_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
    /*
     * Number of bytes actually written to the buffer 
     */

    struct list_item_t *topOfStack;
    int tmpBufSize = 256;
    int sz, numeroOcurrencias, topOfStackNumber;
    char tmpBuf[tmpBufSize];

    
    if (ppos > 0 && abierto){ //Para que no se lea sin fin
        if (down_interruptible(&semaforoPilaLectura)){
            return -EINTR;
        }

        //Se extrae el nodo cima y se calcula las veces que aparece en la pila
        topOfStack = list_first_entry(&integer_list, struct list_item_t, links);
        topOfStackNumber = topOfStack->data;
        numeroOcurrencias = delete_node(topOfStack->data, &integer_list);

        if (numeroOcurrencias){
            printk(KERN_INFO "Top of stack is %d", topOfStackNumber);
            printk(KERN_INFO "All ocurrences of %d will be deleted", topOfStackNumber);
            display(&integer_list);    
        }
        else{
            printk(KERN_INFO "Empty stack");
        }

        if (numeroOcurrencias){
            sz = snprintf(tmpBuf, tmpBufSize,"%d;%d\n",topOfStackNumber, numeroOcurrencias );    
        }
        else{
            sz = snprintf(tmpBuf, tmpBufSize,"Stack empty\n");       
        }
        

        //Debug
        display(&integer_list);
        
        memcpy(buf, tmpBuf, sz);
        
        
        abierto = 0;  
        tamanyoActualPila -= numeroOcurrencias;

        printk(KERN_INFO "modpila_read => nbytes=%d", sz);

        //Avisar de que se ha consumido un elemento (o más), 
        // con lo cual se habilitan huecos
        wake_up_interruptible(&mywriteq);
        up(&semaforoPilaLectura);    
        return sz;          
    }
    
    return 0;
    
}

static ssize_t modpila_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos)
{    
    printk(KERN_INFO "Modpila WRITE ");

    char * tmpBuf;
    int tamanyoTmpBuf = 256;
    tmpBuf = kmalloc(tamanyoTmpBuf, GFP_KERNEL);

    /* write data to the buffer */
    int nbytes = copy_from_user(tmpBuf, buf, lbuf);
    
    char * ptr = tmpBuf;

    int numerosLeidos = 0;
    int num, n;
    int caracteres_escritos = 0;
    char field [ 32 ];

    //SEMAFORO
    if (down_interruptible(&semaforoPilaEscritura)){
        return -EINTR;
    }
    while ( sscanf(ptr, "%i;%n", &num, &n) == 1 ){ 
        if (tamanyoActualPila >= maximoNumeroElementos){
            printk(KERN_INFO "No hay espacio en la pila maximoNumeroElementos: %d, tamanyoActualPila actual: %d\n", maximoNumeroElementos, tamanyoActualPila);
            if ( wait_event_interruptible( mywriteq, tamanyoActualPila < maximoNumeroElementos )) {
                printk(" read signal ...\n");
                return -ERESTARTSYS;
            }        
        }

        tamanyoActualPila += 1;
        numerosLeidos += 1;
        caracteres_escritos += n;
        ptr += n; 
        tmpBuf += n;

        add_node(num,&integer_list);

        //Avisar de que hay un elemento nuevo para consumir
        wake_up_interruptible(&myreadq);
    }
    //SEMAFORO
    up(&semaforoPilaEscritura);

    kfree(tmpBuf);

    display(&integer_list);
   
    return caracteres_escritos;
}


/* *************** */
/* Funciones IOCTL */
/* *************** */

extern int modpila_ioctl
(struct file * file, unsigned int ioctl_num, unsigned long ioctl_param)
{
    loff_t * ppos = vmalloc(sizeof(int));
    char * buffer_entrada;
    unsigned int long_buffer;

 
    *ppos = 0;
    buffer_entrada = NULL;
    long_buffer = 0;
    switch(ioctl_num){
        case IOCTL_MAX_STACK_SIZE:
            modpila_set_max(file, (int) ioctl_param);
            break;
        case IOCTL_CONSUMIR:
            modpila_consumir(file, (char*)ioctl_param);
            break;

        default: return -ENOTTY;
    }

    return 0;
}


/*
    Devuelve en buff_intermedio un string con los números consumidos.
*/

static int modpila_consumir(struct file * fichero, char* buff_intermedio){
    
    int top, sz;    
    int tmpBufSize = 256;    
    int contador = 0;
    int length = 0;
    int numeroElementos;
    char tmpBuf[tmpBufSize];


    sscanf(buff_intermedio, "%i", &numeroElementos);

    while (contador < numeroElementos){
        if (tamanyoActualPila == 0){
            wait_event_interruptible( myreadq, tamanyoActualPila > 0 );
        }

        if (down_interruptible(&semaforoPilaLectura)){
            return -EINTR;
        }

        //Se elimina la cima, y se obtiene su valor
        top = deleteTopOfStack(&integer_list);
        up(&semaforoPilaLectura);


        tamanyoActualPila--;
        wake_up_interruptible(&mywriteq);
        buff_intermedio[contador] = top;

        contador++;
        sz = snprintf(tmpBuf + length, tmpBufSize - length,"%d;",top);
        length += sz;
    }    

    return numeroElementos;
}

/**
 * Cambia maximo numero de elementos en pila. 
 */
static void modpila_set_max(struct file * fichero, int nuevoMaximo)
{   
    printk(KERN_INFO "Antes el maximo numero de elementos era: %d", maximoNumeroElementos);
    maximoNumeroElementos = nuevoMaximo;
    printk(KERN_INFO "Antes el maximo numero de elementos ahora es: %d", maximoNumeroElementos);
    return;
}

/**
 * Posiciona el puntero
 */
static loff_t modpila_lseek(struct file *file, loff_t pos,int whence)
{	
	return 0;
}





void add_node(int arg, struct list_head * head){
/*
    Adds node with "arg" value to the list.
*/

    struct list_item_t * newNode = (struct  list_item_t *)vmalloc(sizeof(struct list_item_t));
    if (newNode == NULL){
        printk(KERN_INFO "Something went wrong");
    }
    
    newNode->data = arg;
    INIT_LIST_HEAD(&newNode->links);    
    list_add(&newNode->links, head); //Adds it to beginning
}


int delete_node(int arg, struct list_head * head){
/*
    Deletes all ocurrences of "arg" within the list.
    Returns number of elements with "arg" value.
*/
    struct list_head *iter;
    struct list_item_t *objPtr;
    struct list_head * q;

    int count = 0;
    printk(KERN_INFO "Deleting all items with value: %d", arg);
    list_for_each_safe(iter, q, &integer_list){
        objPtr = list_entry(iter, struct list_item_t, links);
        if (arg == objPtr->data){
            printk(KERN_INFO "Freeing item with value: %d", objPtr->data);
            list_del(iter); //Delete item from list
            vfree(objPtr); //Free memory associated with element
            count++;
        }       
    }   

    return count;
}

/*
    Returns top of stack.
*/

int deleteTopOfStack(struct list_head * head){
    struct list_item_t *topOfStack;
    topOfStack = list_first_entry(&integer_list, struct list_item_t, links); 
    int top = topOfStack->data;
    list_del(&topOfStack->links); //Delete item from list
    vfree(topOfStack); //Free memory associated with element
    return top;
}

void delete_list(struct list_head * head){
/*
Deletes all nodes of the list.
*/
    struct list_head *iter;
    struct list_item_t *objPtr;
    struct list_head * q;

    printk(KERN_INFO "Deleting all items");
    list_for_each_safe(iter, q, &integer_list){
        objPtr = list_entry(iter, struct list_item_t, links);
        printk(KERN_INFO "Freeing item with value: %d", objPtr->data);
        list_del(iter); //Delete item from list
        vfree(objPtr); //Free memory associated with element
    }
}

void display(struct list_head *head)
/*
    Prints to /var/log/messages elements of the list .
*/
{
    struct list_head *iter;
    struct list_item_t *objPtr;

    printk(KERN_INFO "--Listing inserted numbers--");

    int contador = 0;
    list_for_each(iter, head) {
        objPtr = list_entry(iter, struct list_item_t, links);
        if (contador == 0){
            printk(KERN_INFO "%d <--------- Top of stack", objPtr->data);    
        }
        else{
            printk(KERN_INFO "%d ", objPtr->data);    
        }
        contador++;
    }
    printk(KERN_INFO "--END OF STACK--");
}

int getSize(struct list_head *head)
/*
    Prints to /var/log/messages elements of the list .
*/
{
    struct list_head *iter;
    struct list_item_t *objPtr;
    int contador = 0;
    list_for_each(iter, head) {
        objPtr = list_entry(iter, struct list_item_t, links);
        printk(KERN_INFO "%d ", objPtr->data);
        contador++;
    }
    return contador;
}


module_init(modpila_iniciar);
module_exit(modpila_eliminar);