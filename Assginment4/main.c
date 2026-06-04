#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "GameEntity.h"
#include "collision.h"
#include "objects/house.h"
#include "objects/player.h"
#include "objects/tree.h"
#include "physics/force.h"

#define MAX_ENTITIES 32
#define TREE_COUNT 3

static float g_angle = 0.0f;
static float g_lx = 0.0f;
static float g_lz = -1.0f;

static float g_cam_x = 0.0f;
static float g_cam_z = 5.0f;

static int g_key_forward = 0;
static int g_key_back = 0;
static int g_key_left = 0;
static int g_key_right = 0;

static int g_prev_time_ms = 0;

static CollisionHeap *g_collision_heap = NULL;
static GameEntity *g_entities[MAX_ENTITIES] = {NULL};
static size_t g_entity_count = 0;

static GameEntity *g_player = NULL;

static void add_entity(GameEntity *entity) {
    if (!entity) {
        return;
    }

    if (g_entity_count < MAX_ENTITIES) {
        g_entities[g_entity_count++] = entity;
    }
}

static void draw_ground(void) {
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glVertex3f(-100.0f, 0.0f, -100.0f);
    glVertex3f(-100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, 100.0f);
    glVertex3f(100.0f, 0.0f, -100.0f);
    glEnd();
}

static void on_entity_collision(GameEntity *self,
                                GameEntity *other,
                                GameObject *self_part,
                                GameObject *other_part) {
    if (!self || !other || !self_part || !other_part) {
        return;
    }

    printf("[Collision] %s(%s) <-> %s(%s)\n",
           self->name,
           self_part->name,
           other->name,
           other_part->name);
}

static void update_camera_from_player(void) {
    if (!g_player) {
        return;
    }

    g_cam_x = g_player->x;
    g_cam_z = g_player->z;
}

static void change_size(int w, int h) {
    float ratio = 0.0f;

    if (h == 0) {
        h = 1;
    }

    ratio = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45.0f, ratio, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

static void render_scene(void) {
    size_t i = 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
        g_cam_x,
        1.0f,
        g_cam_z,
        g_cam_x + g_lx,
        1.0f,
        g_cam_z + g_lz,
        0.0f,
        1.0f,
        0.0f);

    draw_ground();

    for (i = 0; i < g_entity_count; ++i) {
        game_entity_draw(g_entities[i]);
    }

    glutSwapBuffers();
}

static float compute_dt_seconds(void) {
    int now = glutGet(GLUT_ELAPSED_TIME);
    int delta = now - g_prev_time_ms;

    if (g_prev_time_ms == 0) {
        g_prev_time_ms = now;
        return 1.0f / 60.0f;
    }

    g_prev_time_ms = now;

    if (delta < 1) {
        delta = 1;
    }
    if (delta > 33) {
        delta = 33;
    }

    return (float)delta / 1000.0f;
}

static void update_input_forces(float dt) {
    const float turn_speed = 2.4f;
    const float move_force = 55.0f;

    if (g_key_left) {
        g_angle -= turn_speed * dt;
    }
    if (g_key_right) {
        g_angle += turn_speed * dt;
    }

    g_lx = sinf(g_angle);
    g_lz = -cosf(g_angle);

    if (!g_player) {
        return;
    }

    if (g_key_forward) {
        Force forward = force_create(g_lx * move_force, 0.0f, g_lz * move_force, dt);
        game_entity_apply_force(g_player, forward);
    }

    if (g_key_back) {
        Force backward = force_create(-g_lx * move_force, 0.0f, -g_lz * move_force, dt);
        game_entity_apply_force(g_player, backward);
    }
}

static void update_scene(void) {
    size_t i = 0;
    float dt = compute_dt_seconds();

    for (i = 0; i < g_entity_count; ++i) {
        game_entity_begin_frame(g_entities[i]);
    }

    update_input_forces(dt);

    for (i = 0; i < g_entity_count; ++i) {
        game_entity_integrate_physics(g_entities[i], dt);
    }

    collision_system_call(g_collision_heap);

    for (i = 0; i < g_entity_count; ++i) {
        game_entity_resolve_collisions(g_entities[i]);
    }

    update_camera_from_player();
    glutPostRedisplay();
}

static void process_special_keys_down(int key, int xx, int yy) {
    (void)xx;
    (void)yy;

    switch (key) {
        case GLUT_KEY_UP:
            g_key_forward = 1;
            break;
        case GLUT_KEY_DOWN:
            g_key_back = 1;
            break;
        case GLUT_KEY_LEFT:
            g_key_left = 1;
            break;
        case GLUT_KEY_RIGHT:
            g_key_right = 1;
            break;
        default:
            break;
    }
}

static void process_special_keys_up(int key, int xx, int yy) {
    (void)xx;
    (void)yy;

    switch (key) {
        case GLUT_KEY_UP:
            g_key_forward = 0;
            break;
        case GLUT_KEY_DOWN:
            g_key_back = 0;
            break;
        case GLUT_KEY_LEFT:
            g_key_left = 0;
            break;
        case GLUT_KEY_RIGHT:
            g_key_right = 0;
            break;
        default:
            break;
    }
}

static void process_normal_keys(unsigned char key, int xx, int yy) {
    (void)xx;
    (void)yy;

    if (key == 27 || key == 'x' || key == 'X') {
        exit(0);
    }
}

static int setup_world(void) {
    int i = 0;
    GameEntity *house = NULL;

    g_collision_heap = collision_heap_create(64);
    if (!g_collision_heap) {
        return 0;
    }

    house = house_create(g_collision_heap, 0.0f, 0.0f, 0.0f, on_entity_collision);
    if (!house) {
        return 0;
    }
    add_entity(house);

    for (i = 0; i < TREE_COUNT; ++i) {
        GameEntity *tree = tree_create(
            g_collision_heap,
            4.0f,
            0.0f,
            3.0f + (float)i * 1.1f,
            on_entity_collision);

        if (!tree) {
            return 0;
        }

        add_entity(tree);
    }

    g_player = player_create(g_collision_heap, 0.0f, 0.0f, 5.0f, on_entity_collision);
    if (!g_player) {
        return 0;
    }
    add_entity(g_player);

    update_camera_from_player();
    return 1;
}

static void cleanup_world(void) {
    size_t i = 0;

    for (i = 0; i < g_entity_count; ++i) {
        game_entity_destroy(g_entities[i], g_collision_heap);
        g_entities[i] = NULL;
    }

    g_entity_count = 0;
    g_player = NULL;

    if (g_collision_heap) {
        collision_heap_destroy(g_collision_heap);
        g_collision_heap = NULL;
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Assignment 4 - Primitive GameObjects + Physics");

    if (!setup_world()) {
        fprintf(stderr, "World kurulumu basarisiz oldu.\n");
        cleanup_world();
        return 1;
    }

    glutDisplayFunc(render_scene);
    glutReshapeFunc(change_size);
    glutIdleFunc(update_scene);
    glutSpecialFunc(process_special_keys_down);
    glutSpecialUpFunc(process_special_keys_up);
    glutKeyboardFunc(process_normal_keys);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);

    glutMainLoop();

    cleanup_world();
    return 0;
}
