#ifndef TIMER_H
#define TIMER_H

#include <linux/module.h>	    /* modulo */

#include <linux/fs.h>		    /* file_operations */
#include <linux/uaccess.h>	    /* copy_to_user, copy_from_user */
#include <linux/init.h>		    /* module_init, module_exit */
#include <linux/slab.h>		    /* kmalloc */
#include <linux/vmalloc.h>      /* vmalloc */
#include <linux/cdev.h>		    /* cdev */
#include <linux/device.h>       /* class_create */
#include <linux/module.h>	/* modulo */
#include <linux/proc_fs.h>	/* struct proc_dir_entry */



#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/semaphore.h>	    /* semaforos */

#include "./workqueues.h"
#include "./funciones_ioctl.h"



/* Funciones file_operations */
static ssize_t modtimer_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos);
static ssize_t modtimer_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos);
static int modtimer_open(struct inode *inode, struct file *file);
static int modtimer_lseek(struct inode *inode, struct file *file);
static int modtimer_release(struct inode *inode, struct file *file);

static ssize_t modtpares_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos);
static ssize_t modtpares_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos);
static int modtpares_open(struct inode *inode, struct file *file);
static int modtpares_release(struct inode *inode, struct file *file);
static int modtpares_lseek(struct inode *inode, struct file *file);


static ssize_t modtimpares_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos);
static ssize_t modtimpares_write(struct file *file, const char __user * buf, size_t lbuf, loff_t * ppos);
static int modtimpares_open(struct inode *inode, struct file *file);
static int modtimpares_release(struct inode *inode, struct file *file);
static int modtimpares_lseek(struct inode *inode, struct file *file);

/* Funciones IOCTL */
// (struct inode * inode, struct file * file, unsigned int ioctl_num, unsigned long ioctl_param);
extern int modtimer_ioctl
(struct file * file, unsigned int ioctl_num, unsigned long ioctl_param);
static void modtimer_desactivar(void);
static void modtimer_activar(void);
static void modificar_threshold(int nuevoValor);
static void modificar_timer_period(int nuevoValor);
static int consultar(int modo);


//Funciones sobre /prod/modtimer y/proc/modconfig
int leerLog(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);
int escribirLog(struct file *file, const char *buffer, unsigned long count, void *data);

int leerModConfig(char *bufferSalida, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data);
int escribirModConfig(struct file *file, const char *buffer, unsigned long count, void *data);


/* Funciones init exit */
static int __init iniciarModulo(void);
static void __exit limpiarModulo(void);

//Funciones auxiliares
static void setTimer(void);

void eliminarDispositivos(void);
void crearDispositivos(void);
void crearProcFiles(void);
void eliminarProcFiles(void);

//Lista aleatorios
#define NOMBRE_DEV_TIMER "modtimer"
#define RUTA_FICHERO_TIMER "/dev/modtimer"

//Lista pares
#define NOMBRE_DEV_PARES "modtpares"
#define RUTA_FICHERO_PARES "/dev/modtpares"

//Lista pares
#define NOMBRE_DEV_IMPARES "modtimpares"
#define RUTA_FICHERO_IMPARES "/dev/modtpares"


#define NOMBRE_PROC_TIMER "modtimer"
#define RUTA_FICHERO_PROC_TIMER "/proc/modtimer"

#define NOMBRE_PROC_CONFIG "modconfig"
#define RUTA_FICHERO_PROC_CONFIG "/proc/modconfig"



#endif /* TIMER_H */
