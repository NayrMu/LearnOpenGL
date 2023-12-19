#include "randGen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


unsigned int NoiseGen(int min, int max, unsigned int seed) {
    int nums[4] = {};
    if (seed > 10000) {
        nums[0] = 13371234;
        nums[1] = 885;
        nums[2] = 123;
        nums[3] = 93823762892357;
    }
    else {
        nums[0] = 13;
        nums[1] = 8587334;
        nums[2] = 123420;
        nums[3] = 420;
    }

    seed /= nums[0];
    seed += nums[1];
    seed *= nums[2];
    seed %= nums[3];


    seed = min + (seed % (max - min + 1));

    return seed;

}

void newSeed(unsigned int* seed) {
    *seed = rand() % 9999999;
}

int p[512];


float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float grad(int hash, float x, float y) {
    int h = hash & 7;            // Use first 4 bits for gradient hash
    float grad = 1.0 + (h & 3); // Gradient value 1-4
    if (h & 4) grad = -grad;     // Randomly invert half of them
    return (grad * x + grad * y); // Gradient times (x, y)
}

float perlin(float x, float y) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);

    int a = p[X] + Y, b = p[X + 1] + Y;

    // And add blended results from 2 corners of the cell (scaled according to the fade curve values)
    return lerp(v, lerp(u, grad(p[a], x, y), grad(p[b], x - 1, y)),
                   lerp(u, grad(p[a + 1], x, y - 1), grad(p[b + 1], x - 1, y - 1)));
}

void initPerlin() {
    for (int i = 0; i < 256; i++) p[256 + i] = p[i] = rand() % 255;
}

void createN3dArray(int n, unsigned int seed, int gameWorld[16][16][16]) {

    initPerlin();
    memset(gameWorld, 0, sizeof(int) * 16 * 16 * 16);

    float rNum;
    int rNumNorm;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            rNum = perlin((float)j * 0.1f + 0.1f, (float)i * 0.1f + 0.1f);
            rNumNorm = (rNum + 1.0f) * 0.5f * 8.0f + 8.0f;
            for (int k = 0; k < n; k++) {
                printf("%d\n", rNumNorm);
                if (k > 5 && k < rNumNorm) {
                    gameWorld[i][j][k] = 2;
                }
                else if (k <= 5) {
                    gameWorld[i][j][k] = 3;
                }
                
            }
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                if ((gameWorld[i][j][k-1] == 2 && gameWorld[i][j][k+1] == 0) || (k == 15 && gameWorld[i][j][k-1] == 2)) {
                    gameWorld[i][j][k] = 1;
                }
                else if (gameWorld[i][j][k+5] == 2) {
                    gameWorld[i][j][k] = 3;
                }
                else if (gameWorld[i][j][k] == 3) {
                    int odds = rand() % 100 + 1;
                    if (odds > 80) {
                        gameWorld[i][j][k] = 4;
                    }
                }
            }
        }
    }
}

