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

void translate(struct v3 vec, struct mat4* mat);

void multiplyVectorMatrix(struct v4* vec, struct mat4 mat);

void normalize3d(struct v3* vec);

void axisAngleToQuaternion(struct v3* axis, float angle,
                           struct quaternion* result);

void mat4ByMat4(struct mat4 mat1, struct mat4* mat2);

void quatToMat4(struct quaternion quat, struct mat4* mat);

void quatRot(struct quaternion quat, struct mat4* mat);

void scale(struct v3 vec, struct mat4* mat);

void makeProjection(float fov, float nearPlane, float farPlane,
                    struct mat4* mat, float width, float height);