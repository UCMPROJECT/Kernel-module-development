#ifndef FUNCIONES_IOCTL_H
#define FUNCIONES_IOCTL_H

#include <linux/ioctl.h>

/** http://stackoverflow.com/questions/10633311/dynamic-registeration-with-ioctls */

#define NUM_UNICO 0xF5
#define NOMBRE_DEV "modtimer"
#define RUTA_FICHERO "/dev/modtimer"

#define NOMBRE_DEV_CONFIG "modconfig"
#define RUTA_FICHERO_CONFIG "/proc/modconfig"


#define IOCTL_ACTIVAR			_IOR(NUM_UNICO, 12, int)
#define IOCTL_DESACTIVAR		_IOR(NUM_UNICO, 13, int)
#define IOCTL_CONSULTAR_VALOR	_IOR(NUM_UNICO, 14, int)
#define IOCTL_MODIFICAR_VALOR	_IOR(NUM_UNICO, 15, int)
#define IOCTL_MODIFICAR_TIMER_PERIOD _IOR(NUM_UNICO, 16, int)
#define IOCTL_MODIFICAR_THRESHOLD	_IOR(NUM_UNICO, 17, int)

#endif

