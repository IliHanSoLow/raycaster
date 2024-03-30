#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

float px, py;

void drawPlayer() {
  glColor3f(1, 1, 0);
  glPointSize(8);
  glBegin(GL_POINTS);
  glVertex2f(px, py);
  glEnd();
}

void buttons(GLFWwindow *window) {
  if (glfwGetKey(window, 'w')) {
    py -= 5;
  }
  if (glfwGetKey(window, 'a')) {
    px -= 5;
  }
  if (glfwGetKey(window, 's')) {
    py += 5;
  }
  if (glfwGetKey(window, 'd')) {
    px += 5;
  }
}

void display(GLFWwindow *window) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  buttons(window);
  drawPlayer();
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void init(GLFWwindow *window) {
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glClearColor(0.3, 0.3, 0.3, 0);
  gluOrtho2D(0, 1024, 759, 0);
  px = 300;
  py = 300;
}

int main() {
  glewExperimental = true; // needed for core profile
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE); // not old opengl
  GLFWwindow *window;
  window = glfwCreateWindow(1024, 759, "Tutorial 01", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to open GLFW window.");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }
  // NOTE: Init Window
  init(window);
  do {
    // NOTE: Gameplay Loop
    display(window);
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

  return 0;
}
