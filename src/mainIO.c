#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow *window;

GLuint vbo, ibo, vao, program;

float px, py;

GLuint map_vbo, map_ibo, map_vao, map_program;
GLfloat map_verticees[400];
GLint map_indecies[600];

int map[10][10] = {
    //////////////////////////////////
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, //
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, //
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //
};

void setMapVertsInds() {
  int innner_size = sizeof(map[0]) / sizeof(int);
  int outer_size = sizeof(map) / innner_size / sizeof(int);
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      float a = (j - 10 + 1) / 11.f;        //(2,3) 0.1
      float b = (j - 10 - 0.9f + 1) / 11.f; //      0.14
      float c = (i + 1) / 11.f;             //      0.15
      float d = (i - 0.09f + 1) / 11.f;     //      0.19

      printf("%f,%f\n", a, c);
      printf("%f,%f\n", b, c);
      printf("%f,%f\n", a, d);
      printf("%f,%f\n", b, d);

      map_verticees[i * j + 0] = (float)a; // / 100.0f
      map_verticees[i * j + 1] = (float)c; // / 100.0f
                                           // /
      map_verticees[i * j + 2] = (float)b; // / 100.0f
      map_verticees[i * j + 3] = (float)c; // / 100.0f
                                           // /
      map_verticees[i * j + 4] = (float)a; // / 100.0f
      map_verticees[i * j + 5] = (float)d; // / 100.0f
                                           // /
      map_verticees[i * j + 6] = (float)b; // / 100.0f
      map_verticees[i * j + 7] = (float)d; // / 100.0f

      map_indecies[i * j + 0] = i * j + 0;
      map_indecies[i * j + 1] = i * j + 1;
      map_indecies[i * j + 2] = i * j + 2;
      map_indecies[i * j + 3] = i * j + 1;
      map_indecies[i * j + 4] = i * j + 2;
      map_indecies[i * j + 5] = i * j + 3;
    }
  }
}

void getMapVerts() {
  int innner_size = sizeof(map[0]) / sizeof(int);
  int outer_size = sizeof(map) / innner_size / sizeof(int);
  printf("[\n");
  printf("(");
  for (int i = 0; i < outer_size * innner_size; i++) {
    printf("%f", map_verticees[i]);
    if ((i + 1) % 8 == 0 && i != 0) {
      printf("),\n(");
    } else if ((i + 1) % 2 == 0) {
      printf("), (");
    } else {
      printf("|");
    }
  }
  printf("\n]");
  printf("%d", outer_size * innner_size);
}

void setMapProgram() {
  const char *vsCode = ""
                       "#version 430\n"
                       "layout(location = 0) in vec4 Position;\n"
                       "void main()\n"
                       "{\n"
                       "	gl_Position = Position;\n"
                       "}\n";

  const char *fsCode = ""
                       "#version 430\n"
                       "layout(location = 0) out vec4 Color;\n"
                       "void main()\n"
                       "{\n"
                       "	Color = vec4(1.0, 1.0, 1.0, 1.0);\n"
                       "}\n";

  glGenVertexArrays(1, &map_vao);
  glBindVertexArray(map_vao);

  glGenBuffers(1, &map_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, map_vbo);

  glGenBuffers(1, &map_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map_ibo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(map_verticees), map_verticees,
               GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(map_indecies), map_indecies,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  map_program = glCreateProgram();
  GLuint status;

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vsCode, NULL);
  glCompileShader(vs);
  glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei messageLength;
    GLchar message[256];
    glGetShaderInfoLog(vs, 256, &messageLength, message);
    fprintf(stderr, "[OpenGL] Failed to compile vertex shader: \n%s\n",
            message);
  }

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fsCode, NULL);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei messageLength;
    GLchar message[256];
    glGetShaderInfoLog(fs, 256, &messageLength, message);
    fprintf(stderr, "[OpenGL] Failed to compile fragment shader: \n%s\n",
            message);
  }

  glAttachShader(map_program, vs);
  glAttachShader(map_program, fs);

  glLinkProgram(map_program);
  glGetProgramiv(map_program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei messageLength;
    GLchar message[256];
    glGetProgramInfoLog(fs, 256, &messageLength, message);
    fprintf(stderr, "[OpenGL] Failed to link program: \n%s\n", message);
  }
  glValidateProgram(map_program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei messageLength;
    GLchar message[256];
    glGetProgramInfoLog(fs, 256, &messageLength, message);
    fprintf(stderr, "[OpenGL] Failed to validate program: \n%s\n", message);
  }
  glDeleteShader(fs);
  glDeleteShader(vs);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void onGraphicsInit() {
  GLfloat vertices[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f};
  GLuint indices[] = {0, 1, 2, 2, 3, 0};

  const char *vsCode = ""
                       "#version 430\n"
                       "layout(location = 0) in vec4 Position;\n"
                       "void main()\n"
                       "{\n"
                       "	gl_Position = Position;\n"
                       "}\n";

  const char *fsCode = ""
                       "#version 430\n"
                       "layout(location = 0) out vec4 Color;\n"
                       "void main()\n"
                       "{\n"
                       "	Color = vec4(1.0, 1.0, 1.0, 1.0);\n"
                       "}\n";

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  program = glCreateProgram();

  GLuint status;

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vsCode, NULL);
  glCompileShader(vs);
  glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei messageLength;
    GLchar message[256];
    glGetShaderInfoLog(vs, 256, &messageLength, message);
    fprintf(stderr, "[OpenGL] Failed to compile vertex shader: \n%s\n",
            message);
  }

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fsCode, NULL);
  glCompileShader(fs);
  glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei messageLength;
    GLchar message[256];
    glGetShaderInfoLog(fs, 256, &messageLength, message);
    fprintf(stderr, "[OpenGL] Failed to compile fragment shader: \n%s\n",
            message);
  }

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei messageLength;
    GLchar message[256];
    glGetProgramInfoLog(fs, 256, &messageLength, message);
    fprintf(stderr, "[OpenGL] Failed to link program: \n%s\n", message);
  }
  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
  if (status != GL_TRUE) {
    GLsizei messageLength;
    GLchar message[256];
    glGetProgramInfoLog(fs, 256, &messageLength, message);
    fprintf(stderr, "[OpenGL] Failed to validate program: \n%s\n", message);
  }
  glDeleteShader(fs);
  glDeleteShader(vs);
}

void onGraphicsCleanup() {
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ibo);
  glDeleteVertexArrays(1, &vao);

  glDeleteProgram(program);

  glDeleteBuffers(1, &map_vbo);
  glDeleteBuffers(1, &map_ibo);
  glDeleteVertexArrays(1, &map_vao);
  glDeleteProgram(map_program);
}

/*
 * Runs when the game starts
 */
void onInit() {
  setMapVertsInds();
  px = 100;
  py = 300;
}

/*
 * Runs before every frame
 */
int onFrame() {
  glfwSwapBuffers(window);
  glfwPollEvents();

  return !glfwWindowShouldClose(window);
}

/*
 * Your game loop (drawing, etc.)
 */
void onUpdate() {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw map
  glUseProgram(map_program);
  glBindVertexArray(map_vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map_ibo);
  glDrawElements(GL_TRIANGLES, 600, GL_UNSIGNED_INT, NULL);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);
  glBindVertexArray(0);
}

/*
 * Runs after onUpdate
 */
void onInput() {
  if (glfwGetKey(window, GLFW_KEY_W)) {
    py -= 5;
  }
  if (glfwGetKey(window, GLFW_KEY_A)) {
    px -= 5;
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    py += 5;
  }
  if (glfwGetKey(window, GLFW_KEY_D)) {
    px += 5;
  }
}

/*
 * Your games cleanup
 */
void onExit() {}

void glfwErrorCallback(int error_code, const char *description) {
  fprintf(stderr, "[GLFW 0x%08X] %s\n", error_code, description);
}

void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message,
                     const void *userParam) {
  const char *_source;
  switch (source) {
  case GL_DEBUG_SOURCE_API:
    _source = "API";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    _source = "APPLICATION";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    _source = "OTHER";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    _source = "SHADER_COMPILER";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    _source = "THIRD_PARTY";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    _source = "WINDOW_SYSTEM";
    break;
  default:
    _source = "UNKNOWN";
    break;
  }

  const char *_type;
  switch (type) {
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    _type = "DEPRECATED_BEHAVIOR";
    break;
  case GL_DEBUG_TYPE_ERROR:
    _type = "ERROR";
    break;
  case GL_DEBUG_TYPE_MARKER:
    _type = "MARKER";
    break;
  case GL_DEBUG_TYPE_OTHER:
    _type = "OTHER";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    _type = "PERFORMANCE";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    _type = "POP_GROUP";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    _type = "PORTABILITY";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    _type = "PUSH_GROUP";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    _type = "UNDEFINED_BEHAVIOR";
    break;
  default:
    _type = "UNKNOWN";
    break;
  }

  const char *_severity;
  switch (severity) {
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    _severity = "NOTIFICATION";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    _severity = "LOW";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    _severity = "MEDIUM";
    break;
  case GL_DEBUG_SEVERITY_HIGH:
    _severity = "HIGH";
    break;
  default:
    _severity = "UNKNOWN";
    break;
  }

  fprintf(stderr, "[OpenGL 0x%08X] [%s %s %s] %s\n", id, _source, _type,
          _severity, message);

  if (type == GL_DEBUG_TYPE_ERROR)
    fprintf(stderr, "F\n");
}

int main() {
  /*
   * Setup OpenGL
   */
  glfwSetErrorCallback(glfwErrorCallback);

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  /*
   * Create Window
   */
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(800, 600, "Tutorial 01", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to create window\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSwapInterval(1);

  glfwSetKeyCallback(window, glfwKeyCallback);

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    glfwDestroyWindow(window);
    glfwTerminate();
    return -1;
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(glDebugCallback, NULL);

  /*
   * Game Structure
   */
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  onGraphicsInit();
  onInit();
  while (onFrame()) {

    onUpdate();
    onInput();
  }
  onExit();
  onGraphicsCleanup();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

void drawPlayer() {
  GLfloat vertices[] = {px + 2, py, px, py + 2, px, py - 2};
  GLuint indices[] = {0, 1, 2, 2, 3, 0};

  const char *vsCode = ""
                       "#version 430\n"
                       "layout(location = 0) in vec4 Position;\n"
                       "void main()\n"
                       "{\n"
                       "	gl_Position = Position;\n"
                       "}\n";

  const char *fsCode = ""
                       "#version 430\n"
                       "layout(location = 0) out vec4 Color;\n"
                       "void main()\n"
                       "{\n"
                       "	Color = vec4(1.0, 1.0, 1.0, 1.0);\n"
                       "}\n";

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  program = glCreateProgram();
}
