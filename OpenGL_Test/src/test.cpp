#include <cmath>
#include <iostream>
#include <stdio.h>

#include "Angel.h"

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

const int NumTimesToSubdivide = 6;
const int NumTriangles        = 16384;  // (4 faces)^(NumTimesToSubdivide + 1)
const int NumVertices         = 3 * NumTriangles;

point4 points[NumVertices];
color4 colors[NumVertices];
vec3   normals[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4(-0.5, -0.5, 0.5, 1.0),   // 0 left bottom front
    point4(-0.5, 0.5, 0.5, 1.0),    // 1 left top front
    point4(0.5, 0.5, 0.5, 1.0),     // 2 right top front
    point4(0.5, -0.5, 0.5, 1.0),    // 3 right bottom front
    point4(-0.5, -0.5, -0.5, 1.0),  // 4 left bottom back
    point4(-0.5, 0.5, -0.5, 1.0),   // 5 left top back
    point4(0.5, 0.5, -0.5, 1.0),    // 6 right top back
    point4(0.5, -0.5, -0.5, 1.0)    // 7 right bottom back
};

// RGBA colors
color4 vertex_colors[8] = {
    color4(0.0, 0.0, 0.0, 1.0),  // 0 black
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
GLfloat zNear = 0.1, zFar = 5.0;

GLuint projection; // projection matrix uniform shader variable location

int screenWidth, screenHeight;
bool debug = false;

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
triangle( const point4& a, const point4& b, const point4& c )
{
    vec3  normal = normalize( cross(b - a, c - b) );

  colors[Index] = vertex_colors[6];
  normals[Index] = normal;
  points[Index] = a;
  Index++;

  colors[Index] = vertex_colors[6];
  normals[Index] = normal;
  points[Index] = b;
  Index++;

  colors[Index] = vertex_colors[6];
  normals[Index] = normal;
  points[Index] = c;
  Index++;
}

//----------------------------------------------------------------------------

point4
unit( const point4& p )
{
    float len = p.x*p.x + p.y*p.y + p.z*p.z;

    point4 t;
    if ( len > DivideByZeroTolerance ) {
  t = p / sqrt(len);
  t.w = 1.0;
    }

    return t;
}

void
divide_triangle( const point4& a, const point4& b,
     const point4& c, int count )
{
    if ( count > 0 ) {
        point4 v1 = unit( a + b );
        point4 v2 = unit( a + c );
        point4 v3 = unit( b + c );
        divide_triangle(  a, v1, v2, count - 1 );
        divide_triangle(  c, v2, v3, count - 1 );
        divide_triangle(  b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else {
        triangle( a, b, c );
    }
}

void
tetrahedron( int count )
{
    point4 v[4] = {
  vec4( 0.0, 0.0, 1.0, 1.0 ),
  vec4( 0.0, 0.942809, -0.333333, 1.0 ),
  vec4( -0.816497, -0.471405, -0.333333, 1.0 ),
  vec4( 0.816497, -0.471405, -0.333333, 1.0 )
    };

    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
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

vec4 translationVector(0.0, 0.0, -2.0, 0.0);
vec4 oldTranslationVector;

void calculateTranslationVector(float xDepth, float yDepth, float zDepth) {
  // calculate displacement based on current angles (note rotations done in reverse order and negative to move model in opposite direction)
  vec4 calculateDisplacement = RotateY(-yAngle) * RotateX(-xAngle) * vec4(xDepth, yDepth, -zDepth, 0.0);

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
    case 'w': // move up
      oldTranslationVector = translationVector;
      calculateTranslationVector(0.0, -0.1, 0.0);
      break;
    case 's': // move down
      oldTranslationVector = translationVector;
      calculateTranslationVector(0.0, 0.1, 0.0);
      break;
    case 'a': // move left
      oldTranslationVector = translationVector;
      calculateTranslationVector(0.1, 0.0, 0.0);
      break;
    case 'd': // move right
      oldTranslationVector = translationVector;
      calculateTranslationVector(-0.1, 0.0, 0.0);
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

      translationVector = vec4(0.0, 0.0, -2.0, 0.0);

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

    calculateTranslationVector(0.0, 0.0, depth);

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

  tetrahedron( NumTimesToSubdivide );

  // Create a vertex array object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create and initialize a buffer object
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

  // Load shaders and use the resulting shader program
  GLuint program = InitShader("vprojectionanddynamiclightshader.glsl", "fcolorshader.glsl");
  glUseProgram(program);

  // set up vertex arrays
  GLuint vPosition = glGetAttribLocation(program, "vPosition");
  glEnableVertexAttribArray(vPosition);
  glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

  GLuint vColor = glGetAttribLocation(program, "vColor");
  glEnableVertexAttribArray(vColor);
  glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

  GLuint vNormal = glGetAttribLocation(program, "vNormal");
  glEnableVertexAttribArray(vNormal);
  glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)) );

  // Initialize shader lighting parameters
  point4 light_position( 0.0, 0.0, 2.0, 1.0 );
  color4 light_ambient( 1.0, 1.0, 1.0, 1.0 );
  color4 light_diffuse( 1.0, 0.0, 0.0, 1.0 );
  color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

  point4 light_position2( 0.0, 5.0, 0.0, 1.0 );
  color4 light_diffuse2( 0.0, 0.0, 1.0, 1.0 );
  color4 light_specular2( 1.0, 1.0, 1.0, 1.0 );

  color4 material_ambient( 0.0, 0.0, 0.0, 1.0 );
  //color4 material_diffuse( 0.0, 0.0, 1.0, 1.0 );
  color4 material_specular( 0.0, 0.0, 0.0, 1.0 );
  float  material_shininess = 100.0;

  //color4 ambient_product = light_ambient * material_ambient;
  //color4 diffuse_product = light_diffuse * material_diffuse;
  //color4 specular_product = light_specular * material_specular;

  glUniform4fv(glGetUniformLocation(program, "LightAmbient"), 1, light_ambient);
  glUniform4fv(glGetUniformLocation(program, "LightDiffuse"), 1, light_diffuse);
  glUniform4fv(glGetUniformLocation(program, "LightSpecular"), 1, light_specular);

  glUniform4fv(glGetUniformLocation(program, "LightDiffuse2"), 1, light_diffuse2);
  glUniform4fv(glGetUniformLocation(program, "LightSpecular2"), 1, light_specular2);

  glUniform4fv(glGetUniformLocation(program, "MaterialAmbient"), 1, material_ambient);
  //glUniform4fv(glGetUniformLocation(program, "MaterialDiffuse"), 1, material_diffuse);
  glUniform4fv(glGetUniformLocation(program, "MaterialSpecular"), 1, material_specular);

  glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1, light_position );
  glUniform4fv( glGetUniformLocation(program, "LightPosition2"), 1, light_position2 );

  glUniform1f( glGetUniformLocation(program, "Shininess"), material_shininess );

  model_view = glGetUniformLocation(program, "ModelView");
  projection = glGetUniformLocation(program, "Projection");

  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.1, 0.1, 0.1, 1.0);
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
