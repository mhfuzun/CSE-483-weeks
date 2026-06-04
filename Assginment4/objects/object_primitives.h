#ifndef OBJECT_PRIMITIVES_H
#define OBJECT_PRIMITIVES_H

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void draw_box(float w, float h, float d);
void draw_plane(float w, float d);
void draw_cylinder(float radius, float height);
void draw_sphere(float radius);

#endif
