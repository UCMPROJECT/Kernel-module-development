#ifndef WORKQUEUES_H
#define WORKQUEUES_H
 
#include <linux/kernel.h>           /* kernel */
#include <linux/module.h>           /* module */
#include <linux/workqueue.h>        /* workqueue_struct */
#include <linux/slab.h>		    /* kmalloc */
#include <linux/vmalloc.h>      /* vmalloc */

#include "listas.h"
#include "Queues.h"


typedef struct  {
  struct work_struct work;
  int numeroAleatorio;
  struct list_head * head;
  struct list_head * lista;
}workqueue_struct_timer;
   
/* Funciones */
void procesar_nodo(struct work_struct * nodo);
int encolar_tarea(struct workqueue_struct * cola, int emergency_threshold,  buffer_circular_t * bufCirc, struct list_head * listaAleatorios, struct list_head * listaPares, struct list_head * listaImpares);

#endif /* WORKQUEUES_H */