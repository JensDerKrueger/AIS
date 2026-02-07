#include <ImageLoader.h>
#include <GLApp.h>
#include <Vec2.h>
#include <GLFramebuffer.h>

#include "Teapot.h"
#include "UnitPlane.h"
#include "UnitCube.h"

static const std::string shadowVertexShader {R"(uniform mat4 MVP;
in vec3 vPos;
void main() {gl_Position = MVP * vec4(vPos, 1.0);})"};

static const std::string shadowFragmentShader {R"(void main() {})"};

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

  GLProgram shadowProgram;
  GLFramebuffer framebuffer;
  GLDepthTexture shadowMap;
  const Mat4 cliptToTextureMatrix {
    0.5f, 0.0f, 0.0f, 0.5f,
    0.0f, 0.5f, 0.0f, 0.5f,
    0.0f, 0.0f, 0.5f, 0.5f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

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

  Mat4 viewMatrix;
  Mat4 lightModelMatrix;
  Mat4 lightProjectionMatrix;
  Mat4 lightViewMatrix;
  Mat4 worldToShadowMatrix;

  Vec4 lightPosition;

  MyGLApp() :
  GLApp(800,600,1,"Exercise 06 - Hello Sky"),
  pPhongBump{GLProgram::createFromFile("phongBump.vert","phongBump.frag","",false,true)},
  pPhongBumpTex{GLProgram::createFromFile("phongBump.vert","phongBumpTex.frag","",false,true)},
  pLight{GLProgram::createFromFile("light.vert","light.frag","",false,true)},
  shadowProgram{GLProgram::createFromString(shadowVertexShader,shadowFragmentShader,"",false,true)}
  {
    shadowMap.setEmpty(2048,2048);
  }

  virtual void init() override {
    setupTextures();
    setupGeometry();
    GL(glDisable(GL_CULL_FACE)); // the teapot is not watertight
    GL(glEnable(GL_DEPTH_TEST));
    GL(glDepthFunc(GL_LESS));
    GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    setAnimation(false);
    resetAnimation();
  }

  void setupTextures() {
    Image image = ImageLoader::load("Stones_Diffuse.png");
    stonesDiffuse.setData(image.data,image.width, image.height, image.componentCount);

    image = ImageLoader::load("Stones_Specular.png");
    stonesSpecular.setData(image.data,image.width, image.height, image.componentCount);

    image = ImageLoader::load("Stones_Normals.png");
    stonesNormals.setData(image.data,image.width, image.height, image.componentCount);

    image = ImageLoader::load("teapot_normals.png");
    udeNormals.setData(image.data,image.width, image.height, image.componentCount);
  }

  virtual void animate(double animationTime) override {
    light.angle = light.degreesPerSecond * float(animationTime);
  }

  void updateState() {
    viewMatrix = Mat4::lookAt(viewPosition, {0,0,0}, {0,1,0});

    viewMatrix = viewMatrix * Mat4::rotationX(viewRotation[0]);
    viewMatrix = viewMatrix * Mat4::rotationY(viewRotation[1]);
    viewMatrix = viewMatrix * Mat4::rotationZ(viewRotation[2]);

    lightModelMatrix = Mat4::rotationY(light.angle) *  Mat4::translation(-80, 60, 80);
    lightPosition =  viewMatrix * lightModelMatrix * Vec4(0, 0, 0, 1);

    lightProjectionMatrix = Mat4::perspective(60.0f,
                                              float(shadowMap.getWidth())/
                                              float(shadowMap.getHeight()),
                                              1.0f, 400);
    lightViewMatrix = Mat4::lookAt(lightModelMatrix * Vec3{0,0,0}, {0,0,0}, {0,1,0});

    worldToShadowMatrix = cliptToTextureMatrix*lightProjectionMatrix*lightViewMatrix;
  }

  void renderLightSource() {
    pLight.enable();
    pLight.setUniform("MVP", projectionMatrix * viewMatrix * lightModelMatrix);
    lightArray.bind();
    GL(glDrawElements(GL_TRIANGLES, sizeof(UnitCube::indices) / sizeof(UnitCube::indices[0]), GL_UNSIGNED_INT, (void*)0));
  }

  void renderScene(bool forReal) {

    Mat4 modelMatrix = Mat4::scaling(100, 100, 100);

    if (forReal) {
      const Mat4 modelView = viewMatrix * modelMatrix;
      const Mat4 modelViewProjection = projectionMatrix * modelView;
      const Mat4 modelViewIT = Mat4::transpose(Mat4::inverse(modelView));

      pPhongBumpTex.enable();
      pPhongBumpTex.setUniform("MVP", modelViewProjection);
      pPhongBumpTex.setUniform("MV", modelView);
      pPhongBumpTex.setUniform("M", modelMatrix);
      pPhongBumpTex.setUniform("worldToShadow", worldToShadowMatrix);
      pPhongBumpTex.setUniform("MVit", modelViewIT);
      pPhongBumpTex.setUniform("lightPosition", lightPosition);
      pPhongBumpTex.setTexture("td", stonesDiffuse,0);
      pPhongBumpTex.setTexture("ts", stonesSpecular,1);
      pPhongBumpTex.setTexture("tn", stonesNormals,2);
      pPhongBumpTex.setTexture("shadowMap", shadowMap,3);
    } else {
      shadowProgram.enable();
      shadowProgram.setUniform("MVP", lightProjectionMatrix*lightViewMatrix*modelMatrix);
    }

    planeArray.bind();
    GL(glDrawArrays(GL_TRIANGLES, 0, sizeof(UnitPlane::vertices) / (3*sizeof(UnitPlane::vertices[0]))));

    modelMatrix = {};

    if (forReal) {
      const Mat4 modelView = viewMatrix * modelMatrix;
      const Mat4 modelViewProjection = projectionMatrix * modelView;
      const Mat4 modelViewIT = Mat4::transpose(Mat4::inverse(modelView));

      pPhongBump.enable();
      pPhongBump.setUniform("MVP", modelViewProjection);
      pPhongBump.setUniform("MV", modelView);
      pPhongBump.setUniform("M", modelMatrix);
      pPhongBumpTex.setUniform("worldToShadow", worldToShadowMatrix);
      pPhongBump.setUniform("MVit", modelViewIT);
      pPhongBump.setUniform("lightPosition", lightPosition);
      pPhongBump.setTexture("tn", udeNormals,0);
      pPhongBump.setTexture("shadowMap", shadowMap,1);
    } else {
      shadowProgram.setUniform("MVP", lightProjectionMatrix*lightViewMatrix*modelMatrix);
    }

    teapotArray.bind();
    GL(glDrawElements(GL_TRIANGLES, sizeof(Teapot::indices) / sizeof(Teapot::indices[0]), GL_UNSIGNED_INT, (void*)0));
  }

  virtual void draw() override {
    updateState();

    framebuffer.bind(shadowMap);
    GL(glViewport(0, 0, GLsizei(shadowMap.getWidth() ), GLsizei(shadowMap.getHeight())));
    GL(glClear(GL_DEPTH_BUFFER_BIT));
    renderScene(false);
    framebuffer.unbind2D();

    const Dimensions dim = glEnv.getFramebufferSize();
    GL(glViewport(0, 0, GLsizei(dim.width), GLsizei(dim.height)));
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    renderLightSource();
    renderScene(true);
  }

  virtual void resize(const Dimensions winDim, const Dimensions fbDim) override {
    GLApp::resize(winDim, fbDim);
    projectionMatrix = Mat4::perspective(60.0f, fbDim.aspect(), 0.1f, 10000.0f);
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
    if (key == GLENV_KEY_LEFT_CONTROL) controlDown = action == GLENV_PRESS;

    if (action == GLENV_PRESS) {
      switch (key) {
        case GLENV_KEY_ESCAPE:
          closeWindow();
          break;
        case GLENV_KEY_SPACE:
          setAnimation(!getAnimation());
          break;
        case GLENV_KEY_R:
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
    if (button == GLENV_MOUSE_BUTTON_RIGHT) rightMouseDown = action == GLENV_MOUSE_PRESS;
    if (button == GLENV_MOUSE_BUTTON_LEFT) leftMouseDown = action == GLENV_MOUSE_PRESS;

    if ((button == GLENV_MOUSE_BUTTON_LEFT ||
         button == GLENV_MOUSE_BUTTON_RIGHT) && action == GLENV_MOUSE_PRESS) {
      mouse[0] = static_cast<float>(xPosition);
      mouse[1] = static_cast<float>(yPosition);
      cameraActive = true;
      firstCameraUpdate = true;
    } else if ((button == GLENV_MOUSE_BUTTON_LEFT ||
                button == GLENV_MOUSE_BUTTON_RIGHT) && action == GLENV_MOUSE_RELEASE) {
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
} myApp;

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
  std::vector<std::string> args = getArgsWindows();
#else
int main(int argc, char** argv) {
  std::vector<std::string> args{ argv + 1, argv + argc };
#endif
  try {
    myApp.run();
  }
  catch (const GLException& e) {
    std::stringstream ss;
    ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
    std::cerr << ss.str().c_str() << std::endl;
#else
    MessageBoxA(
      NULL,
      ss.str().c_str(),
      "OpenGL Error",
      MB_ICONERROR | MB_OK
    );
#endif
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


