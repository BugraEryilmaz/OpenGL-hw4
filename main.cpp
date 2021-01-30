#include "helper.h"
#define _USE_MATH_DEFINES
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>

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

//vec3 cameraPos = {textureWidth/2.f, textureWidth/10.f, -textureWidth/4.f};
//vec3 cameraUp = {0, 1, 0};
//vec3 cameraGaze = {0, 0, 1};
glm::vec3 cameraPos = glm::vec3(textureWidth/2.f, textureWidth/10.f, -textureWidth/4.f);
glm::vec3 cameraUP = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraGaze = glm::vec3(0.0, 0.0, 1.0);
glm::vec3 cameraCross = cross(cameraUP, cameraGaze);
glm::vec3 Tleft = glm::vec3(-1.0, 0.0, 0.0);
glm::vec3 Tright = glm::vec3(1.0, 0.0, 0.0);
float cameraSpeed = 0;


glm::mat4x4 Model;
glm::mat4x4 View;
glm::mat4x4 Projection;
glm::mat4x4 MV;
glm::mat4x4 MVP;

glm::vec3 lightPos;
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
  cameraUP[0] = 0;
  cameraUP[1] = 1;
  cameraUP[2] = 0;
  cameraGaze[0] = 0;
  cameraGaze[1] = 0;
  cameraGaze[2] = 1;
  cameraSpeed = 0;

  Model = glm::mat4x4(1);
  View = glm::mat4x4(1);
  Projection = glm::mat4x4(1);

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
      if (isFullScreen)
        glfwSetWindowMonitor(window, nullptr, 0, 0, widthWindow, heightWindow, 0);
      else{
        const GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        GLFWvidmode *mode = const_cast<GLFWvidmode *>(glfwGetVideoMode(const_cast<GLFWmonitor *>(monitor)));
        glfwSetWindowMonitor(window, const_cast<GLFWmonitor*>(monitor),0,0,mode->width, mode->height,mode->refreshRate);
        widthWindow = mode->width;
        heightWindow = mode->height;
      }
      glfwMakeContextCurrent(win);
      glfwSetKeyCallback(win, keyCallback);
      isFullScreen = !isFullScreen;
    }
   if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      //mat4x4_rotate(cameraGaze, camertGaze, cameraUp[0], cameraUp[1], cameraUp[2], -0.05f);
    
    cameraUP = glm::rotate(cameraUP, -0.05f, cameraCross);
    cameraGaze = glm::rotate(cameraGaze, -0.05f, cameraCross);
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    //mat4x4_rotate(cameraGaze, camertGaze, cameraUp[0], cameraUp[1], cameraUp[2], 0.05f);
      cameraUP = glm::rotate(cameraUP, 0.05f, cameraCross);
      cameraGaze = glm::rotate(cameraGaze, 0.05f, cameraCross);
    }
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    //mat4x4_rotate(cameraGaze, camertGaze, cameraUp[0], cameraUp[1], cameraUp[2], 0.05f);
      cameraCross = glm::rotate(cameraCross, 0.05f, cameraUP);
      cameraGaze = glm::rotate(cameraGaze, 0.05f, cameraUP);
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      //mat4x4_rotate(cameraGaze, camertGaze, cameraUp[0], cameraUp[1], cameraUp[2], -0.05f);
       cameraCross = glm::rotate(cameraCross, -0.05f, cameraUP);
       cameraGaze = glm::rotate(cameraGaze, -0.05f, cameraUP);
    }
     if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      heightFactor += 0.5; //OK
      GLint heightFactorLoc = glGetUniformLocation(idProgramShader, "heightFactor");
     glUniform1f(heightFactorLoc, heightFactor);
    }
     if (key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      heightFactor -= 0.5; //OK
      GLint heightFactorLoc = glGetUniformLocation(idProgramShader, "heightFactor");
      glUniform1f(heightFactorLoc, heightFactor);          
    }
     if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      glm::mat4x4 temp = glm::translate(Tleft);
      Model = temp * Model;
      //Model = glm::translate(Model, Tleft);
    }
      if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      Model = glm::translate(Model, Tright);
    }
     if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      lightPos[1] += 5;   //OK
    }
     if (key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      lightPos[1] -= 5; //OK
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
  Projection = glm::perspective(fov, aspectRatio, nearDistance, farDistance);
  View = glm::lookAt(cameraPos, glm::vec3(cameraPos + cameraGaze), cameraUP);
  MV = View * Model;
  MVP = Projection * MV;

  GLint MVPLoc = glGetUniformLocation(idProgramShader, "MVP");
  glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &MVP[0][0]);
  GLint MVLoc = glGetUniformLocation(idProgramShader, "MV");
  glUniformMatrix4fv(MVLoc, 1, GL_FALSE, (const GLfloat*) &MV[0][0]);

  GLint cameraPosLoc = glGetUniformLocation(idProgramShader, "cameraPosition");
  glUniform3fv(cameraPosLoc, 1, (const GLfloat*) &cameraPos[0]);
  GLint lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
  glUniform3fv(lightPosLoc, 1, (const GLfloat*) &lightPos[0]);
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

  glEnable(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);  
    
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
    cameraPos = cameraPos + cameraGaze * cameraSpeed;
    glfwPollEvents();
  }


  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
