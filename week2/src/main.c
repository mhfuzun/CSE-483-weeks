// (or others, depending on the system in use)
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <stdint.h>

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 600;

uint16_t lfsr = 0xB2E1; // seed

void init (void) {
    glClearColor (1.0, 1.0, 1.0, 0.0); // Set display-window color to white.
    glMatrixMode (GL_PROJECTION); // Set projection parameters.
    gluOrtho2D (0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);
}

// random generator
uint16_t lfsr_next()
{
    unsigned bit;

    bit = ((lfsr >> 0) ^
           (lfsr >> 2) ^
           (lfsr >> 3) ^
           (lfsr >> 5)) & 1;

    lfsr = (lfsr >> 1) | (bit << 15);

    return lfsr;
}

#define random_pos(width) (lfsr_next() % width)

void drawCircle(int cx, int cy, int r) {
    int segments = 100;

    glBegin(GL_POLYGON);

    for(int i=0;i<segments;i++)
    {
        float theta = 2.0f * 3.1415926f * i / segments;

        int x = cx + (int)(r * cos(theta));
        int y = cy + (int)(r * sin(theta));

        glVertex2i(x,y);
    }

    glEnd();
}

void drawStar(int cx, int cy, int R, int r) {
    int points = 5;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2i(cx,cy); // center

    for(int i=0;i<=points*2;i++)
    {
        float angle = (-M_PI/2 + M_PI/5) + i * M_PI / points;

        int radius = (i % 2 == 0) ? R : r;

        int x = cx + radius * cos(angle);
        int y = cy + radius * sin(angle);

        glVertex2i(x,y);
    }

    glEnd();
}

void drawTree(int x, int y, int width, int height)
{
    int trunk_w = width / 4;
    int trunk_h = height / 3;

    /* Body */
    glColor3f(0.55, 0.27, 0.07);  // brown

    glBegin(GL_QUADS);
        glVertex2i(x - trunk_w/2, y);
        glVertex2i(x + trunk_w/2, y);
        glVertex2i(x + trunk_w/2, y + trunk_h);
        glVertex2i(x - trunk_w/2, y + trunk_h);
    glEnd();


    /* Head */
    glColor3f(0.0, 0.7, 0.0);  // green

    glBegin(GL_TRIANGLES);
        glVertex2i(x - width/2, y + trunk_h);
        glVertex2i(x + width/2, y + trunk_h);
        glVertex2i(x, y + height);
    glEnd();
}

void drawTank(int x, int y, int size)
{
    /* Road Wheel */
    glColor3f(0.2,0.2,0.2);
    glRecti(x-60, y, x+60, y+20);

    /* track */
    glColor3f(0.0,0.6,0.0);
    glRecti(x-40, y+20, x+40, y+50);

    /* turret */
    glColor3f(0.0,0.8,0.0);
    drawCircle(x, y+65, 15);

    /* main gun */
    glColor3f(0.3,0.3,0.3);
    glRecti(x+15, y+60, x+70, y+65);
}

void drawChar(int x, int y) {
    // head
    glColor3f(1.0, 0.8, 0.6);
    drawCircle(x, y+120, 20);

    // face
    glColor3f(0,0,0);
    drawCircle(x-8, y+125, 7);
    drawCircle(x+8, y+125, 7);
    drawCircle(x, y+110, 7);
    glColor3f(1.0, 0.8, 0.6);
    drawCircle(x, y+115, 7);

    // body
    glColor3f(0.2,0.4,1.0);

    glBegin(GL_QUADS);
        glVertex2i(x-20, y+60);
        glVertex2i(x+20, y+60);
        glVertex2i(x+20, y+100);
        glVertex2i(x-20, y+100);
    glEnd();

    // legs
    glColor3f(0.1,0.1,0.4);

    glBegin(GL_QUADS);
        glVertex2i(x-20, y+30);
        glVertex2i(x+20, y+30);
        glVertex2i(x+20, y+60);
        glVertex2i(x-20, y+60);
    glEnd();

    glColor3f(1.0,0.8,0.6);

    glBegin(GL_QUADS);
        glVertex2i(x-35, y+70);
        glVertex2i(x-20, y+70);
        glVertex2i(x-20, y+90);
        glVertex2i(x-35, y+90);
    glEnd();


    // arm
    glBegin(GL_QUADS);
        glVertex2i(x+20, y+70);
        glVertex2i(x+35, y+70);
        glVertex2i(x+35, y+90);
        glVertex2i(x+20, y+90);
    glEnd();

    glColor3f(0.1,0.1,0.4);

    glBegin(GL_QUADS);
        glVertex2i(x-18, y);
        glVertex2i(x-5, y);
        glVertex2i(x-5, y+30);
        glVertex2i(x-18, y+30);
    glEnd();

    glBegin(GL_QUADS);
        glVertex2i(x+5, y);
        glVertex2i(x+18, y);
        glVertex2i(x+18, y+30);
        glVertex2i(x+5, y+30);
    glEnd();
}

void screen (void)
{
    glClear (GL_COLOR_BUFFER_BIT); // Clear display window.
    
    // ground
    glColor3f (1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(SCREEN_WIDTH, 0);
        glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT*0.60f);
        glVertex2i(0, SCREEN_HEIGHT*0.60f);
    glEnd();

    // sky
    glColor3f (0.8, 0.8, 0.8);
    glBegin(GL_QUADS);
        glVertex2i(0, SCREEN_HEIGHT*0.60f);
        glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT*0.60f);
        glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
        glVertex2i(0, SCREEN_HEIGHT);
    glEnd();

    // snow
    for (int i=0; i<25; i++) {
        int x = random_pos(SCREEN_WIDTH);
        int y = random_pos(SCREEN_HEIGHT*0.40f) + SCREEN_HEIGHT*0.60f;
        glColor3f(1,1,1);
        glRecti(x, y, x+10, y+10);
    }

    // snowman
    int snowman_ypos=340;
    for (int i=0; i<3; i++) {
        glColor3f (0.9, 0.9, 1);
        drawCircle( 100+i*50, snowman_ypos, 25 );
        drawCircle( 100+i*50, snowman_ypos+37, 12 );
    }

    // star
    glColor3f (0, 0, 0);
    drawStar(350,300,80,36);
    glColor3f (1, 1, 0);
    drawStar(350,300,74,30);

    // forest
    for (int i=0; i<3; i++) {
        drawTree(700+i*120, 300, 100, 200);
    }

    // enemy
    drawTank(100,110,300);

    // player
    drawChar(SCREEN_WIDTH*.9, 110);

    // ball
    glColor3f (1, 0, 0);
    drawCircle( 100+75, 110+60, 15 );

    //////////////////// END ///////////////////////

    glFlush ( ); // Process all OpenGL routines as quickly as possible.
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27: // ESC
            exit(0);
            break;
    }
}

int main (int argc, char** argv)
{
    glutInit (&argc, argv); // Initialize GLUT.
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); // Set display mode.
    glutInitWindowPosition (50, 100); // Set top-left display-window position.
    glutInitWindowSize (SCREEN_WIDTH, SCREEN_HEIGHT); // Set display-window width and height.
    glutCreateWindow ("Assignment 1 ~ M. Furkan UZUN"); // Create display window.
    
    init ( ); // Execute initialization procedure.
    
    glutDisplayFunc (screen); // Send graphics to display window.
    glutKeyboardFunc(keyboard);

    glutMainLoop ( ); // Display everything and wait.

    // never reach there
    return 0;
}
