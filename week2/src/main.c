#include <GL/glut.h> // (or others, depending on the system in use)

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 600;

void init (void)
{
    glClearColor (1.0, 1.0, 1.0, 0.0); // Set display-window color to white.
    glMatrixMode (GL_PROJECTION); // Set projection parameters.
    gluOrtho2D (0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT);
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
    
    glFlush ( ); // Process all OpenGL routines as quickly as possible.
}

void main (int argc, char** argv)
{
    glutInit (&argc, argv); // Initialize GLUT.
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); // Set display mode.
    glutInitWindowPosition (50, 100); // Set top-left display-window position.
    glutInitWindowSize (SCREEN_WIDTH, SCREEN_HEIGHT); // Set display-window width and height.
    glutCreateWindow ("Assignment 1 ~ M. Furkan UZUN"); // Create display window.
    init ( ); // Execute initialization procedure.
    glutDisplayFunc (screen); // Send graphics to display window.
    glutMainLoop ( ); // Display everything and wait.
}
