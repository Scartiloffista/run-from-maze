#include <unistd.h>
#include <time.h>
//  m#include <glib-2.0/glib.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include "maze.h"
#include <limits.h>
#include "heap.h"
#include "collection.h"

maze_t *config_maze_from_file(FILE *fd)
{
    maze_t *g = malloc(sizeof(maze_t));
    if (g == NULL)
    {
        perror("memory error"), exit(EXIT_FAILURE);
    }
    list_t *l = list_init();
    char *str;
    int dim, old_dim;
    str = get_line(fd);
    old_dim = strlen(str);
    list_insert_tail(l, str);
    while (!feof(fd))
    {
        str = get_line(fd);
        if (strcmp(str, "") == 0)
            break;
        list_insert_tail(l, str);
        dim = strlen(str);
        if (dim != old_dim)
            perror("labirinto non rettangolare"), exit(EXIT_FAILURE);
    }

    /* Adesso ho una lista con tutte le righe del labirinto
     * devo solo creare una matrice */


    char **m = malloc(sizeof(char *) * (l->size));
    if (m == NULL)
        perror("memory error"), exit(EXIT_FAILURE);

    int i = 0, size = l->size;

    while (!list_is_empty(l))
    {
        m[i] = list_head(l);
        list_delete_head(l);
        i++;
    }
    g->map = m;
    g->height = size;
    g->width = dim;
    free(l);
    return g;
}

int char_is_in_string(char c, char *string)
{
    int i = 0;
    while (string[i] != 0)
    {
        if (string[i++] == c)
            return 1;
    }
    return 0;
}

coord_t *check_for_bfs(maze_t *m, char **colori, coord_t *** prev, int **dist,
                       queue_t *q, coord_t *o, int x, int y, char t)
{

    /* questa funziona controlla se questa casella è papabile per
     * essere considerata nella bfs (è una casella vuota, non è stata
     * già visitata, etc). dopodiché ritornerà una coordinata
     * se è il nodo che stiamo cercando; in ogni caso aggiornerà
     * colore, distanza, nodo genitore, lo aggiungerà alla coda dei nodi
     * visitare */

    int width = m->width, height = m->height;
    coord_t *c = NULL, *finish = NULL;
    char *to_ignore = " ";
    if (x == -1)
        x = width - 1;
    else if (x == width)
        x = 0;
    if (y == -1)
        y = height - 1;
    else if (y == height)
        y = 0;

    if (t == 'S')
        to_ignore = " E";
    else if (t == 'F')
        to_ignore = " SE";

    if (m->map[y][x] == t || char_is_in_string(m->map[y][x], to_ignore) == 1)
    {
        if (colori[y][x] == 'b')
        {
            c = malloc(sizeof(coord_t));
            if (c == NULL)
                perror("memory error"), exit(EXIT_FAILURE);
            colori[y][x] = 'g';
            c->x = x;
            c->y = y;
            queue_push(q, c);
            prev[c->y][c->x] = o;
            dist[c->y][c->x] = dist[o->y][o->x] + 1;
            if (m->map[c->y][c->x] == t)
            {
                finish = c;
            }
        }
    }
    return finish;
}

void print_map(char **m, int dim)
{
    for (int i = 0; i < dim; ++i)
    {
        puts(m[i]);
    }
}

coord_t *check_adj_for_bfs(maze_t *m, char **colori, coord_t *** prev,
                           int **dist, queue_t *q, coord_t *c, char t)
{
    coord_t *c1, *c2, *c3, *c4;
    c1 = check_for_bfs(m, colori, prev, dist, q, c, c->x - 1, c->y, t);
    if (c1 != NULL)
        return c1;
    c2 = check_for_bfs(m, colori, prev, dist, q, c, c->x + 1, c->y, t);
    if (c2 != NULL)
        return c2;
    c3 = check_for_bfs(m, colori, prev, dist, q, c, c->x, c->y - 1, t);
    if (c3 != NULL)
        return c3;
    c4 = check_for_bfs(m, colori, prev, dist, q, c, c->x, c->y + 1, t);
    if (c4 != NULL)
        return c4;
    return NULL;
}

coord_t *bfs(maze_t *m, int y, int x, char t, int *distanza)
{
    coord_t *finish = NULL;
    queue_t *q = queue_init();
    if (q == NULL)
        perror("memory error"), exit(EXIT_FAILURE);

    char **colori = malloc(sizeof(char *) * m->height);
    if (colori == NULL)
        perror("memory error"), exit(EXIT_FAILURE);

    int **dist = malloc(sizeof(int *) * m->height);
    if (dist == NULL)
        perror("memory error"), exit(EXIT_FAILURE);

    coord_t *** prev = malloc(sizeof(coord_t **) * m->height);
    if (prev == NULL)
        perror("memory error"), exit(EXIT_FAILURE);

    coord_t *c = malloc(sizeof(coord_t));
    if (c == NULL)
        perror("memory error"), exit(EXIT_FAILURE);

    for (int i = 0; i < m->height; ++i)
    {
        colori[i] = malloc(sizeof(char) * m->width);
        if (colori[i] == NULL)
            perror("memory error"), exit(EXIT_FAILURE);

        prev[i] = malloc(sizeof(coord_t *) * m->width);
        if (prev[i] == NULL)
            perror("memory error"), exit(EXIT_FAILURE);

        dist[i] = malloc(sizeof(int) * m->width);
        if (dist[i] == NULL)
            perror("memory error"), exit(EXIT_FAILURE);
        for (int j = 0; j < m->width; ++j)
        {
            colori[i][j] = 'b';
            dist[i][j] = INT_MAX;
            prev[i][j] = NULL;
        }
    }
    colori[y][x] = 'g';
    dist[y][x] = 0;

    // fine inizializzazioni di colori, distanza, code, etc

    c->y = y;
    c->x = x;
    queue_push(q, c);
    queue_t *to_delete = queue_init();

    while (!queue_is_empty(q) && finish == NULL)
    {
        c = queue_top(q);
        queue_pop(q);
        finish = check_adj_for_bfs(m, colori, prev, dist, q, c, t);
        queue_push(to_delete, c);
        colori[c->y][c->x] = 'n';
    }
    coord_t *temp3 = NULL;
    if (finish != NULL)
    {
        *distanza = dist[finish->y][finish->x];
        coord_t *temp = finish, *temp2;
        temp3 = temp2 = temp;
        while (temp != NULL)
        {
            temp3 = temp2;
            temp2 = temp;
            temp = prev[temp->y][temp->x];

        }
        coord_t *boh = malloc(sizeof(coord_t));
        if (boh == NULL)
        {
            perror("memory error"), exit(EXIT_FAILURE);
        }
        boh->x = temp3->x;
        boh->y = temp3->y;
        temp3 = boh;
    }

    for (int i = 0; i < m->height; ++i)
    {
        free(colori[i]);
        free(dist[i]);
        free(prev[i]);
    }
    free(prev);
    free(dist);
    free(colori);

    while (!queue_is_empty(to_delete))
    {
        c = queue_top(to_delete);
        free(c);
        queue_pop(to_delete);
    }
    while (!queue_is_empty(q))
    {
        c = queue_top(q);
        free(c);
        queue_pop(q);
    }
    colori = NULL;
    free(q);
    free(to_delete);
    return temp3;
}

void relax(maze_t *m, heap_t *h, coord_t *c, int **dist, coord_t *** prev,
           int *** index, char *to_ignore, int y, int x)
{
    if (char_is_in_string(m->map[y][x], to_ignore) == 1)
        if (dist[y][x] > dist[c->y][c->x] + 1)
        {
            //if (m->map[y][x] != 'F') m->map[y][x] = 'O';
            //print_map(m->map, m->height);
            h->decrease_key(h, *(index[y][x]), dist[c->y][c->x] + 1);
            prev[y][x] = c;
            //usleep(10000 * 5);
            dist[y][x] = dist[c->y][c->x] + 1;
            //if (m->map[y][x] == 'O') m->map[y][x] = ' ';

        }
}

void relax_adj(maze_t *m, heap_t *h, coord_t *c, int **dist, coord_t *** prev,
               int *** index, char *to_ignore)
{
    relax(m, h, c, dist, prev, index, to_ignore, c->y, (c->x - 1 + m->width) % m->width);
    relax(m, h, c, dist, prev, index, to_ignore, c->y, (c->x + 1 + m->width) % m->width);
    relax(m, h, c, dist, prev, index, to_ignore, (c->y - 1 + m->height) % m->height, c->x);
    relax(m, h, c, dist, prev, index, to_ignore, (c->y + 1 + m->height) % m->height, c->x);
}

coord_t *dijkstra(maze_t *m, int y, int x, char t, int *distanza)
{
    coord_t *c = NULL;
    heap_t *h = heap_create(1, 0, 0, NULL, NULL);
    int **dist = calloc(m->height, sizeof(int *));
    if (dist == NULL)
        perror("memory error"), exit(EXIT_FAILURE);

    int ***index = calloc(m->height, sizeof(int **));
    if (index == NULL)
        perror("memory error"), exit(EXIT_FAILURE);

    queue_t *to_delete = queue_init();

    coord_t *** prev = calloc(m->height, sizeof(coord_t **));
    if (prev == NULL)
        perror("memory error"), exit(EXIT_FAILURE);
    for (int i = 0; i < m->height; ++i)
    {
        prev[i] = calloc(m->width, sizeof(coord_t *));
        if (prev[i] == NULL)
            perror("memory error"), exit(EXIT_FAILURE);

        dist[i] = calloc(m->width, sizeof(int));
        if (dist[i] == NULL)
            perror("memory error"), exit(EXIT_FAILURE);

        index[i] = calloc(m->width, sizeof(int *));
        if (index[i] == NULL)
            perror("memory error"), exit(EXIT_FAILURE);
        for (int j = 0; j < m->width; ++j)
        {
            dist[i][j] = INT_MAX - 10;
            if (i == y && j == x)
                dist[i][j] = 0;
            prev[i][j] = NULL;

            /* qui inizializzo l'heap. inserisco tutti le caselle della matrice.
             * la chiave di ogni casella sarà la distanza dalla sorgente.
             * l'indice di ogni elemento dell'heap sarà uguale a
             * (m->width * numero_di_riga) + numero_di_colonna
             */
            c = malloc(sizeof(coord_t));
            if (c == NULL)
                perror("memory allocation"), exit(EXIT_FAILURE);
            c->x = j;
            c->y = i;

            // ROBA DI DEBUG ATTENZIONE

            index[i][j] = h->insert_key(h, dist[i][j], c);
            queue_push(to_delete, c);
        }
    }
    char *to_ignore;
    if (t == 'S')
        to_ignore = " SE";
    else if (t == 'F')
        to_ignore = " FE";
    c = NULL;

    heap_element_t *he;
    coord_t *temp3 = NULL;
    while (h->size > 1)
    {

        he = (h->extract_top(h));
        c = he->info;
        relax_adj(m, h, c, dist, prev, index, to_ignore);
        if (m->map[c->y][c->x] == t)
        {
            *distanza = dist[c->y][c->x];
            coord_t *temp = c, *temp2;
            temp3 = temp2 = temp;
            while (temp != NULL)
            {
                temp3 = temp2;
                temp2 = temp;
                temp = prev[temp->y][temp->x];

            }
        }
        free(he);
    }

    for (int i = 0; i < m->height; ++i)
    {
        free(dist[i]);
        free(prev[i]);
        free(index[i]);
    }
    free(prev);
    free(index);
    free(dist);

    while (!queue_is_empty(to_delete))
    {
        c = queue_top(to_delete);
        if (c != temp3) free(c);
        queue_pop(to_delete);
    }

    delete_heap(h);
    free(to_delete);
    return temp3;
}
