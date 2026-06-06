// Lecture 1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lectures.h"

// Line Drawing
void lineDraw_preCheck(int *x0, int *y0, int *x1, int *y1) {
    if (*x0 > *x1) {
        int tmp = *x0;
        *x0 = *x1;
        *x1 = tmp;

        tmp = *y0;
        *y0 = *y1;
        *y1 = tmp;
    }
}

void lineDraw_LineEquation(putPixel pp, int x0, int y0, int x1, int y1) {
    lineDraw_preCheck(&x0, &y0, &x1, &y1);

    int dx = x1 - x0;
    int dy = y1 - y0;

    float m = dy / dx;
    float b = y0 - m * x0;

    if (fabs(m) <= 1) {
        for (int x = x0; x <= x1; x++) {
            int y = m * x + b;
            pp(x, y);
        }
    } else {
        for (int y = y0; y <= y1; y++) {
            int x = (y - b) / m;
            pp(x, y);
        }
    }
}

void lineDraw_DDA(putPixel pp, int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    if (steps == 0) {
        pp(x0, y0);
        return;
    }

    float x_inc = (float)dx / steps;
    float y_inc = (float)dy / steps;

    float x = x0;
    float y = y0;

    for (int i = 0; i <= steps; i++) {
        pp((int)roundf(x), (int)roundf(y));

        x += x_inc;
        y += y_inc;
    }
}

void lineDraw_Bresenham(putPixel pp, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int x = x0;
    int y = y0;

    if (dx >= dy) {
        /*
            |m| <= 1 durumu

            Ana eksen: x
            Her adımda x ilerler.
            Gerektiğinde y de ilerler.
        */

        int two_dy = dy + dy;
        int two_dx = dx + dx;

        int p = two_dy - dx;
        int two_dy_minus_dx = two_dy - two_dx;

        for (int i = 0; i <= dx; i++) {
            pp(x, y);

            if (p < 0) {
                p += two_dy;
            } else {
                y += sy;
                p += two_dy_minus_dx;
            }

            x += sx;
        }
    } else {
        /*
            |m| > 1 durumu

            Ana eksen: y
            Her adımda y ilerler.
            Gerektiğinde x de ilerler.
        */

        int two_dx = dx + dx;
        int two_dy = dy + dy;

        int p = two_dx - dy;
        int two_dx_minus_dy = two_dx - two_dy;

        for (int i = 0; i <= dy; i++) {
            pp(x, y);

            if (p < 0) {
                p += two_dx;
            } else {
                x += sx;
                p += two_dx_minus_dy;
            }

            y += sy;
        }
    }
}

// Circle Drawing
void circleDraw_PutSymmetrical(putPixel pp, int xc, int yc, int x, int y) {
    pp(xc + x, yc + y);
    pp(xc - x, yc + y);
    pp(xc + x, yc - y);
    pp(xc - x, yc - y);

    pp(xc + y, yc + x);
    pp(xc - y, yc + x);
    pp(xc + y, yc - x);
    pp(xc - y, yc - x);
}

void circleDraw_PythagoreanTheorem(putPixel pp, int xc, int yc, int r) {
    int x = 0;
    int y = r;

    while (x <= y) {
        y = (int)roundf(sqrtf((float)(r * r - x * x)));

        circleDraw_PutSymmetrical(pp, xc, yc, x, y);

        x++;
    }
}

void circleDraw_PolarCoordinates(putPixel pp, int xc, int yc, int r) {
    float t = 0.0f;
    float step = 1.0f / r;

    int x = r;
    int y = 0;

    while (x >= y) {
        x = (int)roundf(r * cosf(t));
        y = (int)roundf(r * sinf(t));

        circleDraw_PutSymmetrical(pp, xc, yc, x, y);

        t += step;
    }
}

void circleDraw_Midpoint(putPixel pp, int xc, int yc, int r) {
    int x = 0;
    int y = r;

    int d = 1 - r;

    while (x <= y) {
        circleDraw_PutSymmetrical(pp, xc, yc, x, y);

        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }

        x++;
    }
}
