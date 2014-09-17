#ifndef LISTAS_H
#define LISTAS_H

#include "listas.h"
#include <linux/list.h>
#include <linux/kernel.h>           /* kernel */
#include <linux/module.h>           /* module */
#include <linux/workqueue.h>        /* workqueue_struct */
#include <linux/slab.h>         /* kmalloc */
#include <linux/vmalloc.h>      /* vmalloc */

void add_node(int arg, struct list_head * head);

int delete_node(int arg, struct list_head * head);

struct list_item_t * deleteTopOfStack(struct list_head * head);

void delete_list(struct list_head * head);

void display(struct list_head *head);

int getSize(struct list_head *head);

#endif