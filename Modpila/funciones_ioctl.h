#ifndef FUNCIONES_IOCTL_H
#define FUNCIONES_IOCTL_H

#include <linux/ioctl.h>

/** http://stackoverflow.com/questions/10633311/dynamic-registeration-with-ioctls */

#define NUM_UNICO 0xF5
#define NOMBRE_DEV "pila"
#define RUTA_FICHERO "/dev/pila"

#define IOCTL_MAX_STACK_SIZE 	_IOR(NUM_UNICO, 9, int)
#define IOCTL_CONSUMIR			_IOR(NUM_UNICO, 10, int)

#endif

