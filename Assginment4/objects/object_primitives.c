#include "object_primitives.h"

void draw_box(float w, float h, float d) {
    const float x = w / 2.0f;
    const float y = h / 2.0f;
    const float z = d / 2.0f;

    glBegin(GL_QUADS);

    glVertex3f(-x, -y, z);
    glVertex3f(x, -y, z);
    glVertex3f(x, y, z);
    glVertex3f(-x, y, z);

    glVertex3f(-x, -y, -z);
    glVertex3f(-x, y, -z);
    glVertex3f(x, y, -z);
    glVertex3f(x, -y, -z);

    glVertex3f(-x, -y, -z);
    glVertex3f(-x, -y, z);
    glVertex3f(-x, y, z);
    glVertex3f(-x, y, -z);

    glVertex3f(x, -y, -z);
    glVertex3f(x, y, -z);
    glVertex3f(x, y, z);
    glVertex3f(x, -y, z);

    glVertex3f(-x, y, -z);
    glVertex3f(-x, y, z);
    glVertex3f(x, y, z);
    glVertex3f(x, y, -z);

    glVertex3f(-x, -y, -z);
    glVertex3f(x, -y, -z);
    glVertex3f(x, -y, z);
    glVertex3f(-x, -y, z);

    glEnd();
}

void draw_plane(float w, float d) {
    const float x = w / 2.0f;
    const float z = d / 2.0f;

    glBegin(GL_QUADS);
    glVertex3f(-x, 0.0f, -z);
    glVertex3f(-x, 0.0f, z);
    glVertex3f(x, 0.0f, z);
    glVertex3f(x, 0.0f, -z);
    glEnd();
}

void draw_cylinder(float radius, float height) {
    GLUquadric *quad = gluNewQuadric();

    if (!quad) {
        return;
    }

    gluQuadricDrawStyle(quad, GLU_LINE);

    glPushMatrix();
    glTranslatef(0.0f, -height / 2.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quad, radius, radius, height, 20, 1);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void draw_sphere(float radius) {
    glutWireSphere(radius, 20, 20);
}
