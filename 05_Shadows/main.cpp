#include <ImageLoader.h>
#include <GLApp.h>
#include <Vec2.h>
#include "Teapot.h"
#include "UnitPlane.h"
#include "UnitCube.h"


class LightProperties {
public:
  GLint modelViewProjectionMatrixUniform{-1};
  float degreesPerSecond{45.0f};
  float angle{0};
};


class MyGLApp : public GLApp {
public:
  LightProperties light;
  Mat4 projectionMatrix;

  GLTexture2D stonesDiffuse{GL_LINEAR, GL_LINEAR};
  GLTexture2D stonesSpecular{GL_LINEAR, GL_LINEAR};
  GLTexture2D stonesNormals{GL_LINEAR, GL_LINEAR};
  GLTexture2D udeNormals{GL_LINEAR, GL_LINEAR};

  GLProgram pPhongBump;
  GLProgram pPhongBumpTex;
  GLProgram pLight;

  GLArray lightArray;
  GLBuffer lightPosBuffer{GL_ARRAY_BUFFER};
  GLBuffer lightIndexBuffer{GL_ELEMENT_ARRAY_BUFFER};

  GLArray planeArray;
  GLBuffer planePosBuffer{GL_ARRAY_BUFFER};
  GLBuffer planeNormalBuffer{GL_ARRAY_BUFFER};
  GLBuffer planeTangBuffer{GL_ARRAY_BUFFER};
  GLBuffer planeBinBuffer{GL_ARRAY_BUFFER};
  GLBuffer planeTexCoordBuffer{GL_ARRAY_BUFFER};

  GLArray teapotArray;
  GLBuffer teapotPosBuffer{GL_ARRAY_BUFFER};
  GLBuffer teapotNormalBuffer{GL_ARRAY_BUFFER};
  GLBuffer teapotTangBuffer{GL_ARRAY_BUFFER};
  GLBuffer teapotBinBuffer{GL_ARRAY_BUFFER};
  GLBuffer teapotTexCoordBuffer{GL_ARRAY_BUFFER};
  GLBuffer teapotIndexBuffer{GL_ELEMENT_ARRAY_BUFFER};

  bool leftMouseDown{false};
  bool rightMouseDown{false};
  bool controlDown{false};

  // camera
  bool cameraActive{false};
  bool firstCameraUpdate{true};
  Vec3 viewPosition = { 0, 0, 100 }; // view translation position
  Vec3 viewRotation = { -45, 0, 0 }; // view rotation angles
  Vec2 mouse = { 0, 0 }; // last mouse position
  float mouseSensitivity{0.15f}; // system specific factor
  float mousewheelFactor{10.0f}; // system specific factor

  MyGLApp() :
    GLApp(800,600,1,"Assignment 05 - Hello Shadows"),
    pPhongBump{GLProgram::createFromFile("res/phongBump.vert","res/phongBump.frag")},
    pPhongBumpTex{GLProgram::createFromFile("res/phongBump.vert","res/phongBumpTex.frag")},
    pLight{GLProgram::createFromFile("res/light.vert","res/light.frag")}
  {}

  virtual void init() override {
    setupTextures();
    setupGeometry();
    GL(glDisable(GL_CULL_FACE)); // the teapot is not watertight
    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthFunc(GL_LESS));
    GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    setAnimation(false);
  }

  void setupTextures() {
    Image image = ImageLoader::load("res/Stones_Diffuse.png");
    stonesDiffuse.setData(image.data,image.width, image.height, image.componentCount);

    image = ImageLoader::load("res/Stones_Specular.png");
    stonesSpecular.setData(image.data,image.width, image.height, image.componentCount);

    image = ImageLoader::load("res/Stones_Normals.png");
    stonesNormals.setData(image.data,image.width, image.height, image.componentCount);

    image = ImageLoader::load("res/UDE_Normals.png");
    udeNormals.setData(image.data,image.width, image.height, image.componentCount);
  }

  virtual void animate(double animationTime) override {
    light.angle = light.degreesPerSecond * float(animationTime);
  }

  virtual void draw() override {
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    Mat4 viewMatrix = Mat4::lookAt(viewPosition, {0,0,0}, {0,1,0});
    viewMatrix = viewMatrix * Mat4::rotationX(viewRotation[0]);
    viewMatrix = viewMatrix * Mat4::rotationY(viewRotation[1]);
    viewMatrix = viewMatrix * Mat4::rotationZ(viewRotation[2]);

    pLight.enable();

    const Mat4 lightModelMatrix = Mat4::rotationY(-light.angle) *  Mat4::translation(-35, 35, 35);
    const Vec4 lightPosition =  viewMatrix * lightModelMatrix * Vec4(0, 0, 0, 1);

    pLight.setUniform("MVP", projectionMatrix * viewMatrix * lightModelMatrix);
    lightArray.bind();
    GL(glDrawElements(GL_TRIANGLES, sizeof(UnitCube::indices) / sizeof(UnitCube::indices[0]), GL_UNSIGNED_INT, (void*)0));

    pPhongBumpTex.enable();
    Mat4 modelMatrix = Mat4::scaling(100, 100, 100);
    Mat4 modelView = viewMatrix * modelMatrix;
    Mat4 modelViewProjection = projectionMatrix * modelView;
    Mat4 modelViewIT = Mat4::transpose(Mat4::inverse(modelView));

    pPhongBumpTex.setUniform("MVP", modelViewProjection);
    pPhongBumpTex.setUniform("MV", modelView);
    pPhongBumpTex.setUniform("MVit", modelViewIT);
    pPhongBumpTex.setUniform("lightPosition", lightPosition);
    pPhongBumpTex.setTexture("td", stonesDiffuse,0);
    pPhongBumpTex.setTexture("ts", stonesSpecular,1);
    pPhongBumpTex.setTexture("tn", stonesNormals,2);
    planeArray.bind();
    GL(glDrawArrays(GL_TRIANGLES, 0, sizeof(UnitPlane::vertices) / sizeof(UnitPlane::vertices[0])));

    modelMatrix = {};
    modelView = viewMatrix * modelMatrix;
    modelViewProjection = projectionMatrix * modelView;
    modelViewIT = Mat4::transpose(Mat4::inverse(modelView));

    pPhongBump.enable();
    pPhongBump.setUniform("MVP", modelViewProjection);
    pPhongBump.setUniform("MV", modelView);
    pPhongBump.setUniform("MVit", modelViewIT);
    pPhongBump.setUniform("lightPosition", lightPosition);
    pPhongBump.setTexture("tn", udeNormals,0);

    teapotArray.bind();
    GL(glDrawElements(GL_TRIANGLES, sizeof(Teapot::indices) / sizeof(Teapot::indices[0]), GL_UNSIGNED_INT, (void*)0));
  }

  virtual void resize(int width, int height) override {
    float ratio = static_cast<float>(width) / static_cast<float>(height);
    projectionMatrix = Mat4::perspective(60.0f, ratio, 0.1f, 10000.0f);
    GL(glViewport(0, 0, width, height));
  }

  void setupGeometry() {
    lightPosBuffer.setData(UnitCube::vertices,
                            sizeof(UnitCube::vertices)/sizeof(UnitCube::vertices[0]),
                            3, GL_STATIC_DRAW);
    lightArray.connectVertexAttrib(lightPosBuffer, pLight, "vertexPosition", 3);
    lightIndexBuffer.setData(UnitCube::indices, sizeof(UnitCube::indices)/sizeof(UnitCube::indices[0]));


    planePosBuffer.setData(UnitPlane::vertices,
                        sizeof(UnitPlane::vertices)/sizeof(UnitPlane::vertices[0]),
                        3, GL_STATIC_DRAW);
    planeArray.connectVertexAttrib(planePosBuffer, pPhongBumpTex, "vertexPosition", 3);
    planeNormalBuffer.setData(UnitPlane::normals,
                           sizeof(UnitPlane::normals)/sizeof(UnitPlane::normals[0]),
                           3, GL_STATIC_DRAW);
    planeArray.connectVertexAttrib(planeNormalBuffer, pPhongBumpTex, "vertexNormal", 3);
    planeTangBuffer.setData(UnitPlane::tangents,
                           sizeof(UnitPlane::tangents)/sizeof(UnitPlane::tangents[0]),
                           3, GL_STATIC_DRAW);
    planeArray.connectVertexAttrib(planeTangBuffer, pPhongBumpTex, "vertexTangent", 3);
    planeBinBuffer.setData(UnitPlane::binormals,
                           sizeof(UnitPlane::binormals)/sizeof(UnitPlane::binormals[0]),
                           3, GL_STATIC_DRAW);
    planeArray.connectVertexAttrib(planeBinBuffer, pPhongBumpTex, "vertexBinormal", 3);
    planeTexCoordBuffer.setData(UnitPlane::texCoords,
                              sizeof(UnitPlane::texCoords)/sizeof(UnitPlane::texCoords[0]),
                              2, GL_STATIC_DRAW);
    planeArray.connectVertexAttrib(planeTexCoordBuffer, pPhongBumpTex, "vertexTexCoords", 2);

    teapotPosBuffer.setData(Teapot::vertices,
                           sizeof(Teapot::vertices)/sizeof(Teapot::vertices[0]),
                           3, GL_STATIC_DRAW);
    teapotArray.connectVertexAttrib(teapotPosBuffer, pPhongBump, "vertexPosition", 3);
    teapotNormalBuffer.setData(Teapot::normals,
                              sizeof(Teapot::normals)/sizeof(Teapot::normals[0]),
                              3, GL_STATIC_DRAW);
    teapotArray.connectVertexAttrib(teapotNormalBuffer, pPhongBump, "vertexNormal", 3);
    teapotTangBuffer.setData(Teapot::tangents,
                            sizeof(Teapot::tangents)/sizeof(Teapot::tangents[0]),
                            3, GL_STATIC_DRAW);
    teapotArray.connectVertexAttrib(teapotTangBuffer, pPhongBump, "vertexTangent", 3);
    teapotBinBuffer.setData(Teapot::binormals,
                           sizeof(Teapot::binormals)/sizeof(Teapot::binormals[0]),
                           3, GL_STATIC_DRAW);
    teapotArray.connectVertexAttrib(teapotBinBuffer, pPhongBump, "vertexBinormal", 3);
    teapotTexCoordBuffer.setData(Teapot::texCoords,
                                sizeof(Teapot::texCoords)/sizeof(Teapot::texCoords[0]),
                                3, GL_STATIC_DRAW);
    teapotArray.connectVertexAttrib(teapotTexCoordBuffer, pPhongBump, "vertexTexCoords", 3);
    teapotIndexBuffer.setData(Teapot::indices, sizeof(Teapot::indices)/sizeof(Teapot::indices[0]));
  }

  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (key == GLFW_KEY_LEFT_CONTROL) controlDown = action == GLFW_PRESS;

    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          closeWindow();
          break;
        case GLFW_KEY_SPACE:
          setAnimation(!getAnimation());
          break;
        case GLFW_KEY_R:
          resetAnimation();
          viewPosition = Vec3{ 0, 0, 100 };
          viewRotation = Vec3{ -45, 0, 0 };
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
