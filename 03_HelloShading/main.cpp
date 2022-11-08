#include <fstream>

#include <GLApp.h>
#include <Vec2.h>
#include "Teapot.h"
#include "UnitPlane.h"

class MyGLApp : public GLApp {
public:
  double time{};

  GLuint pFlat{};
  GLuint pGouraud{};
  GLuint pPhong{};
  Mat4 modelMatrix{};
  Mat4 projectionMatrix{};
  Mat4 viewMatrix{};
  bool leftMouseDown{false};
  bool rightMouseDown{false};
  bool controlDown{false};

  GLint modelViewProjectionMatrixUniform{-1};

  enum class Shading {
    FLAT, GOURAUD, PHONG
  };

  Shading shading{Shading::FLAT};

  Vec3 kd = { 1, 1, 1 }; // material diffuse color
  GLint kdUniform{-1};

  GLuint vbos[4]{ };
  GLuint vaos[2]{ };
  GLuint eboTeapot{};

  // camera
  bool cameraActive{false};
  bool firstCameraUpdate{true};
  Vec3 viewPosition = { 0, 0, -100 }; // view translation position
  Vec3 viewRotation = { -45, 0, 0 }; // view rotation angles
  Vec2 mouse = { 0, 0 }; // last mouse position
  float mouseSensitivity{0.15f}; // system specific factor
  float mousewheelFactor{10.0f}; // system specific factor

  MyGLApp() : GLApp(800,600,1,"Assignment 03 - Hello Shading") {}

  virtual void init() override {
    time = glfwGetTime();
    setupShaders();
    setupGeometry();
    GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
  }

  void selectShading() {
    switch (shading) {
      case Shading::FLAT:
        GL(glUseProgram(pFlat));
        kd = { 0.8f, 0.0f, 0.0f };
        GL(glUniform3fv(kdUniform, 1, kd));
        break;
      case Shading::GOURAUD:
        GL(glUseProgram(pGouraud));
        kd = { 0.0f, 0.8f, 0.0f };
        GL(glUniform3fv(kdUniform, 1, kd));
        break;
      case Shading::PHONG:
        GL(glUseProgram(pPhong));
        kd = { 0.0f, 0.0f, 0.8f };
        GL(glUniform3fv(kdUniform, 1, kd));
        break;
    }
  }

  virtual void draw() override {
    double t = glfwGetTime();
    double d = t - time;
    time = t;

    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    selectShading();
    viewMatrix = Mat4::translation(viewPosition[0], viewPosition[1], viewPosition[2]);
    viewMatrix = viewMatrix * Mat4::rotationX(viewRotation[0]);
    viewMatrix = viewMatrix * Mat4::rotationY(viewRotation[1]);
    viewMatrix = viewMatrix * Mat4::rotationZ(viewRotation[2]);

    modelMatrix = Mat4();
    modelMatrix = modelMatrix * Mat4::scaling(100, 100, 100);
    Mat4 modelViewProjection = projectionMatrix * viewMatrix * modelMatrix;
    GL(glUniformMatrix4fv(modelViewProjectionMatrixUniform, 1, GL_TRUE, modelViewProjection));
    GL(glBindVertexArray(vaos[0]));
    GL(glDrawArrays(GL_TRIANGLES, 0, sizeof(UnitPlane::vertices) / sizeof(UnitPlane::vertices[0])));

    modelMatrix = Mat4();
    modelViewProjection = projectionMatrix * viewMatrix * modelMatrix;
    GL(glUniformMatrix4fv(modelViewProjectionMatrixUniform, 1, GL_TRUE, modelViewProjection));
    GL(glBindVertexArray(vaos[1]));
    GL(glDrawElements(GL_TRIANGLES, sizeof(Teapot::indices) / sizeof(Teapot::indices[0]), GL_UNSIGNED_INT, (void*)0));
    GL(glBindVertexArray(0));

  }

  virtual void resize(int width, int height) override {
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    projectionMatrix = Mat4::perspective(60.0f, ratio, 0.1f, 10000.0f);
    GL(glViewport(0, 0, width, height));
  }

  std::string loadFile(const std::string& filename) {
    std::ifstream shaderFile{ filename };
    if (!shaderFile) {
      throw GLException{ std::string("Unable to open file ") + filename };
    }
    std::string str;
    std::string fileContents;
    while (std::getline(shaderFile, str)) {
      fileContents += str + "\n";
    }
    return fileContents;
  }

  GLuint createShaderFromFile(GLenum type, const std::string& sourcePath) {
    const std::string shaderCode = loadFile(sourcePath);
    const GLchar* c_shaderCode = shaderCode.c_str();
    const GLuint s = glCreateShader(type);
    GL(glShaderSource(s, 1, &c_shaderCode, NULL));
    glCompileShader(s); checkAndThrowShader(s);
    return s;
  }

  void setupShaders() {
    std::string vertexSrcPath =  "res/shaders/flat.vert";
    std::string fragmentSrcPath =  "res/shaders/flat.frag";
    GLuint vertexShader = createShaderFromFile(GL_VERTEX_SHADER, vertexSrcPath);
    GLuint fragmentShader = createShaderFromFile(GL_FRAGMENT_SHADER, fragmentSrcPath);

    pFlat = glCreateProgram();
    GL(glAttachShader(pFlat, vertexShader));
    GL(glAttachShader(pFlat, fragmentShader));
    GL(glLinkProgram(pFlat));
    checkAndThrowProgram(pFlat);

    modelViewProjectionMatrixUniform = glGetUniformLocation(pFlat, "MVP");
    kdUniform = glGetUniformLocation(pFlat, "kd");

    GL(glDeleteShader(vertexShader));
    GL(glDeleteShader(fragmentShader));

    vertexSrcPath =  "res/shaders/gouraud.vert";
    fragmentSrcPath =  "res/shaders/gouraud.frag";
    vertexShader = createShaderFromFile(GL_VERTEX_SHADER, vertexSrcPath);
    fragmentShader = createShaderFromFile(GL_FRAGMENT_SHADER, fragmentSrcPath);
    pGouraud = glCreateProgram();
    GL(glAttachShader(pGouraud, vertexShader));
    GL(glAttachShader(pGouraud, fragmentShader));
    GL(glLinkProgram(pGouraud));
    checkAndThrowProgram(pGouraud);
    GL(glDeleteShader(vertexShader));
    GL(glDeleteShader(fragmentShader));

    vertexSrcPath =  "res/shaders/phong.vert";
    fragmentSrcPath =  "res/shaders/phong.frag";
    vertexShader = createShaderFromFile(GL_VERTEX_SHADER, vertexSrcPath);
    fragmentShader = createShaderFromFile(GL_FRAGMENT_SHADER, fragmentSrcPath);
    pPhong = glCreateProgram();
    GL(glAttachShader(pPhong, vertexShader));
    GL(glAttachShader(pPhong, fragmentShader));
    GL(glLinkProgram(pPhong));
    checkAndThrowProgram(pPhong);
    GL(glDeleteShader(vertexShader));
    GL(glDeleteShader(fragmentShader));

    selectShading();
  }

  void setupGeometry() {
    const GLint vertexPositionLocation = glGetAttribLocation(pPhong, "vertexPosition");
    const GLint vertexNormalLocation = glGetAttribLocation(pPhong, "vertexNormal");

    GL(glGenVertexArrays(2, vaos));
    GL(glGenBuffers(4, vbos));
    GL(glGenBuffers(1, &eboTeapot));

    // unit plane in xy-plane
    GL(glBindVertexArray(vaos[0]));
    // setup vertices
    GL(glBindBuffer(GL_ARRAY_BUFFER, vbos[0]));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(UnitPlane::vertices), UnitPlane::vertices, GL_STATIC_DRAW));
    GL(glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    GL(glEnableVertexAttribArray(vertexPositionLocation));
    // setup normals
    GL(glBindBuffer(GL_ARRAY_BUFFER, vbos[1]));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(UnitPlane::normals), UnitPlane::normals, GL_STATIC_DRAW));
    GL(glVertexAttribPointer(vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    GL(glEnableVertexAttribArray(vertexNormalLocation));

    // teapot
    GL(glBindVertexArray(vaos[1]));
    // setup vertices
    GL(glBindBuffer(GL_ARRAY_BUFFER, vbos[2]));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Teapot::vertices), Teapot::vertices, GL_STATIC_DRAW));
    GL(glVertexAttribPointer(vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    GL(glEnableVertexAttribArray(vertexPositionLocation));
    // setup normals
    GL(glBindBuffer(GL_ARRAY_BUFFER, vbos[3]));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Teapot::normals), Teapot::normals, GL_STATIC_DRAW));
    GL(glVertexAttribPointer(vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    GL(glEnableVertexAttribArray(vertexNormalLocation));
    // setup indices for indexed drawing the teapot
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboTeapot));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Teapot::indices), Teapot::indices, GL_STATIC_DRAW));

    GL(glBindVertexArray(0));
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (key == GLFW_KEY_LEFT_CONTROL) controlDown = action == GLFW_PRESS;

    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          closeWindow();
          break;
        case GLFW_KEY_F:
          shading = Shading::FLAT;
          std::cout << "switched to FLAT shading" << std::endl;
          break;
        case GLFW_KEY_G:
          shading = Shading::GOURAUD;
          std::cout << "switched to GOURAUD shading" << std::endl;
          break;
        case GLFW_KEY_P:
          shading = Shading::PHONG;
          std::cout << "switched to PHONG shading" << std::endl;
          break;
      }

    }
  }

  virtual void mouseMove(double xPosition, double yPosition) override {
    if (cameraActive) {
      if (firstCameraUpdate) {
        mouse[0] = float(xPosition);
        mouse[1] = float(yPosition);
        firstCameraUpdate = false;
      }

      // rotation
      if (leftMouseDown) {
        viewRotation[0] += (mouse[1] - float(yPosition)) * mouseSensitivity;
        viewRotation[1] += (mouse[0] - float(xPosition)) * mouseSensitivity;
      }
      // panning
      else if (rightMouseDown) {
        float f = 0.6f;
        if (!controlDown) {
          viewPosition[0] -= (mouse[0] - float(xPosition)) * mouseSensitivity * f;
          viewPosition[1] += (mouse[1] - float(yPosition)) * mouseSensitivity * f;
        }
        else {
          viewPosition[2] -= (mouse[1] - float(yPosition)) * mouseSensitivity * f;
        }
      }
      mouse[0] = float(xPosition);
      mouse[1] = float(yPosition);
    }
  }

  virtual void mouseButton(int button, int action, int mods, double xPosition, double yPosition) override {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) rightMouseDown = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT) leftMouseDown = action == GLFW_PRESS;

    if ((button == GLFW_MOUSE_BUTTON_LEFT ||
         button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS) {
      mouse[0] = static_cast<float>(xPosition);
      mouse[1] = static_cast<float>(yPosition);
      cameraActive = true;
      firstCameraUpdate = true;
    } else if ((button == GLFW_MOUSE_BUTTON_LEFT ||
              button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_RELEASE) {
      cameraActive = false;
      firstCameraUpdate = false;
    }
  }

  virtual void mouseWheel(double x_offset, double y_offset, double xPosition, double yPosition) override {
    // panning
    float f = viewPosition[2] / mousewheelFactor;
    viewPosition[0] -= float(x_offset) * f;
    viewPosition[2] -= float(y_offset) * f;
  }
};

int main(int argc, char** argv) {
  MyGLApp myApp;
  myApp.run();
  return EXIT_SUCCESS;
}
