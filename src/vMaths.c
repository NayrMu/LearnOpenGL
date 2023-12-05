#include "vMaths.h"

struct mat4 makeIdentityMatrix() {
  struct mat4 mat = {0};
  mat.m[0][0] = 1.0f;
  mat.m[1][1] = 1.0f;
  mat.m[2][2] = 1.0f;
  mat.m[3][3] = 1.0f;

  return mat;
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

void mat4ByMat4(struct mat4 mat1, struct mat4* mat2) {
  struct mat4 result =
      makeIdentityMatrix();  // Create a temporary mat to hold the result

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result.m[i][j] = 0.0;
      for (int k = 0; k < 4; ++k) {
        result.m[i][j] += mat1.m[i][k] * mat2->m[k][j];
      }
    }
  }

  // Copy the result back to mat1
  *mat2 = result;
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

  mat4ByMat4(quatMat, mat);
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