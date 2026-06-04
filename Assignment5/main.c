// snowman.cpp : 
//

// #include "stdafx.h"
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOUSE_COUNT 6
#define BALL_COUNT 3
#define MAX_CARRIED_BALLS 1
#define HOUSE_HALF_SIZE 1.5f
#define HOUSE_TOP_Y 1.7f
#define LIGHT_HEIGHT 2.6f
#define HOUSE_TEXTURE_SIZE 64
#define MONSTER_COUNT 3
#define MONSTER_TEXTURE_SIZE 64
#define INITIAL_SCORE 4
#define POINTS_PER_DELIVERED_BALL 2
#define MONSTER_SCORE_PENALTY 2
#define MONSTER_HIT_COOLDOWN_MS 1200
#define EMPTY_HOUSE_SCORE_PENALTY 1
#define HOUSE_DOOR_HALF_WIDTH 0.6f
#define WALL_COLLIDER_THICKNESS 0.18f
#define CAMERA_COLLISION_RADIUS 0.2f
#define CHARACTER_MARGIN 0.8f
#define CHARACTER_RADIUS (CAMERA_COLLISION_RADIUS + CHARACTER_MARGIN)
#define BALL_INTERACTION_DISTANCE 0.9f
#define TEAPOT_INTERACTION_DISTANCE 1.0f
#define MONSTER_INTERACTION_DISTANCE 0.9f
#define CONFETTI_COUNT 80
#define MAX_COLLIDERS 32

// !!!!!ÖNEMLİ!!!!!
// streo kontrol
#define LEFT_EYE -1
#define RIGHT_EYE 1
#define STEREO_EYE_SEPARATION 0.18f
#define STEREO_FOCAL_LENGTH 10.0f
#define STEREO_NEAR_PLANE 1.0f
#define STEREO_FAR_PLANE 100.0f
#define STEREO_VERTICAL_FOV_DEGREES 45.0f
/////////////////////////////////////////////////////////

// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx = 0.0f, lz = -1.0f;
// XZ position of the camera
float x = 0.0f, z = 5.0f;

float rotationSpeed = 0.06f;
int isDayMode = 1;
int houseLightsOn[HOUSE_COUNT] = {0, 0, 0, 0};
int windowWidth = 960;
int windowHeight = 480;
int windowedWidth = 960;
int windowedHeight = 480;
int startFullscreen = 0;
int isFullscreen = 0;
int score = INITIAL_SCORE;
int creativeMode = 0;
int gameOver = 0;
int gameWon = 0;
int carriedBallCount = 0;
int carriedBallIds[BALL_COUNT] = {0, 0, 0};
int deliveredBallCount = 0;
int deliveredBallIds[BALL_COUNT] = {0, 0, 0};
int ballAvailable[BALL_COUNT] = {1, 1, 1};
int currentHouseIndex = -1;
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

const int ballHouseIndices[BALL_COUNT] = {1, 2, 3};
const float ballColors[BALL_COUNT][3] = {
    {1.0f, 0.1f, 0.1f},
    {0.1f, 0.9f, 0.1f},
    {0.1f, 0.3f, 1.0f}
};

typedef struct {
    float minX;
    float maxX;
    float minZ;
    float maxZ;
} Collider;

typedef struct {
    float minX;
    float maxX;
    float x;
    float z;
    float speed;
    float direction;
} Monster;

Collider colliders[MAX_COLLIDERS];
int colliderCount = 0;
Monster monsters[MONSTER_COUNT] = {
    {-12.0f, 4.0f, -12.0f, -22.0f, 2.0f,  1.0f},
    {-12.0f, 4.0f,   4.0f, -14.0f, 1.7f, -1.0f},
    {-12.0f, 4.0f,  -4.0f,  -6.0f, 2.3f,  1.0f}
};
int lastMonsterUpdateTime = 0;
int lastMonsterPenaltyTime = -MONSTER_HIT_COOLDOWN_MS;

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

float distanceSquared2D(float ax, float az, float bx, float bz) {
    float dx = ax - bx;
    float dz = az - bz;

    return dx * dx + dz * dz;
}

int getHouseIndexForPosition(float posX, float posZ) {
    int i;

    for (i = 0; i < HOUSE_COUNT; i++) {
        float houseX = housePositions[i][0];
        float houseZ = housePositions[i][1];
        float minX = houseX - HOUSE_HALF_SIZE + CAMERA_COLLISION_RADIUS;
        float maxX = houseX + HOUSE_HALF_SIZE - CAMERA_COLLISION_RADIUS;
        float minZ = houseZ - HOUSE_HALF_SIZE + CAMERA_COLLISION_RADIUS;
        float maxZ = houseZ + HOUSE_HALF_SIZE - CAMERA_COLLISION_RADIUS;

        if (posX >= minX && posX <= maxX &&
            posZ >= minZ && posZ <= maxZ) {
            return i;
        }
    }

    return -1;
}

int getHouseIndexForCamera() {
    return getHouseIndexForPosition(x, z);
}

int houseHasAvailableBall(int houseIndex) {
    int i;

    for (i = 0; i < BALL_COUNT; i++) {
        if (ballHouseIndices[i] == houseIndex && ballAvailable[i]) {
            return 1;
        }
    }

    return 0;
}

void parseCommandLine(int *argc, char **argv) {
    int readIndex;
    int writeIndex = 1;

    for (readIndex = 1; readIndex < *argc; readIndex++) {
        if (strcmp(argv[readIndex], "--creative") == 0) {
            creativeMode = 1;
        } else if (strcmp(argv[readIndex], "--fullscreen") == 0) {
            startFullscreen = 1;
        } else if (strcmp(argv[readIndex], "--size") == 0 && readIndex + 1 < *argc) {
            int width;
            int height;

            if (sscanf(argv[readIndex + 1], "%dx%d", &width, &height) == 2 &&
                width >= 2 && height >= 1) {
                windowedWidth = width;
                windowedHeight = height;
                windowWidth = width;
                windowHeight = height;
            } else {
                fprintf(stderr, "Invalid window size: %s\n", argv[readIndex + 1]);
                exit(1);
            }

            readIndex++;
        } else {
            argv[writeIndex] = argv[readIndex];
            writeIndex++;
        }
    }

    *argc = writeIndex;
}

void toggleFullscreen() {
    if (isFullscreen) {
        glutReshapeWindow(windowedWidth, windowedHeight);
        glutPositionWindow(100, 100);
        isFullscreen = 0;
    } else {
        windowedWidth = windowWidth;
        windowedHeight = windowHeight;
        glutFullScreen();
        isFullscreen = 1;
    }
}

void checkLoseCondition() {
    if (score <= 0 && !creativeMode && !gameWon) {
        gameOver = 1;
    }
}

void changeScore(int delta) {
    score += delta;
    checkLoseCondition();
}

void checkWinCondition() {
    if (deliveredBallCount == BALL_COUNT && score > 0 && !gameOver) {
        gameWon = 1;
    }
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

void updateHouseEntryPenalty() {
    int activeHouse = getHouseIndexForCamera();

    if (activeHouse == currentHouseIndex) {
        return;
    }

    if (activeHouse >= 0 && activeHouse != 0 && !houseHasAvailableBall(activeHouse)) {
        changeScore(-EMPTY_HOUSE_SCORE_PENALTY);
    }

    currentHouseIndex = activeHouse;
}

void updateBallAndTeapotInteractions() {
    int i;

    for (i = 0; i < BALL_COUNT; i++) {
        int houseIndex = ballHouseIndices[i];
        float ballX = housePositions[houseIndex][0];
        float ballZ = housePositions[houseIndex][1];

        if (!ballAvailable[i] || !houseLightsOn[houseIndex] ||
            carriedBallCount >= MAX_CARRIED_BALLS) {
            continue;
        }

        if (distanceSquared2D(x, z, ballX, ballZ) <=
            BALL_INTERACTION_DISTANCE * BALL_INTERACTION_DISTANCE) {
            ballAvailable[i] = 0;
            carriedBallIds[carriedBallCount] = i;
            carriedBallCount++;
        }
    }

    if (carriedBallCount > 0 && deliveredBallCount < BALL_COUNT &&
        distanceSquared2D(x, z, housePositions[0][0], housePositions[0][1]) <=
        TEAPOT_INTERACTION_DISTANCE * TEAPOT_INTERACTION_DISTANCE) {
        deliveredBallIds[deliveredBallCount] = carriedBallIds[0];
        deliveredBallCount++;
        carriedBallCount = 0;
        changeScore(POINTS_PER_DELIVERED_BALL);
        checkWinCondition();
    }
}

void updateMonsterPenalty() {
    int i;
    int now = glutGet(GLUT_ELAPSED_TIME);

    if (now - lastMonsterPenaltyTime < MONSTER_HIT_COOLDOWN_MS) {
        return;
    }

    for (i = 0; i < MONSTER_COUNT; i++) {
        if (distanceSquared2D(x, z, monsters[i].x, monsters[i].z) <=
            MONSTER_INTERACTION_DISTANCE * MONSTER_INTERACTION_DISTANCE) {
            changeScore(-MONSTER_SCORE_PENALTY);
            lastMonsterPenaltyTime = now;
            return;
        }
    }
}

void updateMonsters() {
    int now = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime;
    int i;

    if (lastMonsterUpdateTime == 0) {
        lastMonsterUpdateTime = now;
        return;
    }

    deltaTime = (now - lastMonsterUpdateTime) / 1000.0f;
    lastMonsterUpdateTime = now;

    if (deltaTime > 0.08f) {
        deltaTime = 0.08f;
    }

    for (i = 0; i < MONSTER_COUNT; i++) {
        monsters[i].x += monsters[i].speed * monsters[i].direction * deltaTime;

        if (monsters[i].x > monsters[i].maxX) {
            monsters[i].x = monsters[i].maxX;
            monsters[i].direction = -1.0f;
        } else if (monsters[i].x < monsters[i].minX) {
            monsters[i].x = monsters[i].minX;
            monsters[i].direction = 1.0f;
        }
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

void generateMonsterTexture() {
    unsigned char texture[MONSTER_TEXTURE_SIZE][MONSTER_TEXTURE_SIZE][3];
    int row;
    int col;

    for (row = 0; row < MONSTER_TEXTURE_SIZE; row++) {
        for (col = 0; col < MONSTER_TEXTURE_SIZE; col++) {
            int border = (row < 3 || row > 60 || col < 3 || col > 60);
            int leftEye = ((row - 19) * (row - 19) + (col - 21) * (col - 21)) < 44;
            int rightEye = ((row - 19) * (row - 19) + (col - 43) * (col - 43)) < 44;
            int leftPupil = ((row - 21) * (row - 21) + (col - 23) * (col - 23)) < 12;
            int rightPupil = ((row - 21) * (row - 21) + (col - 41) * (col - 41)) < 12;
            int mouth = (row > 38 && row < 47 && col > 14 && col < 50);
            int tooth = (row >= 47 && row < 55 && col > 16 && col < 48 &&
                ((col - 16) % 10) > (row - 47));
            int scar = (row > 27 && row < 35 && col > 29 && col < 35 &&
                ((row + col) % 3 != 0));
            int spot = ((row * 3 + col * 5) % 23) < 5;
            int stripe = ((row / 7) + (col / 11)) % 2;

            texture[row][col][0] = stripe ? 82 : 54;
            texture[row][col][1] = stripe ? 180 : 134;
            texture[row][col][2] = stripe ? 92 : 116;

            if (spot) {
                texture[row][col][0] = 130;
                texture[row][col][1] = 56;
                texture[row][col][2] = 155;
            }

            if (border) {
                texture[row][col][0] = 24;
                texture[row][col][1] = 60;
                texture[row][col][2] = 46;
            }

            if (leftEye || rightEye) {
                texture[row][col][0] = 250;
                texture[row][col][1] = 240;
                texture[row][col][2] = 92;
            }

            if (leftPupil || rightPupil || mouth || scar) {
                texture[row][col][0] = 18;
                texture[row][col][1] = 20;
                texture[row][col][2] = 24;
            }

            if (tooth) {
                texture[row][col][0] = 245;
                texture[row][col][1] = 245;
                texture[row][col][2] = 230;
            }
        }
    }

    glGenTextures(1, &monsterTexture);
    glBindTexture(GL_TEXTURE_2D, monsterTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, MONSTER_TEXTURE_SIZE, MONSTER_TEXTURE_SIZE,
        0, GL_RGB, GL_UNSIGNED_BYTE, texture
    );
}

void initTextures() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    generateHousePatternTexture();
    generateHouseSignTexture();
    generateMonsterTexture();
    glBindTexture(GL_TEXTURE_2D, 0);
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

    if (key == 'f' || key == 'F') {
        toggleFullscreen();
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

    (void)xx;
    (void)yy;

    if (gameOver || gameWon) {
        return;
    }

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
    if (w < 2)
        w = 2;

    if (h == 0)
        h = 1;

    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
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

void drawDarkHouseInteriorAt(int houseIndex) {
    if (houseLightsOn[houseIndex]) {
        return;
    }

    glPushMatrix();
    glTranslatef(housePositions[houseIndex][0], 0.0f, housePositions[houseIndex][1]);

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glColor4f(0.0f, 0.0f, 0.0f, 0.78f);

    glBegin(GL_QUADS);
    glVertex3f(-1.42f, 0.04f, -1.42f);
    glVertex3f( 1.42f, 0.04f, -1.42f);
    glVertex3f( 1.42f, 0.04f,  1.42f);
    glVertex3f(-1.42f, 0.04f,  1.42f);

    glVertex3f(-1.42f, 1.92f, -1.42f);
    glVertex3f(-1.42f, 0.04f, -1.42f);
    glVertex3f(-1.42f, 0.04f,  1.42f);
    glVertex3f(-1.42f, 1.92f,  1.42f);

    glVertex3f( 1.42f, 1.92f,  1.42f);
    glVertex3f( 1.42f, 0.04f,  1.42f);
    glVertex3f( 1.42f, 0.04f, -1.42f);
    glVertex3f( 1.42f, 1.92f, -1.42f);

    glVertex3f( 1.42f, 1.92f, -1.42f);
    glVertex3f( 1.42f, 0.04f, -1.42f);
    glVertex3f(-1.42f, 0.04f, -1.42f);
    glVertex3f(-1.42f, 1.92f, -1.42f);

    glVertex3f(-1.42f, 1.92f, -1.42f);
    glVertex3f(-1.42f, 1.92f,  1.42f);
    glVertex3f( 1.42f, 1.92f,  1.42f);
    glVertex3f( 1.42f, 1.92f, -1.42f);
    glEnd();

    glDepthMask(GL_TRUE);
    glPopAttrib();
    glPopMatrix();
}

void drawDarkHouseInteriors() {
    int i;

    for (i = 0; i < HOUSE_COUNT; i++) {
        drawDarkHouseInteriorAt(i);
    }
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

void drawGameBalls() {
    int i;

    for (i = 0; i < BALL_COUNT; i++) {
        int houseIndex = ballHouseIndices[i];

        if (ballAvailable[i] && houseLightsOn[houseIndex]) {
            drawBall(
                housePositions[houseIndex][0], 0.02f, housePositions[houseIndex][1],
                0.35f, ballColors[i][0], ballColors[i][1], ballColors[i][2]
            );
        }
    }
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

void drawMonsters() {
    int i;

    for (i = 0; i < MONSTER_COUNT; i++) {
        float yaw = monsters[i].direction > 0.0f ? 90.0f : -90.0f;

        glPushMatrix();
        glTranslatef(monsters[i].x, 0.0f, monsters[i].z);
        glRotatef(yaw, 0.0f, 1.0f, 0.0f);
        drawMonsterBox(0.0f, 0.0f, 0.0f, 0.75f, 1.55f, 0.75f);
        glPopMatrix();
    }
}

void drawBitmapText(float x, float y, const char *text) {
    const char *cursor = text;

    glRasterPos2f(x, y);
    while (*cursor != '\0') {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *cursor);
        cursor++;
    }
}

void drawCircle2D(float centerX, float centerY, float radius, int filled) {
    int i;
    int segmentCount = 32;

    glBegin(filled ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
    if (filled) {
        glVertex2f(centerX, centerY);
    }

    for (i = 0; i <= segmentCount; i++) {
        float angle = (2.0f * 3.14159265f * i) / segmentCount;
        glVertex2f(
            centerX + cos(angle) * radius,
            centerY + sin(angle) * radius
        );
    }
    glEnd();
}

void drawConfettiRain(int viewWidth, int viewHeight) {
    int i;
    int elapsed = glutGet(GLUT_ELAPSED_TIME);

    for (i = 0; i < CONFETTI_COUNT; i++) {
        float seed = (float)(i * 37);
        float speed = 35.0f + (float)((i * 17) % 80);
        float drift = sin((elapsed * 0.002f) + seed) * 16.0f;
        float xPos = fmodf(seed * 23.0f + drift, (float)viewWidth);
        float yPos = fmodf((elapsed * 0.001f * speed) + seed * 11.0f, (float)viewHeight);
        float size = 5.0f + (float)(i % 4);

        if (xPos < 0.0f) {
            xPos += viewWidth;
        }

        switch (i % 5) {
            case 0:
                glColor3f(1.0f, 0.18f, 0.18f);
                break;
            case 1:
                glColor3f(0.15f, 0.8f, 1.0f);
                break;
            case 2:
                glColor3f(1.0f, 0.85f, 0.12f);
                break;
            case 3:
                glColor3f(0.35f, 1.0f, 0.35f);
                break;
            default:
                glColor3f(0.95f, 0.35f, 1.0f);
                break;
        }

        glBegin(GL_QUADS);
        glVertex2f(xPos - size, yPos - size * 0.5f);
        glVertex2f(xPos + size, yPos - size * 0.5f);
        glVertex2f(xPos + size * 0.6f, yPos + size * 0.5f);
        glVertex2f(xPos - size * 0.6f, yPos + size * 0.5f);
        glEnd();
    }
}

void drawScoreUI(int viewWidth, int viewHeight) {
    char scoreText[32];
    int i;
    float circleStartX = 112.0f;
    float circleY = 26.0f;

    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, viewWidth, viewHeight, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    if (gameWon) {
        drawConfettiRain(viewWidth, viewHeight);
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(16.0f, 32.0f, scoreText);

    for (i = 0; i < deliveredBallCount; i++) {
        int ballId = deliveredBallIds[i];
        float centerX = circleStartX + i * 24.0f;

        glColor3f(ballColors[ballId][0], ballColors[ballId][1], ballColors[ballId][2]);
        drawCircle2D(centerX, circleY, 8.0f, 1);

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(2.0f);
        drawCircle2D(centerX, circleY, 8.0f, 0);
    }

    if (creativeMode) {
        glColor3f(0.45f, 0.9f, 1.0f);
        drawBitmapText(16.0f, 56.0f, "Creative mode");
    }

    if (gameWon) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawBitmapText(viewWidth / 2.0f - 86.0f, viewHeight / 2.0f, "CONGRATULATIONS!");
    }

    if (gameOver) {
        glColor3f(1.0f, 0.2f, 0.2f);
        drawBitmapText(viewWidth / 2.0f - 58.0f, viewHeight / 2.0f, "GAME OVER");
    }

    glPopAttrib();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
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

void setupStereoProjection(int eyeSide, int viewportX, int viewportWidth, int viewportHeight) {
    float aspect = viewportWidth * 1.0f / viewportHeight;
    float halfHeight = STEREO_NEAR_PLANE *
        tanf(STEREO_VERTICAL_FOV_DEGREES * 3.14159265f / 360.0f);
    float halfWidth = halfHeight * aspect;
    float frustumShift = -eyeSide * (STEREO_NEAR_PLANE / STEREO_FOCAL_LENGTH) *
        (STEREO_EYE_SEPARATION / 2.0f);

    glViewport(viewportX, 0, viewportWidth, viewportHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(
        -halfWidth + frustumShift, halfWidth + frustumShift,
        -halfHeight, halfHeight,
        STEREO_NEAR_PLANE, STEREO_FAR_PLANE
    );
    glMatrixMode(GL_MODELVIEW);
}

void setupStereoCamera(int eyeSide) {
    float eyeOffset = eyeSide * STEREO_EYE_SEPARATION / 2.0f;
    float rightX = -lz;
    float rightZ = lx;
    float eyeX = x + rightX * eyeOffset;
    float eyeZ = z + rightZ * eyeOffset;

    glLoadIdentity();
    gluLookAt(
        eyeX, 1.0f, eyeZ,
        eyeX + lx, 1.0f, eyeZ + lz,
        0.0f, 1.0f, 0.0f
    );
}

void drawWorld() {
    int i;

    setupHouseLight(0, GL_LIGHT1, 1);
    setupHouseLight(1, GL_LIGHT2, 0);
    setupHouseLight(2, GL_LIGHT3, 0);
    setupHouseLight(3, GL_LIGHT4, 0);
    setupHouseLight(4, GL_LIGHT5, 0);
    setupHouseLight(5, GL_LIGHT6, 0);

    drawGround();

    for (i = 0; i < HOUSE_COUNT; i++) {
        drawHouseAt(housePositions[i][0], housePositions[i][1]);
    }

    drawPatternRectangleOnTeapotHouse();
    drawLeftWallSignOnTeapotHouse();
    drawDarkHouseInteriors();
    drawTeapot(housePositions[0][0], 0.02f, housePositions[0][1], 0.25f);
    drawGameBalls();
    drawMonsters();

    // draw trees
    for (i = 0; i < 3; i++) {
        drawTree(4.0f, 0.0f, +3.0f+i*1.1f, 1.5f, 0.2f);
    }
}

void renderEye(int eyeSide, int viewportX, int viewportWidth, int viewportHeight) {
    setupStereoProjection(eyeSide, viewportX, viewportWidth, viewportHeight);
    setupStereoCamera(eyeSide);
    setupSunLight();
    drawWorld();
    drawScoreUI(viewportWidth, viewportHeight);
}

void renderScene(void) {
    int leftViewWidth = windowWidth / 2;
    int rightViewWidth = windowWidth - leftViewWidth;

    if (!gameOver && !gameWon) {
        updateMonsters();
        updateHouseEntryPenalty();

        if (!gameOver && !gameWon) {
            updateBallAndTeapotInteractions();
            updateMonsterPenalty();
        }
    }

    setupSunLight();
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderEye(LEFT_EYE, 0, leftViewWidth, windowHeight);
    renderEye(RIGHT_EYE, leftViewWidth, rightViewWidth, windowHeight);

    glutSwapBuffers();
}

int main(int argc, char **argv) {
    parseCommandLine(&argc, argv);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowedWidth, windowedHeight);
    glutCreateWindow("Stereoscopic House Game - Left Eye | Right Eye");

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);
    glutSpecialFunc(processSpecialKeys);
    glutKeyboardFunc(processNormalKeys);

    if (startFullscreen) {
        toggleFullscreen();
    }

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
