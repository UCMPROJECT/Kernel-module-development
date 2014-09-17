#include "listas.h"


/* Integer list nodes */
struct list_item_t {
    int data;
    struct list_head links;
};

void add_node(int arg, struct list_head * head){
/*
    Adds node with "arg" value to the list.
*/

    struct list_item_t * newNode = (struct  list_item_t *)vmalloc(sizeof(struct list_item_t));
    if (newNode == NULL){
        printk(KERN_INFO "Something went wrong");
    }
    
    newNode->data = arg;
    INIT_LIST_HEAD(&newNode->links);
    list_add(&newNode->links, head); //Adds it to beginning
}


int delete_node(int arg, struct list_head * head){
/*
    Deletes all ocurrences of "arg" within the list.
    Returns number of elements with "arg" value.
*/
    struct list_head *iter;
    struct list_item_t *objPtr;
    struct list_head * q;

    int count = 0;
    printk(KERN_INFO "Deleting all items with value: %d", arg);
    list_for_each_safe(iter, q, head){
        objPtr = list_entry(iter, struct list_item_t, links);
        if (arg == objPtr->data){
            printk(KERN_INFO "Freeing item with value: %d", objPtr->data);
            list_del(iter); //Delete item from list
            vfree(objPtr); //Free memory associated with element
            count++;
        }       
    }     
    return count;
}

struct list_item_t * deleteTopOfStack(struct list_head * head){

    struct list_item_t * newNode = (struct  list_item_t *)vmalloc(sizeof(struct list_item_t));
    if (newNode == NULL){
        printk(KERN_INFO "Something went wrong");
    }

    struct list_item_t *topOfStack;
    topOfStack = list_first_entry(head, struct list_item_t, links); 
    int top = topOfStack->data;
    newNode->data = top;
    list_del(&topOfStack->links); //Delete item from list
    vfree(topOfStack); //Free memory associated with element
    return newNode;
}

void delete_list(struct list_head * head){
/*
Deletes all nodes of the list.
*/
    struct list_head *iter;
    struct list_item_t *objPtr;
    struct list_head * q;

    printk(KERN_INFO "Deleting all items");
    list_for_each_safe(iter, q, head){
        objPtr = list_entry(iter, struct list_item_t, links);
        printk(KERN_INFO "Freeing item with value: %d", objPtr->data);
        list_del(iter); //Delete item from list
        vfree(objPtr); //Free memory associated with element
    }
}

void display(struct list_head *head)
/*
    Prints to /var/log/messages elements of the list .
*/
{
    struct list_head *iter;
    struct list_item_t *objPtr;

    printk(KERN_INFO "--Listing inserted numbers--");

    int contador = 0;
    list_for_each(iter, head) {
        objPtr = list_entry(iter, struct list_item_t, links);
        printk(KERN_INFO "%d ", objPtr->data);            
        contador++;
    }
    printk(KERN_INFO "--End of list--");
}

int getSize(struct list_head *head)
/*
    Prints to /var/log/messages elements of the list .
*/
{
    struct list_head *iter;
    struct list_item_t *objPtr;
    int contador = 0;
    list_for_each(iter, head) {
        objPtr = list_entry(iter, struct list_item_t, links);
        printk(KERN_INFO "%d ", objPtr->data);
        contador++;
    }
    return contador;
}