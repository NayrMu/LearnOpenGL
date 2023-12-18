#include "vMaths.h"

struct mat4 makeIdentityMatrix() {
  struct mat4 mat = {0};
  mat.m[0][0] = 1.0f;
  mat.m[1][1] = 1.0f;
  mat.m[2][2] = 1.0f;
  mat.m[3][3] = 1.0f;

  return mat;
}

void makeLookAtMatrix(struct v3 eye, struct v3 target, struct v3 up, struct mat4* viewMat) {


  struct mat4 dirMat;

  struct v3 X, Y, Z;

  subtractVectors(eye, target, &Z);
  normalize3d(&Z);
  Y = up;
  crossProduct(Y, Z, &X);
  crossProduct(Z, X, &Y);
  normalize3d(&X);
  normalize3d(&Y);

  float dotX;
  float dotY;
  float dotZ;
  dotProduct(X, eye, &dotX);
  dotProduct(Y, eye, &dotY);
  dotProduct(Z, eye, &dotZ);

  dirMat.m[0][0] = X.x;
  dirMat.m[0][1] = X.y;
  dirMat.m[0][2] = X.z;
  dirMat.m[1][0] = Y.x;
  dirMat.m[1][1] = Y.y;
  dirMat.m[1][2] = Y.z;
  dirMat.m[2][0] = Z.x;
  dirMat.m[2][1] = Z.y;
  dirMat.m[2][2] = Z.z;
  dirMat.m[0][3] = -1.0f * dotX;
  dirMat.m[1][3] = -1.0f * dotY;
  dirMat.m[2][3] = -1.0f * dotZ;
  dirMat.m[3][0] = 0;
  dirMat.m[3][1] = 0;
  dirMat.m[3][2] = 0;
  dirMat.m[3][3] = 1.0f;

  *viewMat = dirMat;
}

void translate(struct v3 vec, struct mat4* mat) {
  mat->m[0][3] += vec.x;
  mat->m[1][3] += vec.y;
  mat->m[2][3] += vec.z;
}

void multiplyVectorMatrix(struct v4* vec, struct mat4 mat) {
  vec->x = mat.m[0][0] * vec->x + mat.m[0][1] * vec->y + mat.m[0][2] * vec->z +
           mat.m[0][3] * vec->w;
  vec->y = mat.m[1][0] * vec->x + mat.m[1][1] * vec->y + mat.m[1][2] * vec->z +
           mat.m[1][3] * vec->w;
  vec->z = mat.m[2][0] * vec->x + mat.m[2][1] * vec->y + mat.m[2][2] * vec->z +
           mat.m[2][3] * vec->w;
  vec->w = mat.m[3][0] * vec->x + mat.m[3][1] * vec->y + mat.m[3][2] * vec->z +
           mat.m[3][3] * vec->w;
}

void subtractVectors(struct v3 vec1, struct v3 vec2, struct v3* returnVec) {
  returnVec->x = vec1.x - vec2.x;
  returnVec->y = vec1.y - vec2.y;
  returnVec->z = vec1.z - vec2.z;
}

void addVectors(struct v3 vec1, struct v3 vec2, struct v3* returnVec) {
  returnVec->x = vec1.x + vec2.x;
  returnVec->y = vec1.y + vec2.y;
  returnVec->z = vec1.z + vec2.z;
}

void VscalarMulitply(float scalar, struct v3 vec1, struct v3* vec2) {
  vec2->x = scalar * vec1.x;
  vec2->y = scalar * vec1.y;
  vec2->z = scalar * vec1.z;
}

void lengthOfVector(struct v3 vec, float* result) {
  *result = sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

void crossProduct(struct v3 vec1, struct v3 vec2, struct v3* returnVec) {
  returnVec->x = (vec1.y * vec2.z) - (vec1.z * vec2.y);
  returnVec->y = (vec1.z * vec2.x) - (vec1.x * vec2.z);
  returnVec->z = (vec1.x * vec2.y) - (vec1.y * vec2.x);
}

void dotProduct(struct v3 vec1, struct v3 vec2, float *result) {
  *result = (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

void normalize3d(struct v3* vec) {
  float dotProduct = vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;
  float magnitude = sqrt(dotProduct);

  if (magnitude != 0.0) {
    vec->x /= magnitude;
    vec->y /= magnitude;
    vec->z /= magnitude;
  }
}

void axisAngleToQuaternion(struct v3* axis, float angle,
                           struct quaternion* result) {
  normalize3d(axis);

  result->w = cos(angle / 2.0);
  float factor = sin(angle / 2.0);
  result->x = factor * axis->x;
  result->y = factor * axis->y;
  result->z = factor * axis->z;
}

void mat4ByMat4(struct mat4 mat1, struct mat4 mat2, struct mat4* mat3) {
  struct mat4 result =
      makeIdentityMatrix();  // Create a temporary mat to hold the result

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result.m[i][j] = 0.0;
      for (int k = 0; k < 4; ++k) {
        result.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
      }
    }
  }

  // Copy the result back to mat1
  *mat3 = result;
}

void quatToMat4(struct quaternion quat, struct mat4* mat) {
  double w = quat.w, x = quat.x, y = quat.y, z = quat.z;

  mat->m[0][0] = 1.0 - 2.0 * (y * y + z * z);
  mat->m[0][1] = 2.0 * (x * y - w * z);
  mat->m[0][2] = 2.0 * (x * z + w * y);
  mat->m[0][3] = 0.0;

  mat->m[1][0] = 2.0 * (x * y + w * z);
  mat->m[1][1] = 1.0 - 2.0 * (x * x + z * z);
  mat->m[1][2] = 2.0 * (y * z - w * x);
  mat->m[1][3] = 0.0;

  mat->m[2][0] = 2.0 * (x * z - w * y);
  mat->m[2][1] = 2.0 * (y * z + w * x);
  mat->m[2][2] = 1.0 - 2.0 * (x * x + y * y);
  mat->m[2][3] = 0.0;

  mat->m[3][0] = 0.0;
  mat->m[3][1] = 0.0;
  mat->m[3][2] = 0.0;
  mat->m[3][3] = 1.0;
}

void quatRot(struct quaternion quat, struct mat4* mat) {
  struct mat4 quatMat;
  quatToMat4(quat, &quatMat);

  mat4ByMat4(quatMat, *mat, mat);
}

void scale(struct v3 vec, struct mat4* mat) {
  mat->m[0][0] *= vec.x;
  mat->m[1][1] *= vec.y;
  mat->m[2][2] *= vec.z;
}

void makeProjection(float fov, float nearPlane, float farPlane,
                    struct mat4* mat, float width, float height) {
  float a = width / height;

  float r = nearPlane * tan(fov / 2);
  float t = r / a;

  mat->m[0][0] = nearPlane / r;
  mat->m[1][1] = nearPlane / t;
  mat->m[2][2] = -1.0f * ((farPlane + nearPlane) / (farPlane - nearPlane));
  mat->m[2][3] = -2.0f * ((farPlane * nearPlane) / (farPlane - nearPlane));
  mat->m[3][2] = -1.0f;
}