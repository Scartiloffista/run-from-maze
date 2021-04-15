#ifndef MAZE_H_
#define MAZE_H_

#include <time.h>
#include <stdio.h>
#include "collection.h"


typedef struct
{
    int x;
    int y;
} coord_t;

typedef struct _maze_t maze_t;

typedef coord_t *(*ENEMYSTRATEGYFN)(maze_t *m, int y, int x, char t, int *);

typedef struct
{
    int x;
    int y;
    int original_x;
    int original_y;
    int pursuit;
    int actual_maze;
    int refresh_win;
    int clock;
    maze_t *m;
    ENEMYSTRATEGYFN strategy;

} enemy_t;


struct _maze_t
{
    char **map;
    int finish_x;
    int finish_y;
    int height;
    int width;
};


char *get_delim(FILE *fd, char d);
maze_t *config_maze_from_file(FILE *fd);
coord_t *bfs(maze_t *m, int y, int x, char t, int *);
coord_t *dijkstra(maze_t *m, int y, int x, char t, int *distanza);
#endif /* MAZE_H_ */
