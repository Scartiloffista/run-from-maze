#include "compare.h"

typedef struct heap_node_t
{
    /* index è il corrispettivo nell'array dei valori
     * key è la chiave */
    int index;
    struct heap_node_t* left;
    struct heap_node_t* right;
    struct heap_node_t* parent;
    int key;
} heap_node_t;

typedef struct heap_element_t
{
    int key;
    void* info;
} heap_element_t;

typedef void(*DELETEKEYFN)(void*,int);
typedef void(*CHANGEKEYFN)(void*,int,int);
typedef int*(*INSERTKEYFN)(void*,int,void*);
typedef heap_element_t*(*EXTRACTKEYFN)(void*);
typedef int(*CHECKHEAPFN)(void*);
typedef void(*PRINTHEAPFN)(void*);


typedef struct heap_t
{
    heap_node_t* root;
    int size;
    void** infos;
    int** index_pointers;
    int max_size;
    COMPAREFN compara;
    int is_max;
    int is_tree;
    int* array;
    DELETEKEYFN delete_key;
    CHANGEKEYFN increase_key;
    CHANGEKEYFN decrease_key;
    INSERTKEYFN insert_key;
    EXTRACTKEYFN extract_top;
    CHECKHEAPFN check_heap;
    PRINTHEAPFN print_heap;
} heap_t;

heap_t* heap_create(int is_tree, int is_max, int size, int* array, void** infos);
void delete_heap(heap_t* h);
