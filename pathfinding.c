#include "main.h"
#include <string.h>
#include <stdlib.h>

/* 8-direction offsets matching Python: [-1,0],[0,-1],[1,0],[0,1],
   [-1,-1],[1,-1],[1,1],[-1,1]  */
static const int WAYS[PF_WAYS][2] = {
    {-1, 0},{0,-1},{1, 0},{0, 1},
    {-1,-1},{1,-1},{1, 1},{-1,1}
};

static void get_next_nodes(const Game *game, int x, int y,
                            int *out_x, int *out_y, int *count)
{
    *count = 0;
    for (int i = 0; i < PF_WAYS; i++) {
        int nx = x + WAYS[i][0];
        int ny = y + WAYS[i][1];
        if (!map_is_wall(&game->map, nx, ny)) {
            out_x[*count] = nx;
            out_y[*count] = ny;
            (*count)++;
        }
    }
}

void pathfinding_init(PathFinding *pf, Game *game) {
    memset(pf, 0, sizeof(*pf));
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            if (!map_is_wall(&game->map, x, y)) {
                int nx[PF_WAYS], ny[PF_WAYS], cnt;
                get_next_nodes(game, x, y, nx, ny, &cnt);
                pf->neighbour_count[y][x] = cnt;
                for (int i = 0; i < cnt; i++) {
                    pf->neighbour_x[y][x][i] = nx[i];
                    pf->neighbour_y[y][x][i] = ny[i];
                }
            }
        }
    }
}

/* BFS from start to goal.
   parent_x/y: for each visited cell, which cell reached it.
   Returns 1 if goal was reached, 0 otherwise. */
static int bfs(PathFinding *pf, const Game *game,
               int sx, int sy, int gx, int gy,
               int parent_x[MAP_ROWS][MAP_COLS],
               int parent_y[MAP_ROWS][MAP_COLS])
{
    int visited[MAP_ROWS][MAP_COLS];
    memset(visited,  0, sizeof(visited));
    memset(parent_x, -1, MAP_ROWS * MAP_COLS * sizeof(int));
    memset(parent_y, -1, MAP_ROWS * MAP_COLS * sizeof(int));

    /* BFS queue: flat array acting as a FIFO */
    int qx[MAP_ROWS * MAP_COLS];
    int qy[MAP_ROWS * MAP_COLS];
    int head = 0, tail = 0;

    qx[tail] = sx; qy[tail] = sy; tail++;
    visited[sy][sx] = 1;

    while (head < tail) {
        int cx = qx[head], cy = qy[head];
        head++;

        if (cx == gx && cy == gy) return 1;

        int cnt = pf->neighbour_count[cy][cx];
        for (int i = 0; i < cnt; i++) {
            int nx = pf->neighbour_x[cy][cx][i];
            int ny = pf->neighbour_y[cy][cx][i];

            if (!visited[ny][nx] &&
                !game->object_handler.npc_positions[ny][nx])
            {
                visited[ny][nx]  = 1;
                parent_x[ny][nx] = cx;
                parent_y[ny][nx] = cy;
                qx[tail] = nx;
                qy[tail] = ny;
                tail++;
            }
        }
    }
    return 0;
}

/* Reconstruct path and return the next step after start toward goal.
   Mirrors Python: path = [goal]; walk parents back to start; return path[-1] */
Pos2D pathfinding_get_path(PathFinding *pf, Game *game,
                            int sx, int sy, int gx, int gy)
{
    Pos2D result = { sx, sy }; /* fallback: stay in place */

    if (sx == gx && sy == gy) return result;

    int parent_x[MAP_ROWS][MAP_COLS];
    int parent_y[MAP_ROWS][MAP_COLS];

    if (!bfs(pf, game, sx, sy, gx, gy, parent_x, parent_y))
        return result;

    /* Walk backwards from goal to start, collecting path */
    int path_x[MAP_ROWS * MAP_COLS];
    int path_y[MAP_ROWS * MAP_COLS];
    int len = 0;

    int cx = gx, cy = gy;
    path_x[len] = cx; path_y[len] = cy; len++;

    while (!(cx == sx && cy == sy)) {
        int px = parent_x[cy][cx];
        int py = parent_y[cy][cx];
        if (px == -1) break; /* goal was unreachable from here */
        cx = px; cy = py;
        if (cx == sx && cy == sy) break;
        path_x[len] = cx; path_y[len] = cy; len++;
    }

    if (len > 0) {
        /* path[-1] = last element = node just after start toward goal */
        result.x = path_x[len - 1];
        result.y = path_y[len - 1];
    }
    return result;
}
