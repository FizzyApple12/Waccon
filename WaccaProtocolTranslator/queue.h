#ifndef _QUEUE_
#define _QUEUE_

#include <stdlib.h>
#include <stdbool.h>

typedef struct node {
    int val;
    struct node *next;
} node_t;

void enqueue(node_t **head, int val);

int dequeue(node_t **head);

bool isEmpty(node_t **head);

#endif