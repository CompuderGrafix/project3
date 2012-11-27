#include <cmath>
#include <iostream>
#include <stdio.h>

#include "Angel.h"

//  Function which reads, compiles, links shader files and if successful returns a program object
//GLuint InitShader( const char* vertexShaderFile, const char* fragmentShaderFile );

const int NUM_POINTS = 3;
const int NUM_DIMENTIONS = 4;

const float TOP = 0.75f;
const float BOTTOM = -0.75f;

void display(void) {
  // clear context variables & draw triangle starting at 0

  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, NUM_POINTS);
  glutSwapBuffers();
}

void init(void) {

  GLfloat vertexPositions[] = {
      -0.75f, BOTTOM,   0.0f, 1.0f,
      0.0f,   TOP,      0.0f, 1.0f,
      0.75f,  BOTTOM,   0.0f, 1.0f,
  };

  GLuint vao, buffer;
  glGenVertexArrays(1, &vao); // get unused name for vertices
  glBindVertexArray(vao); // create on first call/make active on all subsequent calls

  glGenBuffers(1, &buffer); // get 1 unused name for buffer
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

  GLuint program = InitShader("vsimple.glsl", "fsimple.glsl");
  glUseProgram(program);

  GLuint loc = glGetAttribLocation(program, "vPosition");
  glEnableVertexAttribArray(loc);
  glVertexAttribPointer(loc, NUM_DIMENTIONS, GL_FLOAT, GL_FALSE, 0, 0);

  glClearColor(0.0, 0.0, 0.0, 1.0);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // set rendering context
  glutCreateWindow("Hello World"); // title

  glewInit(); // set OpenGL state and initialize shaders
  init();

  glutDisplayFunc(display); // register callback w/Window System
  glutMainLoop();

  return 0;
}
