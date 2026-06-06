#pragma once

#include <fhwcg/fhwcg.h>

#define Float2D(x, y) {x,y}
#define Float3D(x, y, z) {x,y,z}

#define Vec2(x, y) (vec2){x,y}
#define Vec2V(v) (vec2){v,v}

#define Vec3(x, y, z) (vec3){x,y,z}
#define Vec3V(v) (vec3){v,v,v}

#define TOGGLE(b) b = !(b)

/**
 * Returns a random float from 0 to 1
 */
#define RAND_01 (rand() / (float) RAND_MAX)

inline void printMatrix(mat4 mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.2f ", mat[i][j]);
        }
        printf("\n");
    }
}

inline void printArr(float *arr, int n) {
    printf("[ ");
    for (int i = 0; i < n; i++) {
        printf("%.2f ", arr[i]);
    }
    printf("]\n");
}
