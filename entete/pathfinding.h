#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "types.h"
#include "settings.h"

/* 8-direction neighbours */
#define PF_WAYS 8

struct PathFinding {
    /* adjacency list: for each cell, up to PF_WAYS walkable neighbours */
    int neighbour_x[MAP_ROWS][MAP_COLS][PF_WAYS];
    int neighbour_y[MAP_ROWS][MAP_COLS][PF_WAYS];
    int neighbour_count[MAP_ROWS][MAP_COLS];
};

void   pathfinding_init(PathFinding *pf, Game *game);
Pos2D  pathfinding_get_path(PathFinding *pf, Game *game,
                             int start_x, int start_y,
                             int goal_x,  int goal_y);

#endif /* PATHFINDING_H */
