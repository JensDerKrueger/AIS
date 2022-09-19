#include <array>
#include <sstream>

#include "GLTexture2D.h"

GLTexture2D::GLTexture2D(GLint magFilter, GLint minFilter, GLint wrapX, GLint wrapY) :
  id(0),
  internalformat(0),
  format(0),
  type(0),
  magFilter(magFilter),
  minFilter(minFilter),
  wrapX(wrapX),
  wrapY(wrapY),
  width(0),
  height(0),
  componentCount(0),
  dataType(GLDataType::BYTE)
{
  GL(glGenTextures(1, &id));
  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTexture2D::GLTexture2D(const Image& image,
                         GLint magFilter, GLint minFilter, GLint wrapX, GLint wrapY) :
GLTexture2D(magFilter, minFilter, wrapX, wrapY)
{
  setData(image);
}


void GLTexture2D::setFilter(GLint magFilter, GLint minFilter) {
  this->magFilter = magFilter;
  this->minFilter = minFilter;
  
  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTexture2D::~GLTexture2D() {
  GL(glDeleteTextures(1, &id));
}

GLTexture2D::GLTexture2D(const GLTexture2D& other) :
  GLTexture2D(other.magFilter, other.minFilter, other.wrapX, other.wrapY)
{
  if (other.height > 0 && other.width > 0) {
    switch (other.dataType) {
      case GLDataType::BYTE  :
        setData(other.data, other.width, other.height, other.componentCount);
        break;
      case GLDataType::HALF  :
        setData(other.hdata, other.width, other.height, other.componentCount);
        break;
      case GLDataType::FLOAT :
        setData(other.fdata, other.width, other.height, other.componentCount);
        break;
    }
  }
}

GLTexture2D& GLTexture2D::operator=(const GLTexture2D& other) {
  magFilter = other.magFilter;
  minFilter = other.minFilter;
  wrapX = other.wrapX;
  wrapY = other.wrapY;

  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
  
  if (other.height > 0 && other.width > 0) {
    switch (other.dataType) {
      case GLDataType::BYTE  :
        setData(other.data, other.width, other.height, other.componentCount);
        break;
      case GLDataType::HALF  :
        setData(other.hdata, other.width, other.height, other.componentCount);
        break;
      case GLDataType::FLOAT :
        setData(other.fdata, other.width, other.height, other.componentCount);
        break;
    }
  }
  return *this;
}

const GLint GLTexture2D::getId() const {
  return id;
}

void GLTexture2D::clear() {
  setEmpty(width,height,componentCount,dataType);
}

void GLTexture2D::setData(const Image& image) {
  this->data = image.data;
  setData((GLvoid*)(image.data.data()), image.width, image.height, image.componentCount, GLDataType::BYTE);
}

void GLTexture2D::setData(const std::vector<GLubyte>& data) {
  setData(data,width,height,componentCount);
}

void GLTexture2D::setData(const std::vector<GLfloat>& data) {
  setData(data,width,height,componentCount);
}

void GLTexture2D::setEmpty(uint32_t width, uint32_t height, uint32_t componentCount, GLDataType dataType) {
  switch (dataType) {
    case GLDataType::BYTE  : setData(std::vector<GLubyte>(width*height*componentCount), width, height, componentCount); break;
    case GLDataType::HALF  : setData(std::vector<GLhalf>(width*height*componentCount), width, height, componentCount); break;
    case GLDataType::FLOAT : setData(std::vector<GLfloat>(width*height*componentCount), width, height, componentCount); break;
  }
}

void GLTexture2D::setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, uint32_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texure dimensions do not match."};
  }
  
  this->data = data;
  setData((GLvoid*)data.data(), width, height, componentCount, GLDataType::BYTE);
}

void GLTexture2D::setData(const std::vector<GLhalf>& data, uint32_t width, uint32_t height, uint32_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texure dimensions do not match."};
  }
  
  this->hdata = data;
  setData((GLvoid*)data.data(), width, height, componentCount, GLDataType::HALF);
}

void GLTexture2D::setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, uint32_t componentCount) {
  if (data.size() != componentCount*width*height) {
    std::stringstream ss;
    ss << "Data size " << data.size() << " and texure dimensions " << componentCount << "*" << width
       << "*" << height << "=" << componentCount*width*height << " do not match.";
    throw GLException{ss.str()};
  }
  
  this->fdata = data;
  setData((GLvoid*)this->fdata.data(), width, height, componentCount, GLDataType::FLOAT);
}

static std::array<GLenum,3> dataTypeToGL(GLDataType dataType, uint32_t componentCount) {
  GLenum type{0};
  GLenum internalformat{0};
  GLenum format{0};
  
  switch (dataType) {
    case GLDataType::BYTE :
      type = GL_UNSIGNED_BYTE;
      switch (componentCount) {
        case 1 :
          internalformat = GL_R8;
          format = GL_RED;
          break;
        case 2 :
          internalformat = GL_RG8;
          format = GL_RG;
          break;
        case 3 :
          internalformat = GL_RGB8;
          format = GL_RGB;
          break;
        case 4 :
          internalformat = GL_RGBA8;
          format = GL_RGBA;
          break;
      }
      break;

    case GLDataType::HALF :
      type = GL_HALF_FLOAT;
      switch (componentCount) {
        case 1 :
          internalformat = GL_R16F;
          format = GL_RED;
          break;
        case 2 :
          internalformat = GL_RG16F;
          format = GL_RG;
          break;
        case 3 :
          internalformat = GL_RGB16F;
          format = GL_RGB;
          break;
        case 4 :
          internalformat = GL_RGBA16F;
          format = GL_RGBA;
          break;
      }
      break;
      
    case GLDataType::FLOAT :
      type = GL_FLOAT;
      switch (componentCount) {
        case 1 :
          internalformat = GL_R32F;
          format = GL_RED;
          break;
        case 2 :
          internalformat = GL_RG32F;
          format = GL_RG;
          break;
        case 3 :
          internalformat = GL_RGB32F;
          format = GL_RGB;
          break;
        case 4 :
          internalformat = GL_RGBA32F;
          format = GL_RGBA;
          break;
      }
      break;
  }

  return {type, internalformat, format};
}

void GLTexture2D::setData(GLvoid* data, uint32_t width, uint32_t height, uint32_t componentCount, GLDataType dataType) {
  this->dataType = dataType;
  this->width = width;
  this->height = height;
  this->componentCount = componentCount;

  GL(glBindTexture(GL_TEXTURE_2D, id));

  GL(glPixelStorei(GL_PACK_ALIGNMENT ,1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT ,1));

  std::array<GLenum,3> format = dataTypeToGL(dataType, componentCount);
  
  GL(glTexImage2D(GL_TEXTURE_2D, 0, format[1], GLuint(width), GLuint(height), 0, format[2], format[0], data));
}


void GLTexture2D::setPixel(const std::vector<GLubyte>& data, uint32_t x, uint32_t y) {  
  std::array<GLenum,3> format = dataTypeToGL(dataType, componentCount);
  GL(glBindTexture(GL_TEXTURE_2D, id));
  glTexSubImage2D(GL_TEXTURE_2D,0,GLint(x),GLint(y),1,1, format[2], format[0], data.data());
}

void GLTexture2D::generateMipmap() {
  GL(glBindTexture(GL_TEXTURE_2D, id));
  GL(glGenerateMipmap(GL_TEXTURE_2D));
}

Image GLTexture2D::getImage() {
  return {width, height, componentCount, getDataByte()};
}

const std::vector<GLubyte>& GLTexture2D::getDataByte() {
  GL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL(glBindTexture(GL_TEXTURE_2D, id));
  
  std::array<GLenum,3> format = dataTypeToGL(GLDataType::BYTE, componentCount);
  data.resize(componentCount*width*height);
  GL(glGetTexImage(GL_TEXTURE_2D, 0, format[2], format[0], data.data()));
  return data;
}

const std::vector<GLhalf>& GLTexture2D::getDataHalf() {
  GL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL(glBindTexture(GL_TEXTURE_2D, id));

  std::array<GLenum,3> format = dataTypeToGL(GLDataType::HALF, componentCount);
  hdata.resize(componentCount*width*height);
  GL(glGetTexImage(GL_TEXTURE_2D, 0, format[2], format[0], hdata.data()));
  return hdata;
}

const std::vector<GLfloat>& GLTexture2D::getDataFloat() {
  GL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL(glBindTexture(GL_TEXTURE_2D, id));

  std::array<GLenum,3> format = dataTypeToGL(GLDataType::FLOAT, componentCount);
  hdata.resize(componentCount*width*height);
  GL(glGetTexImage(GL_TEXTURE_2D, 0, format[2], format[0], fdata.data()));
  return fdata;
}
