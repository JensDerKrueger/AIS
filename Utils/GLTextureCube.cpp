#include <array>
#include <sstream>

#include "GLTextureCube.h"

GLTextureCube::GLTextureCube(GLint magFilter, GLint minFilter, GLint wrapX, GLint wrapY, GLint wrapZ) :
  id(0),
  internalformat(0),
  format(0),
  type(0),
  magFilter(magFilter),
  minFilter(minFilter),
  wrapX(wrapX),
  wrapY(wrapY),
  wrapZ(wrapZ),
  width(0),
  height(0),
  componentCount(0),
  dataType(GLDataType::BYTE)
{
  GL(glGenTextures(1, &id));
  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapZ));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTextureCube::GLTextureCube(const Image& image0,
                             const Image& image1,
                             const Image& image2,
                             const Image& image3,
                             const Image& image4,
                             const Image& image5,
                             GLint magFilter, GLint minFilter,
                             GLint wrapX, GLint wrapY) :
GLTextureCube(magFilter, minFilter, wrapX, wrapY)
{
  setData(image0,Face::POSX);
  setData(image1,Face::NEGX);
  setData(image2,Face::POSY);
  setData(image3,Face::POSY);
  setData(image4,Face::POSZ);
  setData(image5,Face::POSZ);
}


void GLTextureCube::setFilter(GLint magFilter, GLint minFilter) {
  this->magFilter = magFilter;
  this->minFilter = minFilter;
  
  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter));
}

GLTextureCube::~GLTextureCube() {
  GL(glDeleteTextures(1, &id));
}

GLTextureCube::GLTextureCube(const GLTextureCube& other) :
  GLTextureCube(other.magFilter, other.minFilter, other.wrapX, other.wrapY, other.wrapZ)
{
  if (other.height > 0 && other.width > 0) {
    for (size_t face = 0;face<6;++face) {
      switch (other.dataType) {
        case GLDataType::BYTE  :
          setData(other.data, other.width, other.height, Face(face), other.componentCount);
          break;
        case GLDataType::HALF  :
          setData(other.hdata, other.width, other.height, Face(face), other.componentCount);
          break;
        case GLDataType::FLOAT :
          setData(other.fdata, other.width, other.height, Face(face), other.componentCount);
          break;
      }
    }
  }
}

GLTextureCube& GLTextureCube::operator=(const GLTextureCube& other) {
  magFilter = other.magFilter;
  minFilter = other.minFilter;
  wrapX = other.wrapX;
  wrapY = other.wrapY;
  wrapZ = other.wrapZ;

  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapX));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapY));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapZ));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter));
  GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter));
  
  if (other.height > 0 && other.width > 0) {
    for (size_t face = 0;face<6;++face) {
      switch (other.dataType) {
        case GLDataType::BYTE  :
          setData(other.data, other.width, other.height, Face(face), other.componentCount);
          break;
        case GLDataType::HALF  :
          setData(other.hdata, other.width, other.height, Face(face), other.componentCount);
          break;
        case GLDataType::FLOAT :
          setData(other.fdata, other.width, other.height, Face(face), other.componentCount);
          break;
      }
    }
  }
  return *this;
}

const GLint GLTextureCube::getId() const {
  return id;
}

void GLTextureCube::clear() {
  setEmpty(width,height,componentCount,dataType);
}

void GLTextureCube::setData(const Image& image, Face face) {
  this->data = image.data;
  setData((GLvoid*)(image.data.data()), image.width, image.height, face, image.componentCount, GLDataType::BYTE);
}

void GLTextureCube::setData(const std::vector<GLubyte>& data, Face face) {
  setData(data,width,height,face,componentCount);
}

void GLTextureCube::setData(const std::vector<GLfloat>& data, Face face) {
  setData(data,width,height,face,componentCount);
}

void GLTextureCube::setEmpty(uint32_t width, uint32_t height, uint32_t componentCount, GLDataType dataType) {
  for (size_t face = 0;face<6;++face) {
    switch (dataType) {
      case GLDataType::BYTE  : setData(std::vector<GLubyte>(width*height*componentCount), width, height, Face(face), componentCount); break;
      case GLDataType::HALF  : setData(std::vector<GLhalf>(width*height*componentCount), width, height, Face(face), componentCount); break;
      case GLDataType::FLOAT : setData(std::vector<GLfloat>(width*height*componentCount), width, height, Face(face), componentCount); break;
    }
  }
}

void GLTextureCube::setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, Face face, uint32_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texure dimensions do not match."};
  }
  
  this->data = data;
  setData((GLvoid*)data.data(), width, height, face, componentCount, GLDataType::BYTE);
}

void GLTextureCube::setData(const std::vector<GLhalf>& data, uint32_t width, uint32_t height, Face face, uint32_t componentCount) {
  if (data.size() != componentCount*width*height) {
    throw GLException{"Data size and texure dimensions do not match."};
  }
  
  this->hdata = data;
  setData((GLvoid*)data.data(), width, height, face, componentCount, GLDataType::HALF);
}

void GLTextureCube::setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, Face face, uint32_t componentCount) {
  if (data.size() != componentCount*width*height) {
    std::stringstream ss;
    ss << "Data size " << data.size() << " and texure dimensions " << componentCount << "*" << width
       << "*" << height << "=" << componentCount*width*height << " do not match.";
    throw GLException{ss.str()};
  }
  
  this->fdata = data;
  setData((GLvoid*)this->fdata.data(), width, height, face, componentCount, GLDataType::FLOAT);
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

void GLTextureCube::setData(GLvoid* data, uint32_t width, uint32_t height,
                            Face face, uint32_t componentCount,
                            GLDataType dataType) {
  if (this->width != 0 || this->height != 0) {
    if (this->dataType != dataType ||
        this->width != width ||
        this->height != height ||
        this->componentCount != componentCount) {
      throw GLException{"Texture dimensions do not match."};
    }
  }

  this->dataType = dataType;
  this->width = width;
  this->height = height;
  this->componentCount = componentCount;

  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));

  GL(glPixelStorei(GL_PACK_ALIGNMENT ,1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT ,1));

  std::array<GLenum,3> format = dataTypeToGL(dataType, componentCount);

  GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + int(face), 0, format[1], GLuint(width), GLuint(height), 0, format[2], format[0], data));
}

void GLTextureCube::generateMipmap() {
  GL(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
  GL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
}
