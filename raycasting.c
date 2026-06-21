#include "main.h"
#include <math.h>
#include <string.h>

void raycasting_init(RayCasting *rc) {
    memset(rc, 0, sizeof(*rc));
}

void raycasting_ray_cast(RayCasting *rc, Game *game) {
    Player *p      = &game->player;
    Map    *m      = &game->map;

    int texture_vert = 1, texture_hor = 1;
    double ox = p->x, oy = p->y;
    int x_map = (int)ox, y_map = (int)oy;

    double ray_angle = p->angle - HALF_FOV + 0.0001;

    for (int ray = 0; ray < NUM_RAYS; ray++) {
        double sin_a = sin(ray_angle);
        double cos_a = cos(ray_angle);

        /* --- Horizontal intersections --- */
        double y_hor, dy_h;
        if (sin_a > 0) { y_hor = y_map + 1; dy_h =  1.0; }
        else           { y_hor = y_map - 1e-6; dy_h = -1.0; }

        double depth_hor = (y_hor - oy) / sin_a;
        double x_hor     = ox + depth_hor * cos_a;
        double delta_depth_h = dy_h / sin_a;
        double dx_h          = delta_depth_h * cos_a;

        for (int i = 0; i < MAX_DEPTH; i++) {
            int tx = (int)x_hor, ty = (int)y_hor;
            int val = map_get(m, tx, ty);
            if (val) { texture_hor = val; break; }
            x_hor     += dx_h;
            y_hor     += dy_h;
            depth_hor += delta_depth_h;
        }

        /* --- Vertical intersections --- */
        double x_vert, dx_v;
        if (cos_a > 0) { x_vert = x_map + 1; dx_v =  1.0; }
        else           { x_vert = x_map - 1e-6; dx_v = -1.0; }

        double depth_vert = (x_vert - ox) / cos_a;
        double y_vert     = oy + depth_vert * sin_a;
        double delta_depth_v = dx_v / cos_a;
        double dy_v          = delta_depth_v * sin_a;

        for (int i = 0; i < MAX_DEPTH; i++) {
            int tx = (int)x_vert, ty = (int)y_vert;
            int val = map_get(m, tx, ty);
            if (val) { texture_vert = val; break; }
            x_vert     += dx_v;
            y_vert     += dy_v;
            depth_vert += delta_depth_v;
        }

        /* Choose closest hit */
        double depth, offset;
        int    texture;
        if (depth_vert < depth_hor) {
            depth   = depth_vert;
            texture = texture_vert;
            y_vert  = fmod(y_vert, 1.0);
            offset  = (cos_a > 0) ? y_vert : (1.0 - y_vert);
        } else {
            depth   = depth_hor;
            texture = texture_hor;
            x_hor   = fmod(x_hor, 1.0);
            offset  = (sin_a > 0) ? (1.0 - x_hor) : x_hor;
        }

        /* Remove fish-bowl effect */
        depth *= cos(p->angle - ray_angle);

        /* Projection */
        double proj_height = SCREEN_DIST / (depth + 0.0001);

        rc->ray_casting_result[ray].depth       = (float)depth;
        rc->ray_casting_result[ray].proj_height = (float)proj_height;
        rc->ray_casting_result[ray].texture_id  = texture;
        rc->ray_casting_result[ray].offset      = (float)offset;

        ray_angle += DELTA_ANGLE;
    }
}

void raycasting_get_objects_to_render(RayCasting *rc, Game *game) {
    /* Reset — sprites will be appended after this call */
    rc->objects_count = 0;

    for (int ray = 0; ray < NUM_RAYS; ray++) {
        RaycastHit *h       = &rc->ray_casting_result[ray];
        float       ph      = h->proj_height;
        int         tex_id  = h->texture_id;
        float       offset  = h->offset;

        if (tex_id < 1 || tex_id >= NUM_WALL_TEXTURES) tex_id = 1;
        SDL_Texture *tex = game->object_renderer.wall_textures[tex_id];
        if (!tex) continue;

        SDL_Rect src, dst;

        if (ph < HEIGHT) {
            src.x = (int)(offset * (TEXTURE_SIZE - SCALE));
            src.y = 0;
            src.w = SCALE;
            src.h = TEXTURE_SIZE;

            dst.x = ray * SCALE;
            dst.y = HALF_HEIGHT - (int)(ph / 2);
            dst.w = SCALE;
            dst.h = (int)ph;
        } else {
            float texture_height = (float)TEXTURE_SIZE * HEIGHT / ph;
            src.x = (int)(offset * (TEXTURE_SIZE - SCALE));
            src.y = (int)(HALF_TEXTURE_SIZE - texture_height / 2.0f);
            src.w = SCALE;
            src.h = (int)texture_height;

            dst.x = ray * SCALE;
            dst.y = 0;
            dst.w = SCALE;
            dst.h = HEIGHT;
        }

        if (rc->objects_count < MAX_OBJECTS_TO_RENDER) {
            ObjectToRender *o = &rc->objects_to_render[rc->objects_count++];
            o->depth        = h->depth;
            o->texture      = tex;
            o->src_rect     = src;
            o->dst_rect     = dst;
            o->use_src_rect = 1;
        }
    }
}

void raycasting_update(RayCasting *rc, Game *game) {
    raycasting_ray_cast(rc, game);
    raycasting_get_objects_to_render(rc, game);
}
