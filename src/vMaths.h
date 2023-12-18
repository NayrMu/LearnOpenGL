#pragma once

#include <math.h>

struct v4 {
  float x, y, z, w;
};

struct v3 {
  float x, y, z;
};

struct mat4 {
  float m[4][4];
};

struct quaternion {
  float w, x, y, z;
};

struct mat4 makeIdentityMatrix();

void makeLookAtMatrix(struct v3 eye, struct v3 target, struct v3 up, struct mat4* viewMat);

void translate(struct v3 vec, struct mat4* mat);

void multiplyVectorMatrix(struct v4* vec, struct mat4 mat);

void subtractVectors(struct v3 vec1, struct v3 vec2, struct v3* returnVec);

void addVectors(struct v3 vec1, struct v3 vec2, struct v3* returnVec);

void VscalarMulitply(float scalar, struct v3 vec1, struct v3* vec2);

void lengthOfVector(struct v3 vec1, float* result);

void crossProduct(struct v3 vec1, struct v3 vec2, struct v3* returnVec);

void dotProduct(struct v3 vec1, struct v3 vec2, float *result);

void normalize3d(struct v3* vec);

void axisAngleToQuaternion(struct v3* axis, float angle,
                           struct quaternion* result);

void mat4ByMat4(struct mat4 mat1, struct mat4 mat2, struct mat4* mat3);

void quatToMat4(struct quaternion quat, struct mat4* mat);

void quatRot(struct quaternion quat, struct mat4* mat);

void scale(struct v3 vec, struct mat4* mat);

void makeProjection(float fov, float nearPlane, float farPlane,
                    struct mat4* mat, float width, float height);