#pragma once

unsigned int NoiseGen(int min, int max, unsigned int seed);

void createN3dArray(int n, unsigned int seed, int gameWorld[16][16][16]);

void newSeed(unsigned int* seed);