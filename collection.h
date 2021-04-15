#ifndef COLLECTION_H_
#define COLLECTION_H_

#include <stdio.h>
 
typedef struct node_t
{
	struct node_t* prev;
	struct node_t* next;
	void* info;
} node_t;

typedef struct queue_t
{
	node_t* head;
	node_t* tail;
	int size;
} queue_t;

typedef struct list_t
{
	node_t* head;
	node_t* tail;
	int size;
} list_t;

void queue_push(queue_t* queue, void* elem);
void* queue_top(queue_t* queue);
int queue_is_empty(queue_t* queue);
char *get_line(FILE *fd);
char *get_delim(FILE *fd, char d);
queue_t* queue_init();
void queue_pop(queue_t* queue);
void queue_delete(queue_t* queue);
int list_is_empty(list_t* l);
void list_insert_tail(list_t* list, void* elem);
void list_insert_head(list_t* list, void* elem);
list_t* list_init();
void list_print(list_t* list);
void* list_head(list_t* list);
void list_delete_head(list_t* list);
void list_delete_tail(list_t* list);
list_t* list_delete(list_t* list, int dealloc);
void list_delete_elem(list_t* list, void* elem);

#endif /* COLLECTION_H_ */
