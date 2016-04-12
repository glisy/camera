#include <glisy/uniform.h>
#include <glisy/camera.h>
#include <glisy/math.h>
#include <string.h>

void
glisyCameraInitialize(GlisyCamera *camera) {
  if (!camera) { return; }
  camera->rotation = euler(90.0f, +0.0f, +0.0f);
  camera->position = vec3(0, 0, 0);
  camera->worldUp = vec3(0, 1, 0);
  camera->front = vec3(0, 0, -1);
}

mat4
glisyCameraGetViewMatrix(GlisyCamera *camera) {
  mat4 view;
  memset(&view, 0, sizeof(mat4));
  if (!camera) { return view; }
  vec3 position = camera->position;
  vec3 target = vec3_add(position, camera->front);
  vec3 up = camera->up;
  view = mat4_lookAt(position, target, up);
  return view;
}

void
glisyCameraUpdate(GlisyCamera *camera) {
  if (!camera) { return; }
  euler rotation = camera->rotation;
  vec3 front = {
    cosf(glisy_radians(rotation.x)) * cosf(glisy_radians(rotation.y)),
    sinf(glisy_radians(rotation.y)),
    sinf(glisy_radians(rotation.x)) * cosf(glisy_radians(rotation.y))
  };
  camera->front = vec3_normalize(front);
  camera->right = vec3_normalize(vec3_cross(camera->front, camera->worldUp));
  camera->up = vec3_normalize(vec3_cross(camera->right, camera->front));
}
