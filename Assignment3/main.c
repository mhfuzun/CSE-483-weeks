// snowman.cpp : 
//

// #include "stdafx.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;
// XZ position of the camera
float x = 0.0f, z = 5.0f;

float rotationSpeed = 0.04f;

void processSpecialKeys(int key, int xx, int yy) {
    float fraction = 0.1f;

    switch (key) {
        case 27: // [ESC]
        case 88: // X
        case 120: // x
            exit(0);
            break;
        case GLUT_KEY_LEFT:
            angle -= rotationSpeed;
            lx = sin(angle);
            lz = -cos(angle);
            break;
        case GLUT_KEY_RIGHT:
            angle += rotationSpeed;
            lx = sin(angle);
            lz = -cos(angle);
            break;
        case GLUT_KEY_UP:
            x += lx * fraction;
            z += lz * fraction;
            break;
        case GLUT_KEY_DOWN:
            x -= lx * fraction;
            z -= lz * fraction;
            break;
    }
}

void changeSize(int w, int h) {
    if (h == 0)
        h = 1;

    float ratio = w * 1.0f / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);
    gluPerspective(45, ratio, 1, 100);

    glMatrixMode(GL_MODELVIEW);
}

void drawGround() {
    glBegin(GL_QUADS);
        glVertex3f(-100.0f, 0.0f, -100.0f);
        glVertex3f(-100.0f, 0.0f,  100.0f);
        glVertex3f( 100.0f, 0.0f,  100.0f);
        glVertex3f( 100.0f, 0.0f, -100.0f);
    glEnd();
}

void drawBox(float w, float h, float d) {

    float x = w / 2.0f;
    float y = h / 2.0f;
    float z = d / 2.0f;

    glBegin(GL_QUADS);

    // Front
    glVertex3f(-x, -y,  z);
    glVertex3f( x, -y,  z);
    glVertex3f( x,  y,  z);
    glVertex3f(-x,  y,  z);

    // Back
    glVertex3f(-x, -y, -z);
    glVertex3f(-x,  y, -z);
    glVertex3f( x,  y, -z);
    glVertex3f( x, -y, -z);

    // Left
    glVertex3f(-x, -y, -z);
    glVertex3f(-x, -y,  z);
    glVertex3f(-x,  y,  z);
    glVertex3f(-x,  y, -z);

    // Right
    glVertex3f( x, -y, -z);
    glVertex3f( x,  y, -z);
    glVertex3f( x,  y,  z);
    glVertex3f( x, -y,  z);

    // Top
    glVertex3f(-x,  y, -z);
    glVertex3f(-x,  y,  z);
    glVertex3f( x,  y,  z);
    glVertex3f( x,  y, -z);

    // Bottom
    glVertex3f(-x, -y, -z);
    glVertex3f( x, -y, -z);
    glVertex3f( x, -y,  z);
    glVertex3f(-x, -y,  z);

    glEnd();
}

void drawHouse() {
    // House dimensions
    float left   = -1.5f;
    float right  =  1.5f;
    float front  =  1.5f;
    float back   = -1.5f;
    float bottom =  0.0f;
    float top    =  1.7f;

    // Door dimensions (front wall üzerinde boşluk)
    float doorLeft   = -0.3f;
    float doorRight  =  0.3f;
    // float doorBottom =  0.0f;
    float doorTop    =  1.4f;

    // ---------- Walls ----------
    glBegin(GL_QUADS);

    // Left wall
    glColor3f(0.8f, 0.5f, 0.3f);
    glVertex3f(left,  bottom, back);
    glVertex3f(left,  bottom, front);
    glVertex3f(left,  top,    front);
    glVertex3f(left,  top,    back);

    // Right wall
    glColor3f(0.8f, 0.3f, 0.5f);
    glVertex3f(right, bottom, front);
    glVertex3f(right, bottom, back);
    glVertex3f(right, top,    back);
    glVertex3f(right, top,    front);

    // Back wall
    glColor3f(0.5f, 0.8f, 0.3f);
    glVertex3f(left,  bottom, back);
    glVertex3f(right, bottom, back);
    glVertex3f(right, top,    back);
    glVertex3f(left,  top,    back);

    // Front wall - left of door
    glColor3f(0.8f, 0.5f, 0.3f);
    glVertex3f(left,     bottom, front);
    glVertex3f(doorLeft, bottom, front);
    glVertex3f(doorLeft, top,    front);
    glVertex3f(left,     top,    front);

    // Front wall - right of door
    glVertex3f(doorRight, bottom, front);
    glVertex3f(right,     bottom, front);
    glVertex3f(right,     top,    front);
    glVertex3f(doorRight, top,    front);

    // Front wall - above door
    glVertex3f(doorLeft,  doorTop, front);
    glVertex3f(doorRight, doorTop, front);
    glVertex3f(doorRight, top,     front);
    glVertex3f(doorLeft,  top,     front);

    glEnd();

    // ---------- Roof (pyramid) ----------
    float roofPeakX = 0.0f;
    float roofPeakY = top+1.5f;
    float roofPeakZ = 0.0f;

    glColor3f(0.6f, 0.1f, 0.1f);

    glBegin(GL_TRIANGLES);

    // Front roof face
    glVertex3f(left,  top, front);
    glVertex3f(right, top, front);
    glVertex3f(roofPeakX, roofPeakY, roofPeakZ);

    // Right roof face
    glVertex3f(right, top, front);
    glVertex3f(right, top, back);
    glVertex3f(roofPeakX, roofPeakY, roofPeakZ);

    // Back roof face
    glVertex3f(right, top, back);
    glVertex3f(left,  top, back);
    glVertex3f(roofPeakX, roofPeakY, roofPeakZ);

    // Left roof face
    glVertex3f(left,  top, back);
    glVertex3f(left,  top, front);
    glVertex3f(roofPeakX, roofPeakY, roofPeakZ);

    glEnd();

    // floor inside house
    glColor3f(0.5f, 0.35f, 0.2f);
    glBegin(GL_QUADS);
    glVertex3f(left,  0.01f, back);
    glVertex3f(left,  0.01f, front);
    glVertex3f(right, 0.01f, front);
    glVertex3f(right, 0.01f, back);
    glEnd();
}

void drawBall(float x, float y, float z, float radius) {
    glColor3f(1.0f, 0.0f, 0.0f); // kırmızı top

    glPushMatrix();
    glTranslatef(x, y+0.2f, z); // yukarı kaldır (zeminin üstünde olsun)
    glutWireSphere(radius, 32, 32);   // radius, slices, stacks
    glPopMatrix();
}

void drawTeapot(float x, float y, float z, float size) {
    glColor3f(1.0f, 0.0f, 0.0f); // kırmızı Teapot

    glPushMatrix();
    glTranslatef(x, y+0.2f, z); // yukarı kaldır (zeminin üstünde olsun)
    glutWireTeapot(size);   // radius, slices, stacks
    glPopMatrix();
}

void drawTree(float x, float y, float z, float height, float radius) {
    glPushMatrix();

    glTranslatef(x, y, z);

    // Body
    glColor3f(0.55f, 0.27f, 0.07f);
    glPushMatrix();
    glTranslatef(0.0f, height / 2.0f, 0.0f);
    drawBox(radius, height, radius);
    glPopMatrix();

    // Head
    glColor3f(0.0f, 0.6f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, height, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutWireCone(radius + 0.2f, height * 0.8f, 32, 32);
    glPopMatrix();

    glPopMatrix();
}

void renderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(
        x, 1.0f, z,
        x + lx, 1.0f, z + lz,
        0.0f, 1.0f, 0.0f
    );

    // Draw ground
    glColor3f(0.9f, 0.9f, 0.9f);
    drawGround();

    // Draw others
    drawHouse();
    drawBall(0.0f, 0.0f, +3.0f, 0.20);
    drawTeapot(0.0f, 0.0f, 0.0f, 0.15);

    // draw trees
    for (int i=0; i<3; i++) {
        drawTree(4.0f, 0.0f, +3.0f+i*1.1f, 1.5f, 0.2f);
    }

    // Swap buffers
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutCreateWindow("House with Pyramid Roof");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutSpecialFunc(processSpecialKeys);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);

    glutMainLoop();

    return 1;
}
