obj-m += mod2.o
mod2-objs :=  workqueues.o listas.o Queues.o modtimer.o
#mod2-objs :=  Queues.o modtimer.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules  
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
