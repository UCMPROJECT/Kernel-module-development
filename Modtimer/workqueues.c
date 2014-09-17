#include "workqueues.h"


MODULE_LICENSE("GPL");

/**
 * Funcion de callback al procesar un nodo 
 * @param nodo a procesar 
 */
void procesar_nodo(struct work_struct * nodo)
{
    // 1) Hacemos el casting a nuestra definicion de estructura
    workqueue_struct_timer * n;
    n = (workqueue_struct_timer *) nodo;
    
    struct list_head * lista1 = n->head;
    struct list_head * lista2 = n->lista;

    int numero = n->numeroAleatorio;

    add_node(numero, lista1); //lista pares o impares
    add_node(numero, lista2); //lista general
    // 3) Liberamos los recursos
    kfree( (void *) n );    

    return;
}

/** 
 * @param (struct nodo_cola *) en esta estructura
 * @param int emergency_threshold . Informacion a tener en cuenta para vaciado de buffer
 * @param struct list_head * {listas}. Listas en la que insertar los elementos
 * @return (int) flag errores
 */

int encolar_tarea(struct workqueue_struct * cola, int emergency_threshold,  buffer_circular_t * bufCirc, struct list_head * listaAleatorios, struct list_head * listaPares, struct list_head * listaImpares)
{
    int error = 0;
    
    // 1) Reservamos memoria para un nuevo nodo
    workqueue_struct_timer * nodo;
    nodo = (workqueue_struct_timer *) kmalloc( sizeof(workqueue_struct_timer), GFP_ATOMIC);
    
    if (nodo) {
        
        // 2) Registramos la funcion
        INIT_WORK( (struct work_struct *) nodo, procesar_nodo );
        // 3) Rellenamos la estructura

        //Se comprueba que el buffer circular no haya superado el umbral 
        int tamanyoBufferCircular = QueueSize(bufCirc);
        if ( tamanyoBufferCircular * 100 >= (emergency_threshold * 10)){
          QueueInit(bufCirc);
        }

        int ultimo;
        if ( tamanyoBufferCircular > 0){
          QueueGet(bufCirc, &ultimo);
          nodo->numeroAleatorio = ultimo;
          nodo->lista = listaAleatorios;
          if (ultimo % 2 == 0){
            nodo->head = listaPares;  
          }
          else if (ultimo % 2 == 1){
            nodo->head = listaImpares;
          }  
        }
        
        // 4) Encolamos el nodo
        error = queue_work(cola, (struct work_struct *) nodo);

    }
    
    return error;
}


