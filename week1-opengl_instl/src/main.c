// (or others, depending on the system in use)
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

int SCREEN_WIDTH    = 800;
int SCREEN_HEIGHT   = 400;

void init (void)
{
    glClearColor (1.0, 1.0, 1.0, 0.0); // Set display-window color to white.
    glMatrixMode (GL_PROJECTION); // Set projection parameters.
    gluOrtho2D (0.0, 200.0, 0.0, 150.0);
}
void lineSegment (void)
{
    glClear (GL_COLOR_BUFFER_BIT); // Clear display window.
    
    glColor3f (0.0, 0.0, 1.0); // Set line segment color to blue.
    glBegin (GL_LINES);
        glVertex2i (180, 15); // Specify line-segment geometry.
        glVertex2i (10, 145);
    glEnd ( );

    // ground
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2i(-SCREEN_WIDTH/2, -SCREEN_HEIGHT/2); // Sol Alt
        glVertex2i(+SCREEN_WIDTH/2, -SCREEN_HEIGHT/2); // Sağ Alt
        glVertex2i(-SCREEN_WIDTH/2, +(int)(SCREEN_HEIGHT*.5f)/2); // Sağ Üst
        glVertex2i(+SCREEN_WIDTH/2, +(int)(SCREEN_HEIGHT*.5f)/2); // Sol Üst
    glEnd();

    // sky
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2i(-SCREEN_WIDTH/2, +(int)(SCREEN_HEIGHT*.5f)/2); // Sol Alt
        glVertex2i(+SCREEN_WIDTH/2, +(int)(SCREEN_HEIGHT*.5f)/2); // Sağ Alt
        glVertex2i(-SCREEN_WIDTH/2, +SCREEN_HEIGHT/2); // Sağ Üst
        glVertex2i(+SCREEN_WIDTH/2, +SCREEN_HEIGHT/2); // Sol Üst
    glEnd();
    
    glFlush ( ); // Process all OpenGL routines as quickly as possible.
}

int main (int argc, char** argv)
{
    glutInit (&argc, argv); // Initialize GLUT.
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); // Set display mode.
    glutInitWindowPosition (50, 100); // Set top-left display-window position.
    glutInitWindowSize (SCREEN_WIDTH, SCREEN_HEIGHT); // Set display-window width and height.
    glutCreateWindow ("HW 1 - Muhammet Furkan UZUN"); // Create display window.
    init ( ); // Execute initialization procedure.
    glutDisplayFunc (lineSegment); // Send graphics to display window.
    glutMainLoop ( ); // Display everything and wait.

    return 0;
}
