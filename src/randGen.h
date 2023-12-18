#pragma once

unsigned int NoiseGen(int min, int max, unsigned int seed);

void createN3dArray(int n, unsigned int seed, int gameWorld[64][64][64]);

void newSeed(unsigned int* seed);