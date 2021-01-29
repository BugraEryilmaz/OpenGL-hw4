#include "helper.h"
#define _USE_MATH_DEFINES
#include "linmath.h"

static GLFWwindow* win = NULL;
int widthWindow = 1000, heightWindow = 1000;

int vertexPosSize;
int vertexIDSize;

vector<GLfloat> vertex_data;
vector<GLuint> vertex_ids;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idHeightTexture;
GLuint idMVPMatrix;

// Buffers
GLuint idVertexBuffer;
GLuint idIndexBuffer;

int textureWidth, textureHeight;
float heightFactor = 10;

vec3 cameraPos = {textureWidth/2.f, textureWidth/10.f, -textureWidth/4.f};
vec3 cameraUp = {0, 1, 0};
vec3 cameraGaze = {0, 0, 1};
float cameraSpeed = 0;

mat4x4 Model;
mat4x4 View;
mat4x4 Projection;
mat4x4 MV;
mat4x4 MVP;

vec3 lightPos;
GLuint depthMapFBO;
GLuint depthCubemap;
bool lightPosFlag = false;
bool isFullScreen = false;

GLdouble fov = 45;
GLdouble aspectRatio = 1;
GLdouble nearDistance = 0.1;
GLdouble farDistance = 1000;

void initialSetup() {
  cameraPos[0] = textureWidth/2.f;
  cameraPos[1] = textureWidth/10.f;
  cameraPos[2] = -textureWidth/4.f;
  cameraUp[0] = 0;
  cameraUp[1] = 1;
  cameraUp[2] = 0;
  cameraGaze[0] = 0;
  cameraGaze[1] = 0;
  cameraGaze[2] = 1;
  cameraSpeed = 0;

  mat4x4_identity(Model);
  mat4x4_identity(View);
  mat4x4_identity(Projection);

}

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      cameraSpeed = 0;
    }
    if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      cameraSpeed += 0.01;
    }
    if (key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      cameraSpeed -= 0.01;
    }
    if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      initialSetup();
    }
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      lightPos[2] += 5;
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      lightPos[2] -= 5;
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      lightPos[0] += 5;
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      lightPos[0] -= 5;
    }
    if (key == GLFW_KEY_P && (action == GLFW_PRESS)) {
      glfwDestroyWindow(win);
      if (!isFullScreen)
        win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", glfwGetPrimaryMonitor(), nullptr);
      else
        win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", nullptr, nullptr);
      glfwMakeContextCurrent(win);
      glfwSetKeyCallback(win, keyCallback);
      isFullScreen = !isFullScreen;
    }
}


void setVBOs() {
	glEnableClientState(GL_VERTEX_ARRAY);

  int index = 0;

  for (int x=0 ;x<textureWidth;x++) {
    for (int z=0 ;z<textureHeight;z++) {
      vertex_data.push_back((GLfloat) x);
      vertex_data.push_back(0);
      vertex_data.push_back(z);

      vertex_data.push_back(x+1.f);
      vertex_data.push_back(0);
      vertex_data.push_back(z);

      vertex_data.push_back(x+1.f);
      vertex_data.push_back(0);
      vertex_data.push_back(z+1);

      vertex_data.push_back((GLfloat) x);
      vertex_data.push_back(0);
      vertex_data.push_back(z+1);

      vertex_ids.push_back(index+0);
      vertex_ids.push_back(index+1);
      vertex_ids.push_back(index+2);

      vertex_ids.push_back(index+2);
      vertex_ids.push_back(index+3);
      vertex_ids.push_back(index+0);

      index += 4;
    }
  }

	GLfloat* vertexPos = vertex_data.data();  //vertices with xyz

	GLuint* indices = vertex_ids.data();  //triangle vertex ids
		

	GLuint vertexAttribBuffer, indexBuffer;

	glGenBuffers(1, &vertexAttribBuffer);
	glGenBuffers(1, &indexBuffer);

	assert(vertexAttribBuffer > 0 && indexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

  vertexPosSize = sizeof(GLfloat) * vertex_data.size();
  vertexIDSize = sizeof(GLuint) * vertex_ids.size();
		
	glBufferData(GL_ARRAY_BUFFER, vertexPosSize, vertexPos, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIDSize, indices, GL_STATIC_DRAW);
}

void drawMesh() {
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	glDrawElements(GL_TRIANGLES, vertexIDSize, GL_UNSIGNED_INT, (void*)0);
}

void setMVP() {
  mat4x4_perspective(Projection, fov, aspectRatio, nearDistance, farDistance);
  vec3 lookat;
  vec3_add(lookat, cameraGaze, cameraPos);
  mat4x4_look_at(View, cameraPos, lookat, cameraUp);
  mat4x4_mul(MV, View, Model);
  mat4x4_mul(MVP, Projection, MV);

  GLint MVPLoc = glGetUniformLocation(idProgramShader, "MVP");
  glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, (const GLfloat*) MVP);
  GLint MVLoc = glGetUniformLocation(idProgramShader, "MV");
  glUniformMatrix4fv(MVLoc, 1, GL_FALSE, (const GLfloat*) MV);

  GLint cameraPosLoc = glGetUniformLocation(idProgramShader, "cameraPosition");
  glUniform3fv(cameraPosLoc, 1, (const GLfloat*) cameraPos);
  GLint lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
  glUniform3fv(lightPosLoc, 1, (const GLfloat*) lightPos);
  GLint heightFactorLoc = glGetUniformLocation(idProgramShader, "heightFactor");
  glUniform1f(heightFactorLoc, heightFactor);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Please provide height and texture image files!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This is required for remote
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This might be used for local

  win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", NULL, NULL);

  if (!win) {
      glfwTerminate();
      exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

      glfwTerminate();
      exit(-1);
  }

  string vert = "shader.vert";
  string frag = "shader.frag";
  initShaders(idProgramShader, vert, frag);
  glUseProgram(idProgramShader);


  glfwSetKeyCallback(win, keyCallback);

  initTexture(argv[1], argv[2], &textureWidth, &textureHeight);
  
  GLint texUnitLoc = glGetUniformLocation(idProgramShader, "hTexture");
  glProgramUniform1i(idProgramShader, texUnitLoc , 1);
  texUnitLoc = glGetUniformLocation(idProgramShader, "rgbTexture");
  glProgramUniform1i(idProgramShader, texUnitLoc , 0);

  GLint widthLoc = glGetUniformLocation(idProgramShader, "widthTexture");
  glProgramUniform1i(idProgramShader, widthLoc , textureWidth);

  GLint heightLoc = glGetUniformLocation(idProgramShader, "heightTexture");
  glProgramUniform1i(idProgramShader, heightLoc , textureHeight);

  lightPos[0] = textureWidth/2.0;
  lightPos[1] = 100.0;
  lightPos[2] = textureHeight/2.0;

  setVBOs();
  initialSetup();
  glClearColor(0,0,0,1);
  glClearDepth(1);

  while(!glfwWindowShouldClose(win)) {
    glfwSwapBuffers(win);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwGetWindowSize(win, &widthWindow, &heightWindow);
    glViewport(0, 0, widthWindow, heightWindow);
    setMVP();
    drawMesh();
    vec3 deltaPos;
    vec3_scale(deltaPos, cameraGaze, cameraSpeed);
    vec3_add(cameraPos, cameraPos, deltaPos);
    glfwPollEvents();
  }


  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
