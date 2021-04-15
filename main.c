#include <unistd.h>
#include <limits.h>
#include <stdio.h>

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "compare.h"
#include "maze.h"
#include "heap.h"
#include <ncurses.h>
#define ESC 27
#define SPACE 32
#define ENEMY_SCR 'E' | COLOR_PAIR(4) | A_BLINK | A_BOLD

int x = 0, y = 0, row, col, fine = 0, actual_maze, n_mazes;
WINDOW *win;
maze_t **mazes;

void clear_stdin()
{
    int ch;
    while ((ch = getchar()) != EOF && ch != '\n');
}


void destroy_win(WINDOW *local_win)
{
    /* box(local_win, ' ', ' '); : This won't produce the desired
     * result of erasing the window. It will leave it's four corners
     * and so an ugly remnant of window.
     */
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    /* The parameters taken are
     * 1. win: the window on which to operate
     * 2. ls: character to be used for the left side of the window
     * 3. rs: character to be used for the right side of the window
     * 4. ts: character to be used for the top side of the window
     * 5. bs: character to be used for the bottom side of the window
     * 6. tl: character to be used for the top left corner of the window
     * 7. tr: character to be used for the top right corner of the window
     * 8. bl: character to be used for the bottom left corner of the window
     * 9. br: character to be used for the bottom right corner of the window
     */
    wrefresh(local_win);
    delwin(local_win);
}

coord_t *get_enemy_new_move(enemy_t *enemy)
{
    maze_t *m = enemy->m;
    int dist = INT_MAX;
    coord_t *c = NULL, *c1 = NULL;
    if (enemy->pursuit == 0)
    {
        c = enemy->strategy(m, enemy->y, enemy->x, 'F', &dist);
        dist = INT_MAX;
        c1 = enemy->strategy(m, enemy->y, enemy->x, 'S', &dist);
        if (dist <= 10)
        {
            free(c);
            enemy->pursuit = 1;
            c = c1;
        }
    }
    else
    {
        c = enemy->strategy(m, enemy->y, enemy->x, 'S', &dist);
        if (dist > 10) enemy->pursuit = 0;
    }
    return c;
}


void set_position_on_map(maze_t *maze, int y, int x, int c)
{
    char **m = maze->map;
    m[y][x] = c;
}

char get_type_of_position(maze_t *maze, int y, int x)
{
    return maze->map[y][x];
}

void set_position_on_screen(WINDOW *win, int y, int x, int c)
{
    wmove(win, y, x);
    waddch(win, c);
}

void set_position(maze_t *m, WINDOW *win, int y, int x, int c)
{
    set_position_on_map(m, y, x, c);
    set_position_on_screen(win, y, x, c);
}

int manage_enemies(enemy_t *enemy)
{
    maze_t *m = enemy->m;
    int flag = 0;

    coord_t *c = NULL;

    c = get_enemy_new_move(enemy);

    // ha trovato qualcosa?
    if (c != NULL)
    {
        char ch = get_type_of_position(m, c->y, c->x);

        // non facciamo accavallare i nemici!
        if (ch != 'E')
        {
            int oldy = enemy->y, oldx = enemy->x;

            // comunque vada, la vecchia casella va sbiancata
            set_position_on_map(enemy->m, enemy->y, enemy->x, ' ');
            if (enemy->refresh_win == 1)
            {
                set_position_on_screen(win, oldy, oldx, ' ');
            }
            /* se incontra s, bisogna tornare ai punti di partenza per entrambi */
            if (ch == 'S')
            {
                x = 1; y = 1;
                set_position_on_map(m, c->y, c->x, ' ');
                set_position_on_screen(win, c->y, c->x, ' ');
                set_position_on_map(m, 1, 1, 'S');
                enemy->pursuit = 0;
                enemy->y = enemy->original_y; enemy->x = enemy->original_x;
                set_position_on_map(m, enemy->y, enemy->x, 'E');
            }

            // se incontro la fine, devo cambiare labirinto o vincere
            else if (ch == 'F')
            {
                flag =  1;
            }

            // altrimenti tutto normale
            else
            {
                enemy->y = c->y; enemy->x = c->x;
                set_position_on_map(m, enemy->y, enemy->x, 'E');
            }


            if (enemy->refresh_win && ch != 'F')
            {
                set_position_on_screen(win, enemy->y, enemy->x, ENEMY_SCR);
            }
        }
    }
    free(c);

    if (flag == 1)
    {
        enemy->actual_maze++;

        // se ho finito, il nemico ha vinto!
        if (enemy->actual_maze == n_mazes)
        {
            destroy_win(win);
            refresh();
            clear();
            move(0, 0);
            refresh();
            printw("Hai perso.");
            refresh();
            return -1;
        }

        // sennò, riposizioniamoci da qualche parte random nel nuovo labirinto
        else
        {
            m = enemy->m = mazes[enemy->actual_maze];
            int i, j;
            do
            {
                flag = 1;
                i = rand() % enemy->m->height;
                j = rand() % enemy->m->width;
                if (get_type_of_position(m, i, j) == ' ')
                {
                    set_position_on_map(m, i, j, 'E');
                    enemy->y = enemy->original_y = i; enemy->x = enemy->original_x = j;
                    flag = 0;
                }
            }
            while (flag);

            // se sono nel labirinto visualizzato, devo refreshare le mie mosse

            if (actual_maze != enemy->actual_maze) enemy->refresh_win = 0;
            else
            {
                enemy->refresh_win = 1;
            }
            return 0;
        }
    }
    return 1;
}

WINDOW *create_boxed_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    /* Create a window for the boundinx box */
    local_win = newwin(height, width, starty, startx);
    /* Create the bounding box */
    box(local_win, 0 , 0);
    /* 0, 0 gives default characters
                           * for the vertical and horizontal
                           * lines            */
    wrefresh(local_win);        /* Show all in window       */
    /* Create the window inside the bounding box */
    local_win = newwin(height - 2, width - 2, starty + 1, startx + 1);
    /*        local_win = derwin(local_win,height, width, 1, 1); */
    wrefresh(local_win);        /* Show that box        */
    return local_win;
}

int manage_pg(WINDOW *win, maze_t *m)
{
    static char character = '>';
    int to_return = 0;
    static int key,
           lastkey = 0,
           oldy, oldx;

    key = wgetch(win); /* Try to read a key */
    flushinp();  /* Flush away all the input present in the keyboard buffer */
    wrefresh(win); /* Show effects on schreen */

    if (key != KEY_LEFT && key != KEY_RIGHT && key != KEY_UP && key != KEY_DOWN  && key != ESC && key != SPACE)
    {
        key = lastkey;  /* If no useful key has been pressed, use last key pressed instead */
    }

    oldx = x; oldy = y;
    char ch;
    switch (key)  /* Process key pressed */
    {
    case SPACE : /* Stop character at current position, i.e. do nothing */
        lastkey = key; /* Remember last key pressed */
        break;
    case ESC :
        fine = 1;
        break;
    case KEY_UP : /* Move character up in the toroidal map */
        y = ((y + row) - 1) % row;
        ch = get_type_of_position(m, y, x);
        if (ch != ' ' && ch != 'F') y = oldy;
        else character = 'V';
        break;
    case KEY_DOWN : /* Move character down in the toroidal map */
        y = ((y + row) + 1) % row;
        ch = get_type_of_position(m, y, x);
        if (ch != ' ' && ch != 'F') y = oldy;
        else character = '^';
        break;
    case KEY_RIGHT : /* Move character right in the toroidal map */
        x = ((x + col) + 1) % col;
        ch = get_type_of_position(m, y, x);
        if (ch != ' ' && ch != 'F') x = oldx;
        else character = '<';
        break;
    case KEY_LEFT : /* Move character left in the toroidal map */
        x = ((x + col) - 1) % col;
        ch = get_type_of_position(m, y, x);
        if (ch != ' ' && ch != 'F') x = oldx;
        else character = '>';
        break;
    }

    set_position_on_map(m, oldy, oldx, ' ');
    set_position_on_screen(win, oldy, oldx, ' ' | COLOR_PAIR(2));

    if (get_type_of_position(m, y, x) == 'F')
    {
        to_return = 1;

    }
    else
        set_position_on_map(m, y, x, 'S');
    set_position_on_screen(win, y, x, character | A_BOLD | COLOR_PAIR(3));

    lastkey = key; /* Remember last key pressed */
    wrefresh(win); /* Show effects on schreen */
    return to_return;
}

WINDOW *create_maze(maze_t *m)
{
    attron(COLOR_PAIR(1)); /* Set character colors to 2 (see init_pair) */
    refresh();

    WINDOW *win = create_boxed_newwin(m->height + 2, m->width + 3, 0, 0); /* Create new windows within the terminal
                                                              surrounded by a box */
    keypad(win, TRUE); /* Allow control keys to be read */
    getmaxyx(win, row, col); /* Get the number of rows and columns of window */
    nodelay(win, TRUE); /* Keyboard read will be non-blocking in window */

    /* print maze on screen*/
    wmove(win, 0, 0);
    for (int i = 0; i < m->height; ++i)
    {
        wprintw(win, m->map[i]);
        wprintw(win, "\n");
    }
    wrefresh(win);
    col = col - 1;
    x = 1, y = 1;

    int i, j, flag;
    do
    {
        flag = 1;
        i = rand() % row;
        j = rand() % col;
        if (get_type_of_position(m, i, j) == ' ')
        {
            m->finish_x = j; m->finish_y = i;
            set_position_on_map(m, i, j, 'F');
            set_position_on_screen(win, i, j, 'F');
            flag = 0;
        }
    }
    while (flag);

    return win;
}

void ncurses_init()
{
    system("stty sane");
    srand(time(NULL));
    initscr();              /* Start the curses mode */
    clear();
    keypad(stdscr, TRUE); /* Allow control keys to be read */
    curs_set(0);  /* Hide cursor */
    noecho();    /* Do not echo pressed key on the screen */
    cbreak();   /* Line buffering disabled, pass on everything */
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support colors\n");
        fine = 1;
    }
    start_color();          /* Start colors */
    init_pair(1, COLOR_WHITE, COLOR_BLACK); /* Define character color pair 1 */
    init_pair(2, COLOR_BLACK, COLOR_BLACK); /* Define character color pair 2 */
    init_pair(3, COLOR_RED, COLOR_BLUE);  /* Define character color pari 3 */
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
}

void roba_debug(maze_t *m, enemy_t *enemy)
{
    FILE *fd, *fd2;
    fd2 = fopen("debug2.txt", "w");
    for (int i = 0; i < m->height; ++i)
    {
        fprintf(fd2, m->map[i]);
        fprintf(fd2, "\n");
    }
    fclose(fd2);
    if (enemy->refresh_win != 1)
    {
        fd = fopen("debug.txt", "w");

        for (int i = 0; i < enemy->m->height; ++i)
        {
            fprintf(fd, enemy->m->map[i]);
            fprintf(fd, "\n");
        }
        fclose(fd);
    }
}
int choose_diff()
{
    int diff = 4;
    printf("Scegliere una difficoltà per il gioco\n");
    printf("1) Facile\n");
    printf("2) Normale\n");
    printf("3) Difficile\n");
    char c = getchar(); getchar();
    switch (c)
    {
    case '1':
        diff = 4;
        break;
    case '2':
        diff = 3;
        break;
    case '3':
        diff = 2;
        break;
    default:
        break;
    }
    return diff;
}

maze_t **init_mazes(int n_mazes, char **argv)
{
    int count = 1;
    FILE *fd;
    queue_t *maz = queue_init();
    maze_t *m, **mazes = malloc(sizeof(maze_t *)*n_mazes);
    if (mazes == NULL)
    {
        perror("memory error"), exit(EXIT_FAILURE);
    }
    while (argv[count] != NULL)
    {
        fd = fopen(argv[count], "r");
        m = config_maze_from_file(fd);
        fclose(fd);
        queue_push(maz, m);
        ++count;
    }
    for (int i = 0; i < n_mazes; ++i)
    {
        mazes[i] = (maze_t *)queue_top(maz);
        queue_pop(maz);
    }
    free(maz);
    return mazes;
}

enemy_t **init_enemies(maze_t *m, int diff, int *numb)
{
    int n_enemies, i, j, flag = 1;

    if (diff == 4)
    {
        n_enemies = 3;
    }
    else if (diff == 3)
    {
        n_enemies = 4;
    }
    else
    {
        n_enemies = 5;
    }

    enemy_t **enemies = malloc(sizeof(enemy_t *) * n_enemies);
    if (enemies == NULL)
    {
        perror("memory error"), exit(EXIT_FAILURE);
    }

    for (int k = 0; k < n_enemies; ++k)
    {
        enemies[k] = malloc(sizeof(enemy_t));
        if (enemies[k] == NULL)
        {
            perror("memory error"), exit(EXIT_FAILURE);
        }
        enemies[k]->m = m;
        enemies[k]->refresh_win = 1;
        enemies[k]->actual_maze = 0;
        enemies[k]->strategy = dijkstra;
        enemies[k]->clock = diff;
        do
        {
            flag = 1;
            i = rand() % m->height;
            j = rand() % m->width;
            if (m->map[i][j] == ' ')
            {
                m->map[i][j] = 'E';
                enemies[k]->y = enemies[k]->original_y = i; enemies[k]->x = enemies[k]->original_x = j;
                flag = 0;
            }
        }
        while (flag);
    }

    *numb = n_enemies;
    return enemies;
}

int main(int argc, char **argv)
{
    system("stty sane");
    system("clear");
    if (argc < 2) perror("Non hai dato labirinti in input"), exit(EXIT_FAILURE);
    int diff = 1, count = 1, res1, n_enemies;
    n_mazes = argc - 1;
    actual_maze = 0;
    srand(time(NULL));

    maze_t *m;
    enemy_t **enemies;

    // scelta difficoltà
    diff = choose_diff();

    // lettura labirinti
    mazes = init_mazes(n_mazes, argv);
    m = mazes[0];

    // crea primo livello e nemici
    ncurses_init();
    win = create_maze(m);
    enemies = init_enemies(m, diff, &n_enemies);

    /* fino a qui è semplicemente per gestire il primo labirinto, i nemici, etc */
    count = 0;
    int return_of_enemy;
    while (!fine)
    {
        usleep(100000); count++;
        for (int j = 0; j < n_enemies; j++)
        {
            if (count % enemies[j]->clock == 0)
                /* ogni quanti "turni" devono muoversi i nemici */
            {
                return_of_enemy = manage_enemies(enemies[j]);
                if (return_of_enemy >= 0)
                    wrefresh(win);
                else
                    goto fine_partita;
            }
        }
        res1 = manage_pg(win, m);
        if (res1)
        {
            destroy_win(win);
            actual_maze++;
            if (actual_maze == n_mazes)
            {
                refresh();
                move(0, 0);
                clear();
                refresh();
                printw("Hai vinto.");
                refresh();
                fine = 1;
                break;
            }
            m = mazes[actual_maze];
            win = create_maze(m);
            for (int i = 0; i < n_enemies; ++i)
            {
                if (enemies[i]->actual_maze == actual_maze)
                {
                    enemies[i]->refresh_win = 1;
                }
                else
                {
                    enemies[i]->refresh_win = enemies[i]->pursuit = 0;
                }
            }
            refresh();
            wrefresh(win);
        }
    }
fine_partita:;

    getch();
    attroff(COLOR_PAIR(1)); /* Restore screen colors */
    clear();
    echo();
    curs_set(1);
    endwin();   /* Close the window */
    for (int i = 0; i < n_mazes; ++i)
    {
        for (int j = 0; j < mazes[i]->height; ++j)
        {
            free(mazes[i]->map[j]);
        }
        free(mazes[i]->map);
        free(mazes[i]);
    }
    for (int i = 0; i < n_enemies; ++i)
    {
        free(enemies[i]);
    }
    free(mazes);
    free(enemies);
    system("stty sane");
    system("clear");
    return 0;
}
