#include "Queues.h"


/* Very simple queue
 * These are FIFO queues which discard the new data when full.
 *
 * Queue is empty when in == out.
 * If in != out, then 
 *  - items are placed into in before incrementing in
 *  - items are removed from out before incrementing out
 * Queue is full when in == (out-1 + QUEUE_SIZE) % QUEUE_SIZE;
 *
 * The queue will hold QUEUE_ELEMENTS number of items before the
 * calls to QueuePut fail.
 */

/* Queue structure */

MODULE_LICENSE("GPL");
 


void QueueInit(buffer_circular_t * buf){
   buf->elements = QUEUE_SIZE;
   buf->size = QUEUE_SIZE + 1;
   buf->head = buf->tail = 0;
   buf->size_queue = 0;
}

int QueuePut(buffer_circular_t * buf, int new)
{
    if(buf->head == (( buf->tail - 1 + QUEUE_SIZE) % QUEUE_SIZE))
    {
        return -1; /* Queue Full*/
    }

    buf->Queue[buf->head] = new;

    buf->head = (buf->head + 1) % QUEUE_SIZE;

    buf->size_queue = buf->size_queue + 1;

    return 0; // No errors
}

int isQueueEmpty(buffer_circular_t * buf){
    if (buf->head == buf->tail)
        return 1;
    return 0;
}

int QueueSize(buffer_circular_t * buf){
    return buf->size_queue;
}

int QueueGet(buffer_circular_t * buf, int *old)
{
    if(buf->head == buf->tail)
    {
        return -1; /* Queue Empty - nothing to get*/
    }

    buf->size_queue = buf->size_queue -1;

    *old = buf->Queue[buf->tail];

    buf->tail = (buf->tail + 1) % QUEUE_SIZE;

    return 0; // No errors
}