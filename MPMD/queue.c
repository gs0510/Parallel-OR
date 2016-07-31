// C program for array implementation of queue
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "MPMD1.h"

#define MAX_SIZE  50000
// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    struct Node *array;
};
 
struct Node{
    int size;
    //int fn_size;
    ip_packet* data;    
    int fn;
};
// function to create a queue of given capacity. It initializes size of 
// queue as 0
struct Queue* createQueue(unsigned capacity, int elemsize,int num)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0; 
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (struct Node*)malloc(queue->capacity * sizeof(struct Node));
    int i=0;
    for(i=0;i<capacity;i++)
    {
        (queue->array[i]).size=num;
        (queue->array[i]).data =(ip_packet *)malloc(num*elemsize);
    }
    return queue;
}
 
// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{  
    return (queue->size == queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{  
    return (queue->size == 0); 
}
 
// Function to add an item to the queue.  It changes rear and size
void enqueue(struct Queue* queue, struct Node *item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear].fn = item->fn;
    int i;
    for(i=0;i<item->size;i++)
    {
        queue->array[queue->rear].data[i] = item->data[i];
    }
    queue->array[queue->rear].size = item->size;
    queue->size = queue->size + 1;
    //printf("%d enqueued to queue\n", item);
}
 
// Function to remove an item from queue.  It changes front and size
void dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return ;
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
}
 
// Function to get front of queue
struct Node* front(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return &(queue->array[queue->front]);
}