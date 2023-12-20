#pragma once
#include "Voxel.h"

unsigned int NoiseGen(int min, int max, unsigned int seed);

float fade(float t);

float lerp(float t, float a, float b);

float grad(int hash, float x, float y);

float perlin(float x, float y, int p[512]);

void initPerlin(int p[512]);

void createN3dArray(int n, unsigned int seed, struct chunk* chunk, int offsetX, int offsetZ, int p[512]);

void newSeed(unsigned int* seed);

