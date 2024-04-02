#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>
#include <nuklear_glfw_gl3.h>

GLFWwindow *window;

struct nk_glfw nkGLFW;
struct nk_context *nkContext;

GLuint vbo, ibo, vao, program;

float px, py;

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
}

/*
 * Runs when the game starts
 */
void onInit() {
  px = 300;
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

  glUseProgram(program);
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
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
  glfwSetErrorCallback(glfwErrorCallback);

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

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

  nkContext = nk_glfw3_init(&nkGLFW, window, NK_GLFW3_INSTALL_CALLBACKS);
  {
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&nkGLFW, &atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_glfw3_font_stash_end(&nkGLFW);
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &droid->handle);*/
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(glDebugCallback, NULL);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  onGraphicsInit();
  onInit();
  while (onFrame()) {
    nk_glfw3_new_frame(&nkGLFW);

    if (nk_begin(nkContext, "Test", nk_rect(50, 50, 230, 250),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                     NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
    }
    nk_end(nkContext);

    onUpdate();
    onInput();

    nk_glfw3_render(&nkGLFW, NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
  }
  onExit();
  onGraphicsCleanup();

  nk_glfw3_shutdown(&nkGLFW);

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
