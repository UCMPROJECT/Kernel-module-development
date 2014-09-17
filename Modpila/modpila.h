#ifndef PILA_H
#define PILA_H

#include <linux/module.h>	    /* modulo */
#include <linux/semaphore.h>	/* semaforos */
#include <linux/fs.h>		    /* file_operations */
#include <linux/uaccess.h>	    /* copy_to_user, copy_from_user */
#include <linux/init.h>		    /* module_init, module_exit */
#include <linux/slab.h>		    /* kmalloc */
#include <linux/vmalloc.h>      /* vmalloc */
#include <linux/cdev.h>		    /* cdev */
#include <linux/kthread.h>      /* kthread y waitqueues*/
#include <linux/device.h>       /* class_create */
#include <linux/wait.h>			/* waitqueues*/
#include "./funciones_ioctl.h"  /* funciones ioctl */


#define NOMBRE_THREAD "driver_thread"

/* Funciones init exit */
static int __init modpila_iniciar(void);
static void __exit modpila_eliminar(void);



/* Funciones file_operations */
static int modpila_open(struct inode *inode, struct file *file);
static int modpila_release(struct inode *inode, struct file *file);
static ssize_t modpila_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos);
static ssize_t modpila_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos);

/* Funciones IOCTL */
// extern int modpila_ioctl
// (struct inode * inode, struct file * file, unsigned int ioctl_num, unsigned long ioctl_param);
extern int modpila_ioctl
(struct file * file, unsigned int ioctl_num, unsigned long ioctl_param);
static loff_t modpila_lseek(struct file *file, loff_t pos,int whence);

static void modpila_set_max(struct file * fichero, int nuevoMaximo);
static int  modpila_consumir(struct file * fichero, char* valores);





//Funciones listas
void add_node(int arg, struct list_head * head);
int delete_node(int arg, struct list_head * head);
int deleteTopOfStack(struct list_head * head);
void delete_list(struct list_head * head);
void display(struct list_head *head);
int getSize(struct list_head *head);

#endif /* PILA_H */

