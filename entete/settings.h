#ifndef SETTINGS_H
#define SETTINGS_H

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WIDTH        1600
#define HEIGHT       900
#define HALF_WIDTH   (WIDTH / 2)
#define HALF_HEIGHT  (HEIGHT / 2)
#define FPS          0

#define PLAYER_POS_X       1.5f
#define PLAYER_POS_Y       5.0f
#define PLAYER_ANGLE       0.0f
#define PLAYER_SPEED       0.004f
#define PLAYER_ROT_SPEED   0.002f
#define PLAYER_SIZE_SCALE  60
#define PLAYER_MAX_HEALTH  100

#define MOUSE_SENSITIVITY   0.0003f
#define MOUSE_MAX_REL       40
#define MOUSE_BORDER_LEFT   100
#define MOUSE_BORDER_RIGHT  (WIDTH - MOUSE_BORDER_LEFT)

#define FLOOR_COLOR_R  30
#define FLOOR_COLOR_G  30
#define FLOOR_COLOR_B  30

#define FOV           (M_PI / 3.0)
#define HALF_FOV      (FOV / 2.0)
#define NUM_RAYS      (WIDTH / 2)
#define HALF_NUM_RAYS (NUM_RAYS / 2)
#define DELTA_ANGLE   (FOV / (double)NUM_RAYS)
#define MAX_DEPTH     40

#define SCREEN_DIST   ((double)(HALF_WIDTH) / tan(HALF_FOV))
#define SCALE         (WIDTH / NUM_RAYS)

#define TEXTURE_SIZE       256
#define HALF_TEXTURE_SIZE  (TEXTURE_SIZE / 2)

#define MAX_ANIMATION_FRAMES  32
#define MAX_OBJECTS_TO_RENDER 1024
#define MAX_SPRITES           60
#define MAX_NPCS              30
#define MAP_ROWS              32
#define MAP_COLS              16

#endif /* SETTINGS_H */
