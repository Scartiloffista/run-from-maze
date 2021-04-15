#include "heap.h"
#include "compare.h"
#include "collection.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int father(int i)
{
    return floor((i - 1) / 2);
}

int left(int i)
{
    return (2 * i) + 1;
}

int right(int i)
{
    return (2 * i) + 2;
}

void heap_tree_heapify(heap_node_t *root, heap_t *heap)
{
    /* implementazione di heapify
     * su un heap realizzato come albero binario
     */
    COMPAREFN compara = heap->compara;
    if (root == NULL)
        return;
    int **p = heap->index_pointers;
    void **infos = heap->infos;
    heap_node_t *max = root;

    if (root->right != NULL)
    {
        if (compara(&(root->right->key), &(root->key)) == 1)
            max = root->right;
    }

    if (root->left != NULL)
    {
        if (compara(&(root->left->key), &(max->key)) == 1)
            max = root->left;
    }

    if (max != root)
    {
        void *temp = infos[max->index];
        infos[max->index] = infos[root->index];
        infos[root->index] = temp;

        int inttemp = root->key;
        root->key = max->key;
        max->key = inttemp;

        int *pint = p[max->index];
        p[max->index] = p[root->index];
        p[root->index] = pint;

        *(p[max->index]) = max->index;
        *(p[root->index]) = root->index;

        heap_tree_heapify(max, heap);
    }
}

heap_node_t *heap_tree_build_ric(int *array, heap_t *heap, int index, int size)
{
    /* costruire un heap come albero binario
     * alloca tutto quello che c'è da fare
     * per il nodo corrente. poi va ricorsivamente
     * sui figli
     */
    if (index >= size)
        return NULL; // finito l'heap
    else
    {
        heap_node_t *root = calloc(1,sizeof(heap_node_t));
        if (root == NULL)
        {
            perror("memoria finita\n"), exit(EXIT_FAILURE);
        }
        root->key = array[index];
        root->index = index;
        if (index == 0) root->parent = NULL;
        root->left = heap_tree_build_ric(array, heap, (2 * index) + 1, size);
        root->right = heap_tree_build_ric(array, heap, (2 * index) + 2, size);
        if (root->right != NULL)
            root->right->parent = root;
        if (root->left != NULL)
            root->left->parent = root;
        heap_tree_heapify(root, heap);
        return root;
    }
}



heap_node_t *find_nth_node(heap_node_t *root, int n)
{
    /* Trova la più grande potenza di 2
     * non più grande dell'elemento che vogliamo
     */
    int bit_index = 0;
    while (1)
    {
        if (1 << (bit_index + 1) > n)
            break;
        bit_index++;
    }

    /* abbiamo trovato la "riga" su cui fermarci. ora
     * agiamo sulla rappresentazione binaria di n
     * per trovare il nodo che ci serve
     */
    bit_index--;

    for (; bit_index >= 0; bit_index--)
    {
        int mask = (1 << bit_index);
        if (n & mask)
            root = root->right;
        else
            root = root->left;
    }
    return root;
}

heap_element_t *heap_tree_extract_top(void *h)
{
    heap_t *heap = h;
    if (heap->size < 1)
    {
        return NULL;
    }
    heap_node_t *new_head = find_nth_node(heap->root, heap->size);
    heap_node_t *parent = new_head->parent;

    // preparo elemento da ritornare
    heap_element_t *to_return = malloc(sizeof(heap_element_t));
    if (to_return == NULL)
    {
        perror("memoria finita\n"), exit(EXIT_FAILURE);
    }
    to_return->key = heap->root->key;
    to_return->info = heap->infos[0];
    if (heap->size == 1)
    {
        free(heap->root);
        heap->root = NULL;
        free(heap->index_pointers[0]);
        heap->size--;
        return to_return;
    }
    // il parente del nuovo deve avere il figlio annullato
    if (new_head == parent->left)
        parent->left = NULL;
    else
        parent->right = NULL;

    // la nuova root deve avere la nuova chiave
    // e il nuovo valore di info
    heap->root->key = new_head->key;
    heap->infos[0] = heap->infos[new_head->index];
    heap->infos[new_head->index] = NULL;
    free(heap->index_pointers[0]);
    heap->index_pointers[0] = heap->index_pointers[new_head->index];
    *(heap->index_pointers[0]) = 0;
    // quel nodo non esiste più, regoliamoci di conseguenza
    free(new_head);
    heap->size--;
    heap_tree_heapify(heap->root, heap);
    return to_return;
}

void heap_tree_modify_key(heap_t *heap, heap_node_t *node, int i, int new_val,
                          int flag)
{
    /* in sintesi, questa funzione a seconda di che
     * heap stiamo utilizzando, aumenterà o diminuirà
     * la chiave desiderata
     */

    // trova l'i-esimo elemento e ci dà il nuovo valore
    node->key = new_val;
    int **p = heap->index_pointers;

    /* se è max heap, ok, c'è bisogno di un heapify
     * sennò, bisogna scambiare col padre*/
    if (flag)
        heap_tree_heapify(node, heap);
    else
    {
        heap_node_t *to_it = node;
        int itemp;
        void *vtemp;
        while (to_it->parent != NULL && heap->compara(&(to_it->parent->key), &(to_it->key)) == -1)
        {
            itemp = to_it->key;
            to_it->key = to_it->parent->key;
            to_it->parent->key = itemp;

            vtemp = heap->infos[to_it->index];
            heap->infos[to_it->index] = heap->infos[to_it->parent->index];
            heap->infos[to_it->parent->index] = vtemp;

            int *pint = p[to_it->index];
            p[to_it->index] = p[to_it->parent->index];
            p[to_it->parent->index] = pint;

            *(p[to_it->index]) = to_it->index;
            *(p[to_it->parent->index]) = to_it->parent->index;

            to_it = to_it->parent;
        }
    }
}

void heap_tree_decrease_key(void *h, int i, int new_val)
{
    heap_t *heap = (heap_t *) h;
    if(i >= heap->size) return;
    heap_node_t *node = find_nth_node(heap->root, i + 1);
    if (node->key < new_val)
    {
        printf("errore in %d: %d < %d\n", i, node->key, new_val);
        return;
    }
    if (heap->is_max)
    {
        heap_tree_modify_key(heap, node, i, new_val, 1);
    }
    else
    {
        heap_tree_modify_key(heap, node, i, new_val, 0);
    }
}

void heap_tree_increase_key(void *h, int i, int new_val)
{
    heap_t *heap = (heap_t *) h;
    if(i > heap->size) return;
    heap_node_t *node = find_nth_node(heap->root, i + 1);
    if (node == NULL) return;
    if (node->key > new_val)
    {
        printf("errore in %d: %d > %d\n", i, node->key, new_val);
        return;
    }
    if (heap->is_max)
    {
        heap_tree_modify_key(heap, node, i, new_val, 0);
    }
    else
    {
        heap_tree_modify_key(heap, node, i, new_val, 1);
    }
}

void heap_tree_bfs(void *h)
{
    heap_t *heap = h;
    queue_t *queue = queue_init();
    queue_push(queue, heap->root);
    heap_node_t *actual;
    void *temp;
    do
    {
        /* inizio una bfs */
        actual = (heap_node_t *) queue_top(queue);
        queue_pop(queue);
        temp = heap->infos[actual->index];
        //printf("%d - %p - ", actual->key, temp);
        //int_print(&actual->index);
        printf("%d ", actual->index);
        //
        if (actual->left != NULL)
            queue_push(queue, actual->left);
        if (actual->right != NULL)
            queue_push(queue, actual->right);
    }
    while (queue->size > 0);
    queue_delete(queue);
    free(queue);
    printf("\n");
}

void heap_tree_delete_key(void *h, int i)
{
    heap_t *heap = (heap_t *) h;
    if (heap->is_max)
        heap_tree_increase_key(heap, i, heap->root->key + 1);
    else
        heap_tree_decrease_key(heap, i, heap->root->key - 1);

    heap_tree_extract_top(heap);
}

void heap_array_heapify(heap_t *heap, int i)
{
    /* Compara è una funzione di comodo, per gestire meglio
     * i tre possibili casi di un confronto.
     * heapsize è la sizeensione dell'array, quindi è uguale
     * all'indice massimo + 1 */
    COMPAREFN compara = heap->compara;
    int *array = heap->array;
    int **p = heap->index_pointers;
    int heapsize = heap->size, l = left(i), chosen_one, r = right(i);

    /* Questo pezzo sceglie il massimo
     * o minimo tra radice e due figli */

    if ((l < heapsize) && (compara(&array[l], &array[i]) == 1))
    {
        chosen_one = l;
    }
    else
    {
        chosen_one = i;
    }

    if ((r < heapsize) && (compara(&array[r], &array[chosen_one]) == 1))
    {
        chosen_one = r;
    }

    if (chosen_one != i)
    {
        /* Se il massimo è diverso dalla radice
         * allora bisogna scambiare i due nodi
         * e poi chiamare heapify sul sottoalbero
         * modificato */

        int temp;
        temp = array[i];
        array[i] = array[chosen_one];
        array[chosen_one] = temp;

        void *vtemp;
        vtemp = heap->infos[i];
        heap->infos[i] = heap->infos[chosen_one];
        heap->infos[chosen_one] = vtemp;

        int *pint = p[i];
        p[i] = p[chosen_one];
        p[chosen_one] = pint;

        *(p[i]) = i;
        *(p[chosen_one]) = chosen_one;

        heap_array_heapify(heap, chosen_one);
    }

}

void heap_array_build(heap_t *heap)
{
    /* Crea un heap a partire da un array */
    int i;
    for (i = floor((double) (heap->size - 1) / 2); i >= 0; i--)
    {
        heap_array_heapify(heap, i);
    }
}

heap_element_t *heap_array_extract_top(void *h)
{
    heap_t *heap = (heap_t *) h;
    if (heap->size < 1)
    {
        perror("heap underflow\n");
        return NULL;
    }
    else
    {
        heap_element_t *max = malloc(sizeof(heap_element_t));
        if (max == NULL)
        {
            perror("memoria finita\n"), exit(EXIT_FAILURE);
        }
        max->key = heap->array[0];
        max->info = heap->infos[0];
        heap->size--;
        if (heap->size > 1)
        {
            heap->array[0] = heap->array[(heap->size)];
            free(heap->index_pointers[0]);
            heap->index_pointers[0] = heap->index_pointers[heap->size];
            *(heap->index_pointers[0]) = 0;
            heap_array_heapify(heap, 0);
        }
        return max;
    }
}

void heap_array_modify_key(heap_t *heap, int i, int new_val, int flag)
{

    /* se è max heap, ok, c'è bisogno di un heapify
     * sennò, bisogna scambiare col padre*/
    heap->array[i] = new_val;
    if (flag == 1)
        heap_array_heapify(heap, i);
    else
    {
        int itemp;
        void *vtemp;
        while (i > 0
                && heap->compara(&(heap->array[father(i)]), &(heap->array[i]))
                == -1)
        {
            itemp = heap->array[i];
            heap->array[i] = heap->array[father(i)];
            heap->array[father(i)] = itemp;

            vtemp = heap->infos[i];
            heap->infos[i] = heap->infos[father(i)];
            heap->infos[father(i)] = vtemp;

            int *pint = heap->index_pointers[i];
            heap->index_pointers[i] = heap->index_pointers[father(i)];
            heap->index_pointers[father(i)] = pint;

            *(heap->index_pointers[father(i)]) = father(i);
            *(heap->index_pointers[i]) = i;

            i = father(i);
        }
    }
}

void heap_array_decrease_key(void *h, int i, int new_val)
{
    heap_t *heap = (heap_t *) h;
    if (heap->array[i] < new_val)
    {
        printf("(max=%d, albero=%d) errore in %d: %d > %d\n", heap->is_max, heap->is_tree, i, heap->array[i], new_val);
        return;
    }
    if (heap->is_max)
    {
        heap_array_modify_key(heap, i, new_val, 1);
    }
    else
    {
        heap_array_modify_key(heap, i, new_val, 0);
    }

}

void heap_array_increase_key(void *h, int i, int new_val)
{
    heap_t *heap = (heap_t *) h;
    if (heap->array[i] > new_val)
    {
        printf("(max=%d, albero=%d) errore in %d: %d < %d\n", heap->is_max, heap->is_tree, i, heap->array[i], new_val);
        return;
    }
    if (heap->is_max)
    {
        heap_array_modify_key(heap, i, new_val, 0);
    }
    else
    {
        heap_array_modify_key(heap, i, new_val, 1);
    }
}

void heap_array_delete_key(void *h, int i)
{
    heap_t *heap = (heap_t *) h;
    if (heap->size < 1) return;
    if (heap->is_max)
        heap_array_increase_key(heap, i, heap->array[0] + 1);
    else
        heap_array_decrease_key(heap, i, heap->array[0] - 1);

    heap_array_extract_top(heap);
}

int* heap_tree_insert_key(void *h, int new_key, void *info)
{
    heap_t *heap = (heap_t *) h;
    int i = heap->size, f = father(i), *pint;
    heap_node_t *parent = find_nth_node(heap->root, f + 1);

    heap_node_t *node = calloc(1,sizeof(heap_node_t));
    if (node == NULL)
    {
        perror("memoria finita\n"), exit(EXIT_FAILURE);
    }
    if (heap->size == 0) heap->root = node;
    node->index = heap->size;
    node->left = node->right = NULL;
    if (heap->size == heap->max_size)
    {
        heap->infos = realloc(heap->infos,
                              (heap->max_size + 10) * sizeof(void *));
        heap->index_pointers = realloc(heap->index_pointers,
                                       (heap->max_size + 10) * sizeof(int *));
        heap->max_size += 10;

        if (heap->infos == NULL || heap->index_pointers == NULL)
        {
            perror("memoria finita"), exit(EXIT_FAILURE);
        }
    }
    heap->infos[heap->size] = info;
    heap->index_pointers[heap->size] = pint = malloc(sizeof(int));
    if (heap->index_pointers[heap->size] == NULL) perror("memory allocation"), exit(EXIT_FAILURE);
    *(heap->index_pointers[heap->size]) = heap->size;

    // e tu a chi si figlio?
    if (parent != NULL)
    {
        if (i == left(f))
        {
            parent->left = node;
        }
        else
        {
            parent->right = node;
        }
    }
    node->parent = parent;

    heap->size++;

    if (heap->is_max)
    {
        node->key = INT_MIN;
        heap_tree_increase_key(heap, heap->size - 1, new_key);
    }
    else
    {
        node->key = INT_MAX;
        heap_tree_decrease_key(heap, heap->size - 1, new_key);
    }
    return heap->index_pointers[node->index];
}

int* heap_array_insert_key(void *h, int new_key, void *info)
{
    heap_t *heap = (heap_t *) h;
    int* pint;

    // realloc
    if (heap->size == heap->max_size)
    {
        heap->array = realloc(heap->array, (heap->max_size + 10) * sizeof(int));
        heap->infos = realloc(heap->infos,
                              (heap->max_size + 10) * sizeof(void *));
        heap->index_pointers = realloc(heap->index_pointers,
                                       (heap->max_size + 10) * sizeof(int *));
        heap->max_size += 10;
        if (heap->array == NULL || heap->array == NULL || heap->index_pointers == NULL)
        {
            perror("memoria finita"), exit(EXIT_FAILURE);
        }
    }
    heap->infos[heap->size] = info;
    heap->index_pointers[heap->size] = pint =  malloc(sizeof(int));
    if (heap->index_pointers[heap->size] == NULL) perror("memory allocation"), exit(EXIT_FAILURE);
    *(heap->index_pointers[heap->size]) = heap->size;

    if (heap->is_max)
    {
        heap->array[heap->size++] = INT_MIN;
        heap_array_increase_key(heap, heap->size - 1, new_key);
    }
    else
    {
        heap->array[heap->size++] = INT_MAX;
        heap_array_decrease_key(heap, heap->size - 1, new_key);
    }
    return pint;
}

int check_tree_ric(heap_node_t *root, COMPAREFN compara, int is_max)
{
    if (root == NULL)
    {
        printf("ERRORE: LA ROOT E' VUOTA\n");
        return 0;
    }
    int uno = 1, due = 1;
    if (root->left != NULL)
    {
        if (compara(&(root->key), &(root->left->key)) == -1)
        {
            printf("ERRORE INCREDIBILE NELL'HEAP COME ALBERO NON DOVREBBE SUCCEDERE\n");
            printf("(max=%d) root = %d, figlio = %d, root->key = %d, figlio->key = %d\n", is_max, root->index, root->left->index, root->key, root->left->key);
            uno = 0;
            exit(EXIT_FAILURE);

        }
        else
        {
            uno = check_tree_ric(root->left, compara, is_max);
        }
    }

    if (root->right != NULL)
    {
        if (compara(&(root->key), &(root->right->key)) == -1)
        {
            printf("ERRORE INCREDIBILE NELL'HEAP COME ALBERO NON DOVREBBE SUCCEDERE\n");
            printf("(max=%d) root = %d, figlio = %d, root->key = %d, figlio->key = %d\n", is_max, root->index, root->right->index, root->key, root->right->key);
            due = 0;
            exit(EXIT_FAILURE);
        }
        else
        {
            due = check_tree_ric(root->right, compara, is_max);
        }
    }

    return uno && due;
}

int check_tree(void *h)
{
    heap_t *heap = h;
    if(heap->size < 1) return 1;
    return check_tree_ric(heap->root, heap->compara, heap->is_max);
}

int check_array(void *heap)
{
    heap_t *h = heap;
    for (int i = 0; i < floor((double) (h->size - 1) / 2); ++i)
    {
        if (left(i) < h->size)
        {
            if (h->compara(&(h->array[i]), &(h->array[left(i)])) == -1)
            {
                printf("ERRORE INCREDIBILE NELL'HEAP COME ARRAY NON DOVREBBE SUCCEDERE\n");
                printf("(max=%d) i = %d, figlio = %d, root->key = %d, figlio->key = %d\n", h->is_max, i, left(i), h->array[i], h->array[left(i)]);
                exit(EXIT_FAILURE);

                return 0;
            }
        }
        if (right(i) < h->size)
        {
            if (h->compara(&(h->array[i]), &(h->array[right(i)])) == -1)
            {
                printf("ERRORE INCREDIBILE NELL'HEAP COME ARRAY NON DOVREBBE SUCCEDERE\n");
                printf("(max=%d) i = %d, figlio = %d, root->key = %d, figlio->key = %d\n", h->is_max, i, right(i), h->array[i], h->array[right(i)]);
                exit(EXIT_FAILURE);

                return 0;
            }

        }
    }
    return 1;
}

void print_array(void *heap)
{
    heap_t *h = heap;
    for (int i = 0; i < h->size; ++i)
    {
        printf("%d ", h->array[i]);
    }
}

heap_t *heap_create(int is_tree, int is_max, int size, int *array, void **infos)
{
    //printf("Sto creando un heap di tipo albero=%d, max=%d, di grandezza=%d\n",
        //   is_tree, is_max, size);
    // creo heap e setto se min o max
    heap_t *heap = malloc(sizeof(heap_t));
    heap->is_max = is_max;
    if (is_max)
    {
        heap->compara = int_cmp;
    }
    else
    {
        heap->compara = int_cmp_rev;
    }

    heap->max_size = size;
    heap->size = size;
    heap->infos = infos;
    heap->is_tree = is_tree;
    heap->index_pointers = calloc(size, sizeof(int *));
    if (heap->index_pointers == NULL) perror("memory allocation"), exit(EXIT_FAILURE);
    for (int i = 0; i < size; ++i)
    {
        heap->index_pointers[i] = malloc(sizeof(int));
        if (heap->index_pointers[i] == NULL) perror("memory allocation"), exit(EXIT_FAILURE);
        *(heap->index_pointers[i]) = i;
    }

    if (is_tree)
    {
        heap->root = heap_tree_build_ric(array, heap, 0, size);
        heap->decrease_key = heap_tree_decrease_key;
        heap->increase_key = heap_tree_increase_key;
        heap->insert_key = heap_tree_insert_key;
        heap->delete_key = heap_tree_delete_key;
        heap->extract_top = heap_tree_extract_top;
        heap->check_heap = check_tree;
        heap->print_heap = heap_tree_bfs;
        heap->array = NULL;
    }
    else
    {
        int *array2 = malloc(sizeof(int) * size);
        for (int j = 0; j < size; j++)
        {
            array2[j] = array[j];
        }
        heap->array = array2;
        heap_array_build(heap);
        heap->decrease_key = heap_array_decrease_key;
        heap->increase_key = heap_array_increase_key;
        heap->insert_key = heap_array_insert_key;
        heap->delete_key = heap_array_delete_key;
        heap->extract_top = heap_array_extract_top;
        heap->check_heap = check_array;
        heap->print_heap = print_array;
        heap->root = NULL;
    }
    return heap;
}

void delete_tree(heap_node_t *n)
{
    if (n == NULL) return;
    delete_tree(n->left);
    delete_tree(n->right);
    free(n);
    return;
}

void delete_heap(heap_t *h)
{
    if (h->array != NULL)
        free(h->array);
    if (h->infos != NULL)
        free(h->infos);
    delete_tree(h->root);
    free(h->index_pointers);
    free(h);
}
