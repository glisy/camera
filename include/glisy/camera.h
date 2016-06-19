#ifndef GLISY_CAMERA_H
#define GLISY_CAMERA_H

#include <glisy/math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GLISY_CAMERA_FIELDS_COMMON \

typedef struct GlisyCamera GlisyCamera;
struct GlisyCamera {
  euler rotation;
  vec3 position;
  vec3 worldUp;
  vec3 front;
  vec3 right;
  vec3 up;
};

void
glisyCameraInitialize(GlisyCamera *camera);

mat4
glisyCameraGetViewMatrix(GlisyCamera *camera);

void
glisyCameraUpdate(GlisyCamera *camera);

#ifdef __cplusplus
}
#endif
#endif
