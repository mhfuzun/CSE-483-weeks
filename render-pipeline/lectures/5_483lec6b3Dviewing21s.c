// 483lec6b3Dviewing21s

#include "lectures.h"

float planeLeft  (vector4_t p) { return p.x + p.w; } // x >= -w
float planeRight (vector4_t p) { return p.w - p.x; } // x <=  w

float planeBottom(vector4_t p) { return p.y + p.w; } // y >= -w
float planeTop   (vector4_t p) { return p.w - p.y; } // y <=  w

float planeNear  (vector4_t p) { return p.z + p.w; } // z >= -w
float planeFar   (vector4_t p) { return p.w - p.z; } // z <=  w

typedef float (*PlaneFunc)(vector4_t p);

matrix4_t getProjectionMatrix(
    float n, // Znear
    float f, // Zfar
    float projectionAngle,
    float aspectRatio
) {
    float s = 1.0f / tanf(projectionAngle * 0.5f);

    return (matrix4_t) {
        .row = {
            {s / aspectRatio, 0, 0, 0},
            {0, s, 0, 0},
            {0, 0, -(f + n) / (f - n), -(2.0f * f * n) / (f - n)},
            {0, 0, -1, 0}
        }
    };
}

matrix4_t createCameraProjectionMatrix(
    transform_t camera,
    float znear,
    float zfar,
    float projecitonAngle,
    float aspectRatio
) {
    // M = I
    matrix4_t M = loadIdentityMatrix();
    
    // M = R * T * I
    M = matrixTranslate(
        &M,
        -camera.position.x,
        -camera.position.y,
        -camera.position.z
    );
    M = matrixRotateOriginInverse(
        &M,
        camera.rotation.x,
        camera.rotation.y,
        camera.rotation.z
    );

    // M = P * R * T * I
    matrix4_t P = getProjectionMatrix(znear, zfar, projecitonAngle, aspectRatio);
    M = matrix4_mul(&P, &M);
    
    return M;
}

matrix4_t createViewPortMatrix(
    float xmin,
    float ymin,
    float xmax,
    float ymax
) {
    return (matrix4_t) {
        .row = {
            {(xmax - xmin)*0.5f, 0, 0, (xmax+xmin)*0.5f},
            {0, (ymax - ymin)*0.5f, 0, (ymax+ymin)*0.5f},
            {0, 0, 0.5, 0.5},
            {0, 0, 0, 1} 
        }
    };
}

vertex_t lerpVertex(vertex_t a, vertex_t b, float t) {
    vertex_t r;

    r.clip_pos.x = a.clip_pos.x + t * (b.clip_pos.x - a.clip_pos.x);
    r.clip_pos.y = a.clip_pos.y + t * (b.clip_pos.y - a.clip_pos.y);
    r.clip_pos.z = a.clip_pos.z + t * (b.clip_pos.z - a.clip_pos.z);
    r.clip_pos.w = a.clip_pos.w + t * (b.clip_pos.w - a.clip_pos.w);

    return r;
}

// clipping for single plane
int clipPolygonAgainstPlane(
    vertex_t *in,
    int in_count,
    vertex_t *out,
    PlaneFunc plane
) {
    if (in_count == 0)
        return 0;

    int out_count = 0;

    vertex_t prev = in[in_count - 1];
    float fprev = plane(prev.clip_pos);
    int prev_inside = fprev >= 0.0f;

    for (int i = 0; i < in_count; i++) {
        vertex_t curr = in[i];
        float fcurr = plane(curr.clip_pos);
        int curr_inside = fcurr >= 0.0f;

        // A: outside, B: inside
        if (!prev_inside && curr_inside) {
            float t = fprev / (fprev - fcurr);
            out[out_count++] = lerpVertex(prev, curr, t);
            out[out_count++] = curr;
        }
        // A: inside, B: inside
        else if (prev_inside && curr_inside) {
            out[out_count++] = curr;
        }
        // A: inside, B: outside
        else if (prev_inside && !curr_inside) {
            float t = fprev / (fprev - fcurr);
            out[out_count++] = lerpVertex(prev, curr, t);
        }
        // A: outside, B: outside
        else {
            // ikisi de dışarıda: hiçbir şey ekleme
        }

        prev = curr;
        fprev = fcurr;
        prev_inside = curr_inside;
    }

    return out_count;
}

int clipPolygon(
    renderTriangle_t* triangles,
    int base,
    vertex_t* v1,
    vertex_t* v2,
    vertex_t* v3
) {
    vertex_t polyA[16];
    vertex_t polyB[16];

    int count = 3;
    polyA[0] = *v1;
    polyA[1] = *v2;
    polyA[2] = *v3;

    count = clipPolygonAgainstPlane(polyA, count, polyB, planeLeft);
    count = clipPolygonAgainstPlane(polyB, count, polyA, planeRight);
    count = clipPolygonAgainstPlane(polyA, count, polyB, planeBottom);
    count = clipPolygonAgainstPlane(polyB, count, polyA, planeTop);
    count = clipPolygonAgainstPlane(polyA, count, polyB, planeNear);
    count = clipPolygonAgainstPlane(polyB, count, polyA, planeFar);

    int produced = 0;
    for (int i = 1; i < count - 1; i++) {
        triangles[base+produced].p1 = polyA[0];
        triangles[base+produced].p2 = polyA[i];
        triangles[base+produced].p3 = polyA[i + 1];
        produced++;
    }

    return produced;
}
