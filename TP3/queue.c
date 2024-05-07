#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "queue.h"

// Function to create a queue of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (char*) malloc(queue->capacity * sizeof(char)); // Changed to char*
    return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue* queue)
{ return (queue->size == queue->capacity); }

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{ return (queue->size == 0); }

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, char item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size++;
}

// Function to remove an item from the queue.
// It changes front and size
char dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return CHAR_MIN;
    char item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return item;
}

// Function to get the front of the queue
char front(struct Queue* queue)
{
    if (isEmpty(queue))
        return CHAR_MIN;
    return queue->array[queue->front];
}

// Function to get the rear of the queue
char rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return CHAR_MIN;
    return queue->array[queue->rear];
}
