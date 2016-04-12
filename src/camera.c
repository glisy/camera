#include <glisy/uniform.h>
#include <glisy/camera.h>
#include <glisy/math.h>

void
UpdatePerspectiveCameraProjectionMatrix(PerspectiveCamera *camera) {
  camera->projection = mat4_perspective(camera->fov,
                                        camera->aspect,
                                        camera->near,
                                        camera->far);
  glisyUniformSet(&camera->uniforms.projection,
                  &camera->projection,
                  sizeof(camera->projection));
  glisyUniformBind(&camera->uniforms.projection, 0);
}

void
UpdatePerspectiveCameraLookAt(PerspectiveCamera *camera) {
  vec3 target = camera->target;
  vec3 position = vec3_transform_mat4(camera->position,
                                      camera->transform);
  camera->view = mat4_lookAt(position, target, camera->up);
  glisyUniformBind(&camera->uniforms.view, 0);
  mat4_identity(camera->transform);
}

void
UpdatePerspectiveCamera(PerspectiveCamera *camera) {
  camera->orientation.direction =
    vec3_normalize(vec3_subtract(camera->position, camera->target));

  camera->orientation.right =
    vec3_normalize(vec3_cross(camera->up, camera->orientation.direction));

  glisyUniformSet(&camera->uniforms.view,
                    &camera->view,
                    sizeof(camera->view));

  UpdatePerspectiveCameraProjectionMatrix(camera);
  UpdatePerspectiveCameraLookAt(camera);
}

void
InitializePerspectiveCamera(PerspectiveCamera *camera, int width, int height) {
  camera->position = vec3(0, 0, 0);
  camera->target = vec3(0, 0, 0);
  camera->up = vec3(0, 1, 0);

  camera->aspect = width / height;
  camera->near = 1.0f;
  camera->far = 1000.0f;
  camera->fov = 45.0f;

  mat4_identity(camera->projection);
  mat4_identity(camera->transform);
  mat4_identity(camera->view);

  glisyUniformInit(&camera->uniforms.projection,
                   "projection",
                   GLISY_UNIFORM_MATRIX, 4);

  glisyUniformInit(&camera->uniforms.view,
                   "view",
                   GLISY_UNIFORM_MATRIX, 4);

  UpdatePerspectiveCamera(camera);
}
