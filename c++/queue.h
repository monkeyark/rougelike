#ifndef QUEUE_H
# define QUEUE_H

#include <stdbool.h>
#include <stdint.h>


typedef struct queue_node Node;

struct queue_node
{
	Node *next;
	Node *prev;
	int priority;
};

typedef struct Priority_Queue
{
	Node **head;
	uint32_t size;
	int32_t (*compare)(const void *key, const void *with);
	void (*datum_delete)(void *);
} Queue;


Node *node_new(int priority);
void pq_insert(Queue pq, Node **head, int priority, int *dist);
int pq_pop(Queue pq, Node **head);
bool pq_isEmpty(Queue pq, Node **head);
void pq_delete();


#endif
