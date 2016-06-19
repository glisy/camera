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

const char *name = "Camera Test";
int keys[BUFSIZ];
int firstMouse = 1;
int height = 600;
int width = 800;

GLFWwindow *window;
GLfloat lastX = 400, lastY = 300;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// model
typedef struct Cube Cube;
struct Cube {
  // glisy
  GlisyGeometry geometry;
  GlisyUniform model;

  // gl
  vec3 position;
  vec3 scale;
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
    vec3(-1.0, -1.0, +1.0),
    vec3(+1.0, -1.0, +1.0),
    vec3(-1.0, +1.0, +1.0),
    vec3(+1.0, +1.0, +1.0),

    vec3(-1.0, -1.0, -1.0),
    vec3(+1.0, -1.0, -1.0),
    vec3(-1.0, +1.0, -1.0),
    vec3(+1.0, +1.0, -1.0),
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
  cube->scale = vec3(1, 1, 1);
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
  model = mat4_translate(model, cube->position);
  model = mat4_scale(model, cube->scale);
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

void
onMouseMove(GLFWwindow *window, double x, double y) {
  GlisyCamera *camera = (GlisyCamera *) glfwGetWindowUserPointer(window);

  if (firstMouse) {
    lastX = x;
    lastY = y;
    firstMouse = 0;
  }

  GLfloat friction = 0.25;
  GLfloat xoffset = x - lastX;
  GLfloat yoffset = lastY - y;  // Reversed since y-coordinates go from bottom to left

  lastX = x;
  lastY = y;

  xoffset *= friction;
  yoffset *= friction;

  camera->rotation.x -= xoffset;
  camera->rotation.y -= yoffset;

  if (camera->rotation.y > 89.0f) {
    camera->rotation.y = 89.0f;
  }

  if (camera->rotation.y < -89.0f) {
    camera->rotation.y = -89.0f;
  }
}

void
onKeyPress(GLFWwindow *window, int key, int scancode, int action, int mods) {
  GLfloat speed = 50.0;
  GLfloat velocity = speed * deltaTime;
  GlisyCamera *camera = (GlisyCamera *) glfwGetWindowUserPointer(window);

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      keys[key] = 1;
    } else if (action == GLFW_RELEASE) {
      keys[key] = 0;
    }
  }

  vec3 position = camera->position;
  vec3 target = position;

  if (keys[GLFW_KEY_W]) {
    target = vec3_add(position, vec3_scale(camera->front, velocity));
  }

  if (keys[GLFW_KEY_S]) {
     target = vec3_subtract(position, vec3_scale(camera->front, velocity));
  }

  if (keys[GLFW_KEY_A]) {
    target = vec3_subtract(position, vec3_scale(camera->right, velocity));
  }

  if (keys[GLFW_KEY_D]) {
    target = vec3_add(position, vec3_scale(camera->right, velocity));
  }

  position = vec3_lerp(position, target, 2.0 * M_PI * deltaTime);
  vec3_copy(camera->position, position);
  glisyCameraUpdate(camera);
}

int
main (void) {
  GlisyCamera camera;
  Cube cube;

  GLFW_SHELL_CONTEXT_INIT(3, 3);
  GLFW_SHELL_WINDOW_INIT(window, width, height);

  glfwSetCursorPosCallback(window, onMouseMove);
  glfwSetKeyCallback(window, onKeyPress);
  glfwSetWindowUserPointer(window, &camera);

  // initialize program
  glisyProgramInit(&program);
  glisyShaderInit(&vertex, GL_VERTEX_SHADER, fs_read("./vertex.glsl"));
  glisyShaderInit(&fragment, GL_FRAGMENT_SHADER, fs_read("./fragment.glsl"));
  glisyProgramAttachShader(&program, &vertex);
  glisyProgramAttachShader(&program, &fragment);
  glisyProgramLink(&program);
  glisyProgramBind(&program);

  // camera
  glisyCameraInitialize(&camera);
  glisyCameraUpdate(&camera);

  // objects
  InitializeCube(&cube);

  // move camera behind cube
  float fov = M_PI / 3.0;
  float zoom = 100.0;

  cube.scale = vec3(0.25, 0.25, 0.25);

  enum {ORTHO, PERSPECTIVE};
  float near = 0.1;
  float far = 1000.0;
  int mode = ORTHO;
  //int mode = PERSPECTIVE;
  camera.position.z = -2.0;

  // start loop
  GLFW_SHELL_RENDER(window, {
    const float time = glfwGetTime();
    const float angle = time * 25.0f;
    const float radians = angle * (M_PI / 180);
    const vec3 rotation = vec3(0, 1, 0);
    float aspect = (float) width / (float) height;
    float delta = (float) height / (float) width;

    deltaTime = time - lastTime;
    lastTime = time;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glisyCameraUpdate(&camera);
    mat4 view = glisyCameraGetViewMatrix(&camera);
    GlisyUniform uProjection;
    GlisyUniform uView;
    mat4 projection;

    switch (mode) {
      case ORTHO:
        if (aspect >= 1.0) {
          projection = mat4_ortho(-1.0 * aspect, delta * aspect,
                                  -1.0, delta,
                                  -near, far);
        } else {
          projection = mat4_ortho(-1.0, delta,
                                  -1.0 / aspect, delta / aspect,
                                  -near, far);
        }
        break;

      case PERSPECTIVE:
        projection = mat4_perspective(fov, aspect, near, far);
        break;

    }

    glisyUniformInit(&uView, "view", GLISY_UNIFORM_MATRIX, 4);
    glisyUniformSet(&uView, &view, sizeof(mat4));
    glisyUniformBind(&uView, 0);

    glisyUniformInit(&uProjection, "projection", GLISY_UNIFORM_MATRIX, 4);
    glisyUniformSet(&uProjection, &projection, sizeof(mat4));
    glisyUniformBind(&uProjection, 0);

    // rotate cube at radians angle in opposite direction
    RotateCube(&cube, radians, vec3_negate(rotation));
    Render(&cube);
  });
  return 0;
}
