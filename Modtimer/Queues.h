#ifndef QUEUES_H
#define QUEUES_H

#include <linux/kernel.h>           /* kernel */
#include <linux/module.h>           /* module */


#define QUEUE_SIZE 10

typedef struct {
   int Queue[QUEUE_SIZE];
    int head;
    int tail;
    int size;
    int elements;
    int size_queue;
}buffer_circular_t;

//void QueueInit(struct buffer_circular_t * buf);
void QueueInit( buffer_circular_t * buf);
int QueuePut( buffer_circular_t * buf, int newValue);
int QueueGet(buffer_circular_t * buf, int *old);
int QueueSize(buffer_circular_t * buf);
int QueueEmpty(buffer_circular_t * buf);
int isQueueEmpty(buffer_circular_t * buf);

#endif