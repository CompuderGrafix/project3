#include <cmath>
#include <iostream>
#include <stdio.h>

#include "Angel.h"

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = { point4(-0.5, -0.5, 0.5, 1.0), // 0 left bottom front
point4(-0.5, 0.5, 0.5, 1.0), // 1 left top front
point4(0.5, 0.5, 0.5, 1.0), // 2 right top front
point4(0.5, -0.5, 0.5, 1.0), // 3 right bottom front
point4(-0.5, -0.5, -0.5, 1.0), // 4 left bottom back
point4(-0.5, 0.5, -0.5, 1.0), // 5 left top back
point4(0.5, 0.5, -0.5, 1.0), // 6 right top back
point4(0.5, -0.5, -0.5, 1.0)  // 7 right bottom back
};

// RGBA colors
color4 vertex_colors[8] = { color4(0.0, 0.0, 0.0, 1.0),  // 0 black
color4(1.0, 0.0, 0.0, 1.0),  // 1 red
color4(1.0, 1.0, 0.0, 1.0),  // 2 yellow
color4(0.0, 1.0, 0.0, 1.0),  // 3 green
color4(0.0, 0.0, 1.0, 1.0),  // 4 blue
color4(1.0, 0.0, 1.0, 1.0),  // 5 magenta
color4(1.0, 1.0, 1.0, 1.0),  // 6 white
color4(0.0, 1.0, 1.0, 1.0)   // 7 cyan
};

GLuint model_view;  // model-view matrix uniform shader variable location

// Projection transformation parameters

GLfloat fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat aspect;       // Viewport aspect ratio

GLfloat left = -1.0, right = 1.0;
GLfloat bottom = -1.0, top = 1.0;
GLfloat zNear = 0.1, zFar = 3.0;

GLuint projection; // projection matrix uniform shader variable location

int screenWidth, screenHeight;
bool debug = false;

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void face(int a, int b, int c, int d, int color) {
  colors[Index] = vertex_colors[color];
  points[Index] = vertices[a];
  Index++;
  colors[Index] = vertex_colors[color];
  points[Index] = vertices[b];
  Index++;
  colors[Index] = vertex_colors[color];
  points[Index] = vertices[c];
  Index++;
  colors[Index] = vertex_colors[color];
  points[Index] = vertices[a];
  Index++;
  colors[Index] = vertex_colors[color];
  points[Index] = vertices[c];
  Index++;
  colors[Index] = vertex_colors[color];
  points[Index] = vertices[d];
  Index++;
}

void colorcube() {
  face(1, 0, 3, 2, 0); // front black
  face(2, 3, 7, 6, 1); // right red
  face(3, 0, 4, 7, 2); // bottom yellow
  face(6, 5, 1, 2, 3); // top green
  face(4, 5, 6, 7, 4); // back blue
  face(5, 4, 0, 1, 5); // left magenta
}

int PROJECTION = 0;

mat4 getProjectionMatrix() {

  switch (PROJECTION) {
    case 1:
      return Ortho(left, right, bottom, top, zNear, zFar);
    case 2:
      return Frustum(left, right, bottom, top, zNear, zFar);
    default:
      return Perspective(fovy, aspect, zNear, zFar);
  }
}

float xAngle = 0.0;
float yAngle = 0.0;

vec4 translationVector(0.0, 0.0, 0.0, 0.0);
vec4 oldTranslationVector;

void calculateTranslationVector(float depth, float side) {
  // calculate displacement based on current angles (note rotations done in reverse order and negative to move model in opposite direction)
  vec4 calculateDisplacement = RotateY(-yAngle) * RotateX(-xAngle) * vec4(side, 0.0, -depth, 0.0);

  translationVector = oldTranslationVector + calculateDisplacement;
}

void display(void) {
  // clear context variables & draw triangle starting at 0

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  mat4 mv = RotateX(xAngle) * RotateY(yAngle) * Translate(translationVector);
  glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

  mat4 p = getProjectionMatrix();
  glUniformMatrix4fv(projection, 1, GL_TRUE, p);

  glDrawArrays(GL_TRIANGLES, 0, NumVertices);

  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 033: // Escape Key
    case 'q':
    case 'Q':
      exit(EXIT_SUCCESS);
      break;
    case 'x':
      left *= 1.1;
      right *= 1.1;
      break;
    case 'X':
      left *= 0.9;
      right *= 0.9;
      break;
    case 'y':
      bottom *= 1.1;
      top *= 1.1;
      break;
    case 'Y':
      bottom *= 0.9;
      top *= 0.9;
      break;
    case 'z':
      zNear *= 1.1;
      zFar *= 1.1;
      break;
    case 'Z':
      zNear *= 0.9;
      zFar *= 0.9;
      break;
    case 'w': // forward
      oldTranslationVector = translationVector;
      calculateTranslationVector(-0.1, 0.0);
      break;
    case 's': // backward
      oldTranslationVector = translationVector;
      calculateTranslationVector(0.1, 0.0);
      break;
    case 'a': // rotate counter
      oldTranslationVector = translationVector;
      calculateTranslationVector(0.0, 0.1);
      break;
    case 'd': // rotate clock
      oldTranslationVector = translationVector;
      calculateTranslationVector(0.0, -0.1);
      break;
    case 'p':
      PROJECTION = (PROJECTION + 1) % 3;
      break;
    case ' ':  // reset values to their defaults
      PROJECTION = 0;

      left = -1.0;
      right = 1.0;
      bottom = -1.0;
      top = 1.0;
      zNear = 0.5;
      zFar = 3.0;

      xAngle = 0.0;
      yAngle = 0.0;

      translationVector = vec4(0.0, 0.0, 0.0, 0.0);

      break;
  }
}

bool leftDown = false;
bool rightDown = false;

int mouseDownX, mouseDownY;

float oldYAngle;
float oldXAngle;

void mouse(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    switch (button) {
      case GLUT_LEFT_BUTTON:
        oldYAngle = yAngle;
        oldXAngle = xAngle;

        leftDown = true;
        mouseDownX = x;
        mouseDownY = y;
        break;
      case GLUT_MIDDLE_BUTTON:
        break;
      case GLUT_RIGHT_BUTTON:
        oldTranslationVector = translationVector;

        rightDown = true;
        mouseDownX = x;
        mouseDownY = y;
        break;
    }
  } else if (state == GLUT_UP) {
    switch (button) {
      case GLUT_LEFT_BUTTON:
        leftDown = false;
        break;
      case GLUT_MIDDLE_BUTTON:
        break;
      case GLUT_RIGHT_BUTTON:
        rightDown = false;
        break;
    }
  }
}

float rotationFactor = .5;
float translationFactor = 5.0;

void motion(int x, int y) {

  float temp;
  float depth;

  if (leftDown) {
    temp = (x - mouseDownX);
    yAngle = oldYAngle + (temp * rotationFactor);

    temp = (y - mouseDownY);
    xAngle = oldXAngle + (temp * rotationFactor);

    if (debug)
      printf("yAngle = %f, xAngle = %f\n", yAngle, xAngle);

  } else if (rightDown) {
    temp = (y - mouseDownY);
    depth = (temp * translationFactor / screenHeight);

    calculateTranslationVector(depth, 0.0);

    if (debug)
      printf("x = %f, y = %f, z = %f, yAngle = %f, xAngle = %f\n", translationVector.x, translationVector.y, translationVector.z, yAngle, xAngle);
  }
}

void idle(void) {
  glutPostRedisplay();
}

void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  screenWidth = width;
  screenHeight = height;
  aspect = GLfloat(width) / height;
}

void init(void) {

  colorcube();

  // Create a vertex array object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create and initialize a buffer object
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

  // Load shaders and use the resulting shader program
  GLuint program = InitShader("vprojectionshader.glsl", "fcolorshader.glsl");
  glUseProgram(program);

  // set up vertex arrays
  GLuint vPosition = glGetAttribLocation(program, "vPosition");
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

  GLuint vColor = glGetAttribLocation(program, "vColor");
  glEnableVertexAttribArray(vColor);
  glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)) );

  model_view = glGetUniformLocation(program, "model_view");
  projection = glGetUniformLocation(program, "projection");

  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0, 1.0, 1.0, 1.0);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // set rendering context
  glutInitWindowSize(512, 512);
  glutCreateWindow("Fly through"); // title

  glewInit(); // set OpenGL state and initialize shaders
  init();

  glutDisplayFunc(display); // register callback w/Window System
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);

  glutMainLoop();

  return 0;
}
