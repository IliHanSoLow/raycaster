#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLEW_STATIC
#include <GL/glew.h>

#include <cglm/cglm.h>
#include <cglm/mat4.h>

#include <malloc.h>

int main(int argc, const char **argv)
{
    if (!glfwInit())
        return 1;

    glfwDefaultWindowHints();
    GLFWwindow *window = glfwCreateWindow(800, 600, "GLTest", NULL, NULL);
    if (!window)
        return 2;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        glewGetErrorString(error);
        return 3;
    }

    glClearColor(0.2f, 0.3f, 1.0f, 1.0f);

    const int mapW = 8;
    const int mapH = 8;
    const int map[] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1};

    const char *vs = "#version 430 core\nlayout(location = 0) in vec4 Position;\nlayout(location = 1) in vec4 Color;\nlayout(location = 0) out vec4 vColor;\nuniform mat4 uP = mat4(1.0);\nuniform mat4 uM = mat4(1.0);\nvoid main() { gl_Position = uP * uM * Position; vColor = Color; }\n";
    const char *fs = "#version 430 core\nlayout(location = 0) out vec4 Color;\nlayout(location = 0) in vec4 vColor;\nvoid main() { Color = vColor; }\n";

    GLfloat *vertices = malloc(mapW * mapH * 4 * 6 * sizeof(GLfloat)); // 4 vertices a 6 elements per square
    GLuint *indices = malloc(mapW * mapH * 6 * sizeof(GLuint));        // 6 indices per square

    int vi = 0;
    int ii = 0;
    for (int j = 0; j < mapH; ++j)
        for (int i = 0; i < mapW; ++i)
        {
            float col = map[i + j * mapW] ? 1.0f : 0.0f;

            vertices[vi + 0] = i + 0.02f;
            vertices[vi + 1] = j + 0.02f;
            vertices[vi + 2] = col;
            vertices[vi + 3] = col;
            vertices[vi + 4] = col;
            vertices[vi + 5] = 1.0f;

            vertices[vi + 6] = i + 0.02f;
            vertices[vi + 7] = j + 0.98f;
            vertices[vi + 8] = col;
            vertices[vi + 9] = col;
            vertices[vi + 10] = col;
            vertices[vi + 11] = 1.0f;

            vertices[vi + 12] = i + 0.98f;
            vertices[vi + 13] = j + 0.98f;
            vertices[vi + 14] = col;
            vertices[vi + 15] = col;
            vertices[vi + 16] = col;
            vertices[vi + 17] = 1.0f;

            vertices[vi + 18] = i + 0.98f;
            vertices[vi + 19] = j + 0.02f;
            vertices[vi + 20] = col;
            vertices[vi + 21] = col;
            vertices[vi + 22] = col;
            vertices[vi + 23] = 1.0f;

            indices[ii + 0] = vi / 6 + 0;
            indices[ii + 1] = vi / 6 + 1;
            indices[ii + 2] = vi / 6 + 2;
            indices[ii + 3] = vi / 6 + 2;
            indices[ii + 4] = vi / 6 + 3;
            indices[ii + 5] = vi / 6 + 0;

            vi += 24;
            ii += 6;
        }

    GLuint vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));

    glBufferData(GL_ARRAY_BUFFER, mapW * mapH * 96, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mapW * mapH * 24, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    GLuint program = glCreateProgram();
    {
        GLuint shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shader, 1, &vs, NULL);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLsizei length;
            GLchar message[1024];
            glGetShaderInfoLog(shader, 1024, &length, message);
            fprintf(stderr, "%s\n", message);
        }

        glAttachShader(program, shader);
        glDeleteShader(shader);
    }
    {
        GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shader, 1, &fs, NULL);
        glCompileShader(shader);

        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLsizei length;
            GLchar message[1024];
            glGetShaderInfoLog(shader, 1024, &length, message);
            fprintf(stderr, "%s\n", message);
        }

        glAttachShader(program, shader);
        glDeleteShader(shader);
    }
    glLinkProgram(program);
    glValidateProgram(program);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 uP;
        float a = (float)width / height;
        glm_ortho(-a, a, -1.0f, 1.0f, 0.0f, 1.0f, uP);

        mat4 uM;
        vec3 scale;
        scale[0] = scale[1] = 0.1f;
        scale[3] = 1.0f;
        vec3 translation;
        translation[0] = -a;
        translation[1] = -1.0f;
        translation[2] = 0.0f;
        glm_translate_make(uM, translation);
        glm_scale(uM, scale);

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glUseProgram(program);

        glUniformMatrix4fv(glGetUniformLocation(program, "uP"), 1, GL_FALSE, &uP[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program, "uM"), 1, GL_FALSE, &uM[0][0]);

        glDrawElements(GL_TRIANGLES, mapW * mapH * 6, GL_UNSIGNED_INT, NULL);
        glUseProgram(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
