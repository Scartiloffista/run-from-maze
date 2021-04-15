#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "collection.h"


char *get_delim(FILE *fd, char d)
{
    int i = 0, dim = 10;
    char *str = malloc(sizeof(char) * dim);
    do
    {
        str[i] = getc(fd);
        i++;
        if (i == dim)
        {
            dim += 10;
            str = realloc(str, dim);
            if (str == NULL)
            {
                perror("memory error"), exit(EXIT_FAILURE);
            }
        }
    }
    while (str[i - 1] != d && !feof(fd));
    str[i - 1] = 0;
    return str;
}

char *get_line(FILE *fd)
{
    char *str;
    int dim;
    str = get_delim(fd, '\n');
    dim = strlen(str);
    dim--;
    while (str[dim] == '\n' || str[dim] == '\r')
    {
        str[dim] = 0;
        dim--;
    }
    dim++;
    str = realloc(str, sizeof(char) * (dim + 1));
    if (str == NULL)
        perror("memory error"), exit(EXIT_FAILURE);
    return str;
}

void list_insert_tail(list_t* list, void* elem)
{
    /* se ho un solo elemento,
     * devo avere testa e coda uguali. */

	node_t* node = malloc(sizeof(node_t));
    if (node == NULL)
	{
        perror("memoria finita\n"), exit(EXIT_FAILURE);
    }
	node->info = elem;
	node->next = NULL;
	node->prev = list->tail;

	if(list->size > 0)
	{
		list->tail->next = node;
		list->tail = node;
	}
	else
	{
		list->tail = list->head = node;
	}
	list->size++;
}


void list_insert_head(list_t* list, void* elem)
{
    /* se ho un solo elemento,
     * devo avere testa e coda uguali. */

	node_t* node = malloc(sizeof(node_t));
    if (node == NULL)
	{
        perror("memoria finita\n"), exit(EXIT_FAILURE);
    }
	node->info = elem;
	node->next = list->head;
	node->prev = NULL;
	list->head = node;

    list->size++;
    if(list->size == 1) list->tail = node;
}


list_t* list_init()
{
	list_t* list = malloc(sizeof(list_t));
    if (list == NULL)
	{
        perror("memoria finita\n"), exit(EXIT_FAILURE);
    }
	list->head = list->tail = NULL;
	list->size = 0;
	return list;
}

int list_is_empty(list_t* l){
    return l->size == 0;
}

void* list_head(list_t* list)
{
    if (list->size > 0)
        return list->head->info;
    return (void*)-1;
}

void list_delete_head(list_t* list)
{
    /* se c'era un solo elemento,
     * testa e coda a null */
	if(list->size == 0) return;
	node_t* temp = list->head->next;
	free(list->head);
	list->head = temp;
	if(list->head == NULL) list->tail = NULL;
	list->size--;
}

void list_delete_tail(list_t* list)
{
    /* se c'era un solo elemento,
     * testa e coda a null */
	if(list->size == 0) return;
	node_t* temp = list->tail->prev;
	free(list->tail);
	list->tail = temp;
	if(list->tail == NULL) list->head = NULL;
	list->size--;
}


void list_print(list_t* list)
{
	node_t* node = list->head;
	while(node != NULL)
	{
		printf("%p ", (node->info));
		node = node->next;
	}
}

void list_delete_elem(list_t* list, void* elem){
    node_t* node = list->head;
    if(list->head == NULL) return;
    while(node != NULL && node->info != elem){
        node = node->next;
    }
    if(node != NULL){
        node_t* temp = node->next;
        node_t* temp2 = node->prev;
        if(node == list->head) list->head = temp;
        free(node);
        if(temp2 != NULL) temp2->next = temp;
    }
}

list_t* list_delete(list_t* list, int dealloc)
{
	if(list == NULL) return list;
	node_t* node = list->head, *tmp;
	while(node)
	{
		tmp = node->next;
		free(node);
		node = tmp;
	}
	free(list);
	return NULL;
}




void queue_push(queue_t* queue, void* elem)
{
    /* se ho un solo elemento,
     * devo avere testa e coda uguali. */

	node_t* node = malloc(sizeof(node_t));
    if (node == NULL)
	{
        perror("memoria finita\n"), exit(EXIT_FAILURE);
    }
	node->info = elem;
	node->next = NULL;
	node->prev = queue->tail;

	if(queue->size > 0)
	{
		queue->tail->next = node;
		queue->tail = node;
	}
	else
	{
		queue->tail = queue->head = node;
	}
	queue->size++;
}

queue_t* queue_init()
{
	queue_t* queue = malloc(sizeof(queue_t));
    if (queue == NULL)
	{
        perror("memoria finita\n"), exit(EXIT_FAILURE);
    }
	queue->head = queue->tail = NULL;
	queue->size = 0;
	return queue;
}

void* queue_top(queue_t* queue)
{
	if(queue->size > 0)
		return queue->head->info;
	return NULL;
}

void queue_pop(queue_t* queue)
{
    /* se c'era un solo elemento,
     * testa e coda a null */
	if(queue->size == 0) return;
	node_t* temp = queue->head->next;
	free(queue->head);
	queue->head = temp;
	if(queue->head == NULL) queue->tail = NULL;
	queue->size--;
}

int queue_is_empty(queue_t* q){
	return 0 == q->size;
}

void queue_delete(queue_t* queue)
{
	while(queue->size > 0)
		queue_pop(queue);
}
