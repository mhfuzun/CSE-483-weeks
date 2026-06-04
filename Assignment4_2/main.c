// snowman.cpp : 
//

// #include "stdafx.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define HOUSE_COUNT 6
#define HOUSE_HALF_SIZE 1.5f
#define HOUSE_TOP_Y 1.7f
#define LIGHT_HEIGHT 2.6f
#define HOUSE_TEXTURE_SIZE 64
#define HOUSE_DOOR_HALF_WIDTH 0.6f
#define WALL_COLLIDER_THICKNESS 0.18f
#define CAMERA_COLLISION_RADIUS 0.2f
#define CHARACTER_MARGIN 0.8f
#define CHARACTER_RADIUS (CAMERA_COLLISION_RADIUS + CHARACTER_MARGIN)
#define BALL_COLLIDER_RADIUS 0.22f
#define TEAPOT_COLLIDER_RADIUS 0.2f
#define MAX_COLLIDERS 32

// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;
// XZ position of the camera
float x = 0.0f, z = 5.0f;

float rotationSpeed = 0.06f;
int isDayMode = 1;
int houseLightsOn[HOUSE_COUNT] = {0, 0, 0, 0};
GLuint housePatternTexture = 0;
GLuint houseSignTexture = 0;
GLuint monsterTexture = 0;

const float housePositions[HOUSE_COUNT][2] = {
    {-8.0f, -26.0f},
    {-0.0f, -26.0f},
    {-8.0f, -18.0f},
    { 0.0f, -18.0f},
    {-8.0f, -10.0f},
    { 0.0f, -10.0f}
};

typedef struct {
    float minX;
    float maxX;
    float minZ;
    float maxZ;
} Collider;

Collider colliders[MAX_COLLIDERS];
int colliderCount = 0;

void addCollider(float minX, float maxX, float minZ, float maxZ) {
    if (colliderCount >= MAX_COLLIDERS) {
        return;
    }

    colliders[colliderCount].minX = minX;
    colliders[colliderCount].maxX = maxX;
    colliders[colliderCount].minZ = minZ;
    colliders[colliderCount].maxZ = maxZ;
    colliderCount++;
}

float clampf(float value, float minValue, float maxValue) {
    if (value < minValue) {
        return minValue;
    }

    if (value > maxValue) {
        return maxValue;
    }

    return value;
}

void buildColliderArray() {
    int i;
    colliderCount = 0;

    for (i = 0; i < HOUSE_COUNT; i++) {
        float houseX = housePositions[i][0];
        float houseZ = housePositions[i][1];
        float left = houseX - HOUSE_HALF_SIZE;
        float right = houseX + HOUSE_HALF_SIZE;
        float back = houseZ - HOUSE_HALF_SIZE;
        float front = houseZ + HOUSE_HALF_SIZE;
        float doorLeft = houseX - HOUSE_DOOR_HALF_WIDTH;
        float doorRight = houseX + HOUSE_DOOR_HALF_WIDTH;
        float doorColliderLeft = doorLeft - CHARACTER_RADIUS;
        float doorColliderRight = doorRight + CHARACTER_RADIUS;

        addCollider(
            left - WALL_COLLIDER_THICKNESS, left + WALL_COLLIDER_THICKNESS,
            back, front
        );
        addCollider(
            right - WALL_COLLIDER_THICKNESS, right + WALL_COLLIDER_THICKNESS,
            back, front
        );
        addCollider(
            left, right,
            back - WALL_COLLIDER_THICKNESS, back + WALL_COLLIDER_THICKNESS
        );
        addCollider(
            left, doorColliderLeft,
            front - WALL_COLLIDER_THICKNESS, front + WALL_COLLIDER_THICKNESS
        );
        addCollider(
            doorColliderRight, right,
            front - WALL_COLLIDER_THICKNESS, front + WALL_COLLIDER_THICKNESS
        );
    }

    addCollider(
        housePositions[0][0] - TEAPOT_COLLIDER_RADIUS,
        housePositions[0][0] + TEAPOT_COLLIDER_RADIUS,
        housePositions[0][1] - TEAPOT_COLLIDER_RADIUS,
        housePositions[0][1] + TEAPOT_COLLIDER_RADIUS
    );

    for (i = 1; i < HOUSE_COUNT; i++) {
        addCollider(
            housePositions[i][0] - BALL_COLLIDER_RADIUS,
            housePositions[i][0] + BALL_COLLIDER_RADIUS,
            housePositions[i][1] - BALL_COLLIDER_RADIUS,
            housePositions[i][1] + BALL_COLLIDER_RADIUS
        );
    }
}

int collidesWithScene(float candidateX, float candidateZ) {
    int i;

    for (i = 0; i < colliderCount; i++) {
        float closestX = clampf(candidateX, colliders[i].minX, colliders[i].maxX);
        float closestZ = clampf(candidateZ, colliders[i].minZ, colliders[i].maxZ);
        float deltaX = candidateX - closestX;
        float deltaZ = candidateZ - closestZ;

        if ((deltaX * deltaX) + (deltaZ * deltaZ) <
            (CHARACTER_RADIUS * CHARACTER_RADIUS)) {
            return 1;
        }
    }

    return 0;
}

void tryMoveCamera(float deltaX, float deltaZ) {
    float nextX = x + deltaX;
    float nextZ = z + deltaZ;

    if (!collidesWithScene(nextX, nextZ)) {
        x = nextX;
        z = nextZ;
        return;
    }

    if (!collidesWithScene(nextX, z)) {
        x = nextX;
    }

    if (!collidesWithScene(x, nextZ)) {
        z = nextZ;
    }
}

void setMaterialColor(float r, float g, float b, float shininess) {
    GLfloat specular[] = {0.25f, 0.25f, 0.25f, 1.0f};
    GLfloat emission[] = {0.0f, 0.0f, 0.0f, 1.0f};

    glColor3f(r, g, b);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
}

void setEmissiveMaterial(
    float r, float g, float b,
    float er, float eg, float eb,
    float shininess
) {
    GLfloat specular[] = {0.35f, 0.35f, 0.35f, 1.0f};
    GLfloat emission[] = {er, eg, eb, 1.0f};

    glColor3f(r, g, b);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
}

void clearEmission() {
    GLfloat emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
}

unsigned char *readBMP(const char *filename, int *width, int *height) {
    unsigned char header[54];
    int rawHeight;
    int dataOffset;
    int rowSize;
    int imageSize;
    int y;
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        return NULL;
    }

    if (fread(header, 1, 54, file) != 54 || header[0] != 'B' || header[1] != 'M') {
        fclose(file);
        return NULL;
    }

    dataOffset = *(int *)&header[10];
    *width = *(int *)&header[18];
    rawHeight = *(int *)&header[22];
    *height = rawHeight < 0 ? -rawHeight : rawHeight;
    rowSize = ((*width * 3) + 3) & (~3);
    imageSize = rowSize * (*height);

    {
        unsigned char *rawData = (unsigned char *)malloc(imageSize);
        unsigned char *rgbData = (unsigned char *)malloc((*width) * (*height) * 3);

        if (rawData == NULL || rgbData == NULL) {
            free(rawData);
            free(rgbData);
            fclose(file);
            return NULL;
        }

        fseek(file, dataOffset, SEEK_SET);
        if ((int)fread(rawData, 1, imageSize, file) != imageSize) {
            free(rawData);
            free(rgbData);
            fclose(file);
            return NULL;
        }

        for (y = 0; y < *height; y++) {
            int sourceY = rawHeight > 0 ? (*height - 1 - y) : y;
            int xCoord;

            for (xCoord = 0; xCoord < *width; xCoord++) {
                int srcIndex = sourceY * rowSize + xCoord * 3;
                int dstIndex = (y * (*width) + xCoord) * 3;

                rgbData[dstIndex + 0] = rawData[srcIndex + 2];
                rgbData[dstIndex + 1] = rawData[srcIndex + 1];
                rgbData[dstIndex + 2] = rawData[srcIndex + 0];
            }
        }

        free(rawData);
        fclose(file);
        return rgbData;
    }
}

void generateHousePatternTexture() {
    unsigned char texture[HOUSE_TEXTURE_SIZE][HOUSE_TEXTURE_SIZE][3];
    int row;
    int col;

    for (row = 0; row < HOUSE_TEXTURE_SIZE; row++) {
        for (col = 0; col < HOUSE_TEXTURE_SIZE; col++) {
            int border = (row < 5 || row > 58 || col < 5 || col > 58);
            int diamond = abs(row - 32) + abs(col - 32) < 20;
            int stripe = ((row / 8) + (col / 8)) % 2;

            texture[row][col][0] = border ? 225 : (stripe ? 205 : 170);
            texture[row][col][1] = border ? 190 : (stripe ? 145 : 105);
            texture[row][col][2] = border ? 60 : (stripe ? 70 : 35);

            if (diamond) {
                texture[row][col][0] = 35;
                texture[row][col][1] = 95;
                texture[row][col][2] = 205;
            }

            if ((row % 16 == 0) || (col % 16 == 0)) {
                texture[row][col][0] = 250;
                texture[row][col][1] = 245;
                texture[row][col][2] = 230;
            }
        }
    }

    glGenTextures(1, &housePatternTexture);
    glBindTexture(GL_TEXTURE_2D, housePatternTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, HOUSE_TEXTURE_SIZE, HOUSE_TEXTURE_SIZE,
        0, GL_RGB, GL_UNSIGNED_BYTE, texture
    );
}

void generateHouseSignTexture() {
    unsigned char texture[HOUSE_TEXTURE_SIZE][HOUSE_TEXTURE_SIZE][3];
    int row;
    int col;

    for (row = 0; row < HOUSE_TEXTURE_SIZE; row++) {
        for (col = 0; col < HOUSE_TEXTURE_SIZE; col++) {
            int border = (row < 4 || row > 59 || col < 4 || col > 59);
            int innerFrame = (row < 10 || row > 53 || col < 10 || col > 53);
            int cupBody = (row > 28 && row < 48 && col > 18 && col < 42);
            int cupHandle = (row > 31 && row < 45 && col > 40 && col < 50);
            int steamLeft = (col > 22 && col < 27 && row > 12 && row < 30 &&
                ((row + col) % 7 < 3));
            int steamMid = (col > 30 && col < 35 && row > 10 && row < 28 &&
                ((row + 2 * col) % 7 < 3));
            int steamRight = (col > 38 && col < 43 && row > 12 && row < 30 &&
                ((2 * row + col) % 7 < 3));

            texture[row][col][0] = 48;
            texture[row][col][1] = 86;
            texture[row][col][2] = 120;

            if (innerFrame) {
                texture[row][col][0] = 32;
                texture[row][col][1] = 58;
                texture[row][col][2] = 82;
            }

            if (border) {
                texture[row][col][0] = 205;
                texture[row][col][1] = 165;
                texture[row][col][2] = 92;
            }

            if (cupBody || cupHandle) {
                texture[row][col][0] = 236;
                texture[row][col][1] = 231;
                texture[row][col][2] = 214;
            }

            if (steamLeft || steamMid || steamRight) {
                texture[row][col][0] = 244;
                texture[row][col][1] = 245;
                texture[row][col][2] = 255;
            }

            if (row > 48 && row < 52 && col > 14 && col < 50) {
                texture[row][col][0] = 114;
                texture[row][col][1] = 65;
                texture[row][col][2] = 34;
            }
        }
    }

    glGenTextures(1, &houseSignTexture);
    glBindTexture(GL_TEXTURE_2D, houseSignTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, HOUSE_TEXTURE_SIZE, HOUSE_TEXTURE_SIZE,
        0, GL_RGB, GL_UNSIGNED_BYTE, texture
    );
}

void loadMonsterTexture() {
    int width = 0;
    int height = 0;
    unsigned char *pixels = readBMP("monster.bmp", &width, &height);

    if (pixels == NULL) {
        pixels = readBMP("Assignment4_2/monster.bmp", &width, &height);
    }

    if (pixels == NULL) {
        fprintf(stderr, "Warning: monster.bmp could not be loaded.\n");
        return;
    }

    glGenTextures(1, &monsterTexture);
    glBindTexture(GL_TEXTURE_2D, monsterTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    free(pixels);
}

void initTextures() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    generateHousePatternTexture();
    generateHouseSignTexture();
    loadMonsterTexture();
    glBindTexture(GL_TEXTURE_2D, 0);
}

int getHouseIndexForCamera() {
    for (int i = 0; i < HOUSE_COUNT; i++) {
        float houseX = housePositions[i][0];
        float houseZ = housePositions[i][1];
        float minX = houseX - HOUSE_HALF_SIZE + CHARACTER_RADIUS;
        float maxX = houseX + HOUSE_HALF_SIZE - CHARACTER_RADIUS;
        float minZ = houseZ - HOUSE_HALF_SIZE + CHARACTER_RADIUS;
        float maxZ = houseZ + HOUSE_HALF_SIZE - CHARACTER_RADIUS;

        if (x >= minX && x <= maxX &&
            z >= minZ && z <= maxZ) {
            return i;
        }
    }

    return -1;
}

void processNormalKeys(unsigned char key, int mouseX, int mouseY) {
    (void)mouseX;
    (void)mouseY;

    if (key == 27 || key == 'x' || key == 'X') {
        exit(0);
    }

    if (key == 's' || key == 'S') {
        isDayMode = !isDayMode;
    }

    if (key == '1' || key == '0') {
        int activeHouse = getHouseIndexForCamera();

        if (activeHouse >= 0) {
            houseLightsOn[activeHouse] = (key == '1');
        }
    }

    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) {
    float fraction = 0.3f;

    switch (key) {
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
            tryMoveCamera(lx * fraction, lz * fraction);
            break;
        case GLUT_KEY_DOWN:
            tryMoveCamera(-lx * fraction, -lz * fraction);
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
    setMaterialColor(0.35f, 0.65f, 0.35f, 8.0f);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
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
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-x, -y,  z);
    glVertex3f( x, -y,  z);
    glVertex3f( x,  y,  z);
    glVertex3f(-x,  y,  z);

    // Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-x, -y, -z);
    glVertex3f(-x,  y, -z);
    glVertex3f( x,  y, -z);
    glVertex3f( x, -y, -z);

    // Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-x, -y, -z);
    glVertex3f(-x, -y,  z);
    glVertex3f(-x,  y,  z);
    glVertex3f(-x,  y, -z);

    // Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f( x, -y, -z);
    glVertex3f( x,  y, -z);
    glVertex3f( x,  y,  z);
    glVertex3f( x, -y,  z);

    // Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-x,  y, -z);
    glVertex3f(-x,  y,  z);
    glVertex3f( x,  y,  z);
    glVertex3f( x,  y, -z);

    // Bottom
    glNormal3f(0.0f, -1.0f, 0.0f);
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
    float top    =  2.0f;

    // Door dimensions (front wall üzerinde boşluk)
    float doorLeft   = -0.8f;
    float doorRight  =  0.8f;
    // float doorBottom =  0.0f;
    float doorTop    =  1.8f;

    // ---------- Walls ----------
    setMaterialColor(0.8f, 0.5f, 0.3f, 12.0f);
    glBegin(GL_QUADS);

    // Left wall
    glColor3f(0.8f, 0.5f, 0.3f);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(left,  bottom, back);
    glVertex3f(left,  bottom, front);
    glVertex3f(left,  top,    front);
    glVertex3f(left,  top,    back);

    // Right wall
    glColor3f(0.8f, 0.3f, 0.5f);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(right, bottom, front);
    glVertex3f(right, bottom, back);
    glVertex3f(right, top,    back);
    glVertex3f(right, top,    front);

    // Back wall
    glColor3f(0.5f, 0.8f, 0.3f);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(left,  bottom, back);
    glVertex3f(right, bottom, back);
    glVertex3f(right, top,    back);
    glVertex3f(left,  top,    back);

    // Front wall - left of door
    glColor3f(0.8f, 0.5f, 0.3f);
    glNormal3f(0.0f, 0.0f, 1.0f);
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

    setMaterialColor(0.6f, 0.1f, 0.1f, 24.0f);

    glBegin(GL_TRIANGLES);

    // Front roof face
    glNormal3f(0.0f, 0.707f, 0.707f);
    glVertex3f(left,  top, front);
    glVertex3f(right, top, front);
    glVertex3f(roofPeakX, roofPeakY, roofPeakZ);

    // Right roof face
    glNormal3f(0.707f, 0.707f, 0.0f);
    glVertex3f(right, top, front);
    glVertex3f(right, top, back);
    glVertex3f(roofPeakX, roofPeakY, roofPeakZ);

    // Back roof face
    glNormal3f(0.0f, 0.707f, -0.707f);
    glVertex3f(right, top, back);
    glVertex3f(left,  top, back);
    glVertex3f(roofPeakX, roofPeakY, roofPeakZ);

    // Left roof face
    glNormal3f(-0.707f, 0.707f, 0.0f);
    glVertex3f(left,  top, back);
    glVertex3f(left,  top, front);
    glVertex3f(roofPeakX, roofPeakY, roofPeakZ);

    glEnd();

    // floor inside house
    setMaterialColor(0.5f, 0.35f, 0.2f, 8.0f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(left,  0.01f, back);
    glVertex3f(left,  0.01f, front);
    glVertex3f(right, 0.01f, front);
    glVertex3f(right, 0.01f, back);
    glEnd();
}

void drawHouseAt(float worldX, float worldZ) {
    glPushMatrix();
    glTranslatef(worldX, 0.0f, worldZ);
    drawHouse();
    glPopMatrix();
}

void drawPatternRectangleOnTeapotHouse() {
    glPushMatrix();
    glTranslatef(housePositions[0][0], 0.0f, housePositions[0][1]);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, housePatternTexture);
    setMaterialColor(1.0f, 1.0f, 1.0f, 18.0f);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.45f, -1.51f);
    glTexCoord2f(2.5f, 0.0f); glVertex3f( 1.0f, 0.45f, -1.51f);
    glTexCoord2f(2.5f, 1.5f); glVertex3f( 1.0f, 1.25f, -1.51f);
    glTexCoord2f(0.0f, 1.5f); glVertex3f(-1.0f, 1.25f, -1.51f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawLeftWallSignOnTeapotHouse() {
    glPushMatrix();
    glTranslatef(housePositions[0][0], 0.0f, housePositions[0][1]);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, houseSignTexture);
    setMaterialColor(1.0f, 1.0f, 1.0f, 24.0f);

    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.51f, 0.65f, -0.95f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.51f, 0.65f,  0.95f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.51f, 1.65f,  0.95f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.51f, 1.65f, -0.95f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawBall(float x, float y, float z, float radius, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y + radius, z);
    setMaterialColor(r, g, b, 48.0f);
    glutSolidSphere(radius, 32, 32);
    glPopMatrix();
}

void drawTeapot(float x, float y, float z, float size) {
    glPushMatrix();
    glTranslatef(x, y + size, z);
    setMaterialColor(0.65f, 0.65f, 0.7f, 96.0f);
    {
        GLfloat specular[] = {0.95f, 0.95f, 0.95f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    }
    glutSolidTeapot(size);
    glPopMatrix();
}

void drawTree(float x, float y, float z, float height, float radius) {
    glPushMatrix();

    glTranslatef(x, y, z);

    // Body
    setMaterialColor(0.55f, 0.27f, 0.07f, 6.0f);
    glPushMatrix();
    glTranslatef(0.0f, height / 2.0f, 0.0f);
    drawBox(radius, height, radius);
    glPopMatrix();

    // Head
    setMaterialColor(0.0f, 0.6f, 0.0f, 8.0f);
    glPushMatrix();
    glTranslatef(0.0f, height, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCone(radius + 0.2f, height * 0.8f, 32, 32);
    glPopMatrix();

    glPopMatrix();
}

void drawMonsterBox(float x, float y, float z, float width, float height, float depth) {
    float halfW = width / 2.0f;
    float halfH = height / 2.0f;
    float halfD = depth / 2.0f;

    glPushMatrix();
    glTranslatef(x, y + halfH, z);

    if (monsterTexture != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, monsterTexture);
        setMaterialColor(1.0f, 1.0f, 1.0f, 20.0f);
    } else {
        setMaterialColor(0.7f, 0.2f, 0.2f, 20.0f);
    }

    glBegin(GL_QUADS);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, -halfH,  halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfW, -halfH,  halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfW,  halfH,  halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfW,  halfH,  halfD);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( halfW, -halfH, -halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfW, -halfH, -halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfW,  halfH, -halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( halfW,  halfH, -halfD);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, -halfH, -halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfW, -halfH,  halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfW,  halfH,  halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfW,  halfH, -halfD);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( halfW, -halfH,  halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfW, -halfH, -halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfW,  halfH, -halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( halfW,  halfH,  halfD);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW,  halfH, -halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfW,  halfH,  halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfW,  halfH,  halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( halfW,  halfH, -halfD);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, -halfH,  halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfW, -halfH, -halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfW, -halfH, -halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( halfW, -halfH,  halfD);

    glEnd();

    if (monsterTexture != 0) {
        glDisable(GL_TEXTURE_2D);
    }

    glPopMatrix();
}

void setupSunLight() {
    GLfloat position[] = {-0.4f, 1.0f, -0.3f, 0.0f};
    GLfloat ambientDay[] = {0.25f, 0.25f, 0.2f, 1.0f};
    GLfloat diffuseDay[] = {0.95f, 0.9f, 0.75f, 1.0f};
    GLfloat specularDay[] = {0.75f, 0.7f, 0.6f, 1.0f};
    GLfloat ambientNight[] = {0.03f, 0.03f, 0.08f, 1.0f};
    GLfloat diffuseNight[] = {0.12f, 0.12f, 0.25f, 1.0f};
    GLfloat specularNight[] = {0.1f, 0.1f, 0.2f, 1.0f};
    GLfloat globalAmbientDay[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat globalAmbientNight[] = {0.03f, 0.03f, 0.05f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, isDayMode ? ambientDay : ambientNight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, isDayMode ? diffuseDay : diffuseNight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, isDayMode ? specularDay : specularNight);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,
        isDayMode ? globalAmbientDay : globalAmbientNight);

    if (isDayMode) {
        glClearColor(0.52f, 0.8f, 0.98f, 1.0f);
    } else {
        glClearColor(0.04f, 0.05f, 0.12f, 1.0f);
    }
}

void setupHouseLight(int index, GLenum lightId, int isTeapotHouse) {
    int lightEnabled = houseLightsOn[index];
    GLfloat position[] = {
        housePositions[index][0], LIGHT_HEIGHT, housePositions[index][1], 1.0f
    };
    GLfloat ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat diffuse[] = {0.95f, 0.95f, 0.95f, 1.0f};
    GLfloat specular[] = {0.95f, 0.95f, 0.95f, 1.0f};

    if (isTeapotHouse) {
        diffuse[0] = 0.2f;
        diffuse[1] = 0.35f;
        diffuse[2] = 1.0f;
        specular[0] = 0.2f;
        specular[1] = 0.35f;
        specular[2] = 1.0f;
    }

    glLightfv(lightId, GL_POSITION, position);
    glLightfv(lightId, GL_AMBIENT, ambient);
    glLightfv(lightId, GL_DIFFUSE, diffuse);
    glLightfv(lightId, GL_SPECULAR, specular);
    glLightf(lightId, GL_CONSTANT_ATTENUATION, 0.7f);
    glLightf(lightId, GL_LINEAR_ATTENUATION, 0.15f);
    glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.05f);

    if (lightEnabled) {
        glEnable(lightId);
    } else {
        glDisable(lightId);
    }

    glPushMatrix();
    glTranslatef(position[0], position[1], position[2]);
    if (isTeapotHouse) {
        float blueGlow = lightEnabled ? 0.9f : 0.08f;
        setEmissiveMaterial(0.3f, 0.4f, 1.0f, 0.1f, 0.2f, blueGlow, 32.0f);
    } else {
        float glow = lightEnabled ? 0.8f : 0.05f;
        setEmissiveMaterial(0.9f, 0.9f, 0.9f, glow, glow, glow, 24.0f);
    }
    glutSolidSphere(0.12f, 20, 20);
    clearEmission();
    glPopMatrix();
}

void renderScene(void) {
    setupSunLight();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    gluLookAt(
        x, 1.0f, z,
        x + lx, 1.0f, z + lz,
        0.0f, 1.0f, 0.0f
    );

    setupHouseLight(0, GL_LIGHT1, 1);
    setupHouseLight(1, GL_LIGHT2, 0);
    setupHouseLight(2, GL_LIGHT3, 0);
    setupHouseLight(3, GL_LIGHT4, 0);

    drawGround();

    for (int i = 0; i < HOUSE_COUNT; i++) {
        drawHouseAt(housePositions[i][0], housePositions[i][1]);
    }

    drawPatternRectangleOnTeapotHouse();
    drawLeftWallSignOnTeapotHouse();
    drawTeapot(housePositions[0][0], 0.02f, housePositions[0][1], 0.25f);
    drawBall(housePositions[1][0], 0.02f, housePositions[1][1], 0.35f, 1.0f, 0.1f, 0.1f);
    drawBall(housePositions[2][0], 0.02f, housePositions[2][1], 0.35f, 0.1f, 0.9f, 0.1f);
    drawBall(housePositions[3][0], 0.02f, housePositions[3][1], 0.35f, 0.1f, 0.3f, 1.0f);
    drawMonsterBox(0.0f, 0.0f, -1.0f, 0.2f, 1.2f, 0.2f);

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
    glutKeyboardFunc(processNormalKeys);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
    buildColliderArray();
    initTextures();
    setupSunLight();

    glutMainLoop();

    return 1;
}
