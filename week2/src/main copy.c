// (or others, depending on the system in use)
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

 #include <stdlib.h>

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 600;

typedef enum {CIRC, RECT, POLY} subObj_t;

typedef struct vertexNode_t {
    int vertex[2];
    struct vertexNode_t* next;
} vertexNode_t;

typedef struct vertexGroup_t {
    int vertexCount;
    vertexNode_t* last;
    vertexNode_t* root;
} vertexGroup_t;

typedef struct subObject_t {
    subObj_t type;

    union
    {
        int circle[3];
        int rect[4];
        vertexGroup_t* poly;
    };

    struct subObject_t* next;

} subObject_t;

typedef struct obj_t {
    int partCount;
    subObject_t* subObjGroup_last;
    subObject_t* subObjGroup_root;
} obj_t;

subObject_t* createCircle(int x,int y,int r) {
    subObject_t* circle = malloc(sizeof(subObject_t));

    if(circle == NULL)
        return NULL;

    circle->type = CIRC;

    circle->circle[0] = x;
    circle->circle[1] = y;
    circle->circle[2] = r;

    circle->next = NULL;

    return circle;
}
subObject_t* createRect(int x1,int y1,int x2,int y2) {
    subObject_t* rect = malloc(sizeof(subObject_t));

    if(rect == NULL)
        return NULL;

    rect->type = RECT;

    rect->circle[0] = x1;
    rect->circle[1] = y1;
    rect->circle[2] = x2;
    rect->circle[3] = y2;

    rect->next = NULL;

    return rect;
}
vertexGroup_t* createVertexGroup() {
    vertexGroup_t* vertexGroup = malloc(sizeof(vertexGroup_t));
    
    if(vertexGroup == NULL)
        return NULL;
    
    vertexGroup->vertexCount = 0;
    vertexGroup->last = NULL;
    vertexGroup->root = NULL;

    return vertexGroup;
}

vertexGroup_t* addVertex_to_VertexGroup(vertexGroup_t* vertexGroup, int x, int y) {
    vertexNode_t* vertexNode = malloc(sizeof(vertexNode_t));

    if(vertexNode == NULL)
        return NULL;

    vertexNode->vertex[0] = x;
    vertexNode->vertex[1] = y;
    vertexNode->next = NULL;

    if (vertexGroup->root == NULL) {
        vertexGroup->vertexCount = 1;
        vertexGroup->last = vertexNode;
        vertexGroup->root = vertexNode;
    } else {
        vertexGroup->vertexCount++;
        vertexGroup->last->next = vertexNode;
        vertexGroup->last = vertexNode;
    }
}

subObject_t* createPolygon(vertexGroup_t* vertexGroup)
{
    subObject_t* poly = malloc(sizeof(subObject_t));

    poly->type = POLY;
    poly->poly = vertexGroup;
    poly->next = NULL;

    return poly;
}

obj_t* addSubObject_to_obj(obj_t* obj, subObject_t* subObj) {
    if (obj == NULL || subObj == NULL) return NULL;

    if (obj->subObjGroup_root == NULL) {
        obj->partCount = 1;
        obj->subObjGroup_last = subObj;
        obj->subObjGroup_root = subObj;
    } else {
        obj->partCount++;
        obj->subObjGroup_last->next = subObj;
        obj->subObjGroup_last = subObj;
    }

    return obj;
}

obj_t tree;
obj_t snowman;
obj_t robot;
obj_t user;

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
