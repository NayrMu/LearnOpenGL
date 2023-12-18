#include "randGen.h"
#include <stdio.h>
#include <stdlib.h>

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

void createN3dArray(int n, unsigned int seed, int gameWorld[64][64][64]) {

    int rNum;
    char Srnum[9];
    sprintf(Srnum, "%d", rNum);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                rNum = NoiseGen(100000000, 999999999, seed);
                sprintf(Srnum, "%d", rNum);
                int index = rand() % 9;
                int temp = (Srnum[index]) - '0';
                if ( temp > 2) {
                    gameWorld[i][j][k] = 1;
                }
                else { gameWorld[i][j][k] = 0; }
            }
        }
    }
}
