#pragma once

#include <vector>

#include "GLEnv.h"
#include "Image.h"

class GLTexture2D {
public:
	GLTexture2D(GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT);
	~GLTexture2D();
	
  GLTexture2D(const Image& image,
              GLint magFilter=GL_NEAREST, GLint minFilter=GL_NEAREST,
              GLint wrapX=GL_REPEAT, GLint wrapY=GL_REPEAT);
  GLTexture2D(const GLTexture2D& other);
  GLTexture2D& operator=(const GLTexture2D& other);
    
	const GLint getId() const;
  void clear();
  void setEmpty(uint32_t width, uint32_t height, uint32_t componentCount, GLDataType dataType=GLDataType::BYTE);
  void setData(const Image& image);
  void setData(const std::vector<GLubyte>& data, uint32_t width, uint32_t height, uint32_t componentCount=4);
  void setData(const std::vector<GLubyte>& data);
  void setData(const std::vector<GLfloat>& data, uint32_t width, uint32_t height, uint32_t componentCount=4);
  void setData(const std::vector<GLfloat>& data);
  void setData(const std::vector<GLhalf>& data, uint32_t width, uint32_t height, uint32_t componentCount=4);
  void setData(const std::vector<GLhalf>& data);
  void setFilter(GLint magFilter, GLint minFilter);
  
  void setPixel(const std::vector<GLubyte>& data, uint32_t x, uint32_t y);
  
  void generateMipmap();
  
  uint32_t getHeight() const {return height;}
  uint32_t getWidth() const {return width;}
  uint32_t getComponentCount() const {return componentCount;}
  uint32_t getSize() const {return height*width*componentCount;}
  GLDataType getType() const {return dataType;}
  
  Image getImage();
  const std::vector<GLubyte>& getDataByte();
  const std::vector<GLhalf>& getDataHalf();
  const std::vector<GLfloat>& getDataFloat();
  
private:
	GLuint id;
	GLenum internalformat;
	GLenum format;
	GLenum type;

  GLint magFilter;
  GLint minFilter;
  GLint wrapX;
  GLint wrapY;
  std::vector<GLubyte> data;
  std::vector<GLhalf> hdata;
  std::vector<GLfloat> fdata;
  uint32_t width;
  uint32_t height;
  uint32_t componentCount;
  GLDataType dataType;
  
  void setData(GLvoid* data, uint32_t width, uint32_t height, uint32_t componentCount, GLDataType dataType);
};
