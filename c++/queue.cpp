#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"


Node *node_new(int priority)
{
	Node *temp = (Node *) malloc(sizeof(Node));
	temp->priority = priority;
	temp->next = NULL;

	return temp;
}

void pq_insert(Queue pq, Node **head, int priority, int *dist)
{
	Node *temp = *head;
	Node *min = node_new(priority);
    
    if(*head == NULL)
    {
        min->next = (*head);
        (*head) = min;
    }
    else
    {
        if (dist[(*head)->priority] >= dist[min->priority])
        {
            min->next = (*head);
            (*head) = min;
        }
        else
        {
            while (temp->next != NULL && dist[temp->next->priority] < dist[min->priority])
            {
                temp = temp->next;
            }

            min->next = temp->next;
            temp->next = min;
        }
    }
}

int pq_pop(Queue pq, Node **head)
{
	int n = (*head)->priority;
	Node *t = *head;
	(*head) = (*head)->next;
	free(t);
	return n;
}

bool pq_isEmpty(Queue pq, Node **head)
{
	return (*head) == NULL;
}

void pq_delete(Queue pq)
{
	while (!pq_isEmpty(pq, pq.head))
	{
		pq_pop(pq, pq.head);
	}

	pq.head = NULL;
}

