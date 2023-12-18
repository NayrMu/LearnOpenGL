#include "randGen.h"

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

