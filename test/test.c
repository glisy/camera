#include <glfw-shell/glfw-shell.h>
#include <glisy/geometry.h>
#include <glisy/texture.h>
#include <glisy/uniform.h>
#include <glisy/program.h>
#include <glisy/shader.h>
#include <glisy/math.h>
#include <glisy/vao.h>
#include <glisy/gl.h>

#include <SOIL/SOIL.h>
#include <stdlib.h>
#include <stdio.h>
#include <fs/fs.h>

#include <glisy/camera.h>

GlisyUniform uTexture;
GlisyTexture texture;
GlisyProgram program;
GlisyShader fragment;
GlisyShader vertex;

GLFWwindow *window;
const char *name = "Camera Test";
int height = 640;
int width = 640;

// model
typedef struct Cube Cube;
struct Cube {
  // glisy
  GlisyGeometry geometry;
  GlisyUniform model;

  // gl
  vec3 position;
  mat4 transform;
  mat4 rotation;

  // vao
  int faceslen;
};

// forward decl
static void InitializeCube(Cube *cube);
static void UpdateCube(Cube *cube);
static void RotateCube(Cube *cube, float radians, vec3 axis);

// cube constructor
void
InitializeCube(Cube *cube) {
  const vec3 vertices[] = {
    vec3(-0.5, -0.5, +0.5),
    vec3(+0.5, -0.5, +0.5),
    vec3(-0.5, +0.5, +0.5),
    vec3(+0.5, +0.5, +0.5),

    vec3(-0.5, -0.5, -0.5),
    vec3(+0.5, -0.5, -0.5),
    vec3(-0.5, +0.5, -0.5),
    vec3(+0.5, +0.5, -0.5),
  };


  const GLushort faces[] = {
    0, 1, 3, 0, 3, 2,
    1, 5, 7, 1, 7, 3,
    5, 4, 6, 5, 6, 7,
    4, 0, 2, 4, 2, 6,
    4, 5, 1, 4, 1, 0,
    2, 3, 7, 2, 7, 6,
  };

  glisyUniformInit(&cube->model, "model", GLISY_UNIFORM_MATRIX, 4);

  cube->position = vec3(0, 0, 0);
  cube->faceslen = sizeof(faces) / sizeof(GLushort);
  GLuint size = sizeof(vertices);

  GlisyVAOAttribute vPosition;
  memset(&vPosition, 0, sizeof(vPosition));
  vPosition.buffer.data = (void *) vertices;
  vPosition.buffer.type = GL_FLOAT;
  vPosition.buffer.size = size;
  vPosition.buffer.usage = GL_STATIC_DRAW;
  vPosition.buffer.dimension = 3;

  // init matrices
  mat4_identity(cube->transform);
  mat4_identity(cube->rotation);

  // init vao attributes
  glisyGeometryInit(&cube->geometry);
  glisyGeometryAttr(&cube->geometry, "position", &vPosition);
  glisyGeometryFaces(&cube->geometry,
                       GL_UNSIGNED_SHORT,
                       cube->faceslen,
                       (void *) faces);

  // update geometry with attributes and faces
  glisyGeometryUpdate(&cube->geometry);

  UpdateCube(cube);
}

void
UpdateCube(Cube *cube) {
  mat4 model;
  mat4_identity(model);
  model = mat4_multiply(model, cube->rotation);
  glisyUniformSet(&cube->model, &model, sizeof(model));
  glisyUniformBind(&cube->model, 0);
}

void
Render(Cube *cube) {
  UpdateCube(cube);
  glisyGeometryBind(&cube->geometry, 0);
  glisyGeometryDraw(&cube->geometry, GL_TRIANGLES, 0, cube->faceslen);
  glisyGeometryUnbind(&cube->geometry);
}

void
RotateCube(Cube *cube, float radians, vec3 axis) {
  (void) mat4_rotate(cube->rotation, radians, axis);
}

int
main (void) {
  PerspectiveCamera camera;
  Cube cube;

  GLFW_SHELL_CONTEXT_INIT(3, 2);
  GLFW_SHELL_WINDOW_INIT(window, width, height);

  // initialize program
  glisyProgramInit(&program);
  glisyShaderInit(&vertex, GL_VERTEX_SHADER, fs_read("./vertex.glsl"));
  glisyShaderInit(&fragment, GL_FRAGMENT_SHADER, fs_read("./fragment.glsl"));
  glisyProgramAttachShader(&program, &vertex);
  glisyProgramAttachShader(&program, &fragment);
  glisyProgramLink(&program);
  glisyProgramBind(&program);

  InitializePerspectiveCamera(&camera, width, height);
  InitializeCube(&cube);

  // move camera behind cube
  camera.position = vec3(1, 1, 1);

  // start loop
  GLFW_SHELL_RENDER(window, {
    const float time = glfwGetTime();
    const float angle = time * 25.0f;
    const float radians = angle * (M_PI / 180);
    const vec3 rotation = vec3(0, 1, 0);
    (void) mat4_rotate(camera.transform,
                       radians,
                       rotation);

    // handle resize
    camera.aspect = width / height;

    // update camera orientation
    UpdatePerspectiveCamera(&camera);

    // rotate cube at radians angle in opposite direction
    RotateCube(&cube, radians, vec3_negate(rotation));
    Render(&cube);
  });
  return 0;
}
