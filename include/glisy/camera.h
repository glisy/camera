#ifndef GLISY_CAMERA_H
#define GLISY_CAMERA_H

#include <glisy/uniform.h>
#include <glisy/math.h>
#include <glisy/gl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PerspectiveCamera PerspectiveCamera;
struct PerspectiveCamera {
  mat4 projection;
  mat4 transform;
  mat4 view;

  vec3 position;
  vec3 target;
  vec3 up;

  struct {
    GlisyUniform projection;
    GlisyUniform view;
  } uniforms;

  struct {
    vec3 direction;
    vec3 right;
    vec3 up;
  } orientation;

  float aspect;
  float near;
  float far;
  float fov;
};

void
InitializePerspectiveCamera(PerspectiveCamera *camera,
                            int width,
                            int height);

void
UpdatePerspectiveCameraProjectionMatrix(PerspectiveCamera *camera);

void
UpdatePerspectiveCameraLookAt(PerspectiveCamera *camera);

void
UpdatePerspectiveCamera(PerspectiveCamera *camera);

#ifdef __cplusplus
}
#endif
#endif
