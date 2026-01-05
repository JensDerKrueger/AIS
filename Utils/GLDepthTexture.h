#pragma once

#include <vector>

#include "GLEnv.h"

/**
 * @file GLDepthTexture.h
 * @brief RAII wrapper for a 2D OpenGL depth texture with compare mode enabled.
 *
 * Manages the lifetime of a `GL_TEXTURE_2D` storing depth values and configures
 * common sampler state (wrap, filters, and comparison mode). Storage can be
 * allocated in 16/24/32‑bit depth formats via @ref setEmpty().
 *
 * @note The internal depth format is selected by ::GLDepthDataType (declared in
 *       `GLEnv.h`). All GL calls are wrapped with the `GL()` macro for debug
 *       error checking.
 */
class GLDepthTexture {
public:
  /**
   * @brief Create a depth texture with initial sampler parameters.
   * @param magFilter Magnification filter (e.g., `GL_LINEAR`).
   * @param minFilter Minification filter (e.g., `GL_LINEAR`).
   * @param wrapX     Wrap mode for S (x) coordinate (e.g., `GL_CLAMP_TO_EDGE`).
   * @param wrapY     Wrap mode for T (y) coordinate (e.g., `GL_CLAMP_TO_EDGE`).
   * @post A texture name is generated, parameters are applied, and depth
   *       comparison is enabled (`GL_COMPARE_REF_TO_TEXTURE` with `GL_LESS`).
   */
  GLDepthTexture(GLint magFilter=GL_LINEAR, GLint minFilter=GL_LINEAR,
                 GLint wrapX=GL_CLAMP_TO_EDGE, GLint wrapY=GL_CLAMP_TO_EDGE) :
  id{ 0 },
  width{ 0 },
  height{ 0 },
  dataType{ GLDepthDataType::DEPTH24 }
  {
    GL(glGenTextures(1, &id));
    GL(glBindTexture(GL_TEXTURE_2D, id));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS));
  }

  /** @brief Destroy and delete the GL texture name. */
  ~GLDepthTexture() {
    GL(glDeleteTextures(1, &id));
  }

  /**
   * @brief Retrieve the OpenGL texture object name.
   * @return GLuint of the managed depth texture.
   */
  const GLuint getId() const {return id;}

  /**
   * @brief Allocate empty depth storage of the requested size/format.
   * @param width   Width in texels.
   * @param height  Height in texels.
   * @param dataType Depth format to allocate (DEPTH16/DEPTH24/DEPTH32).
   * @post Internal storage is defined at level 0; pixel store alignment is set
   *       to 1 for pack/unpack.
   */
  void setEmpty(uint32_t width, uint32_t height,
                GLDepthDataType dataType=GLDepthDataType::DEPTH32) {
    this->width = width;
    this->height = height;

    GL(glBindTexture(GL_TEXTURE_2D, id));
    GL(glPixelStorei(GL_PACK_ALIGNMENT ,1));
    GL(glPixelStorei(GL_UNPACK_ALIGNMENT ,1));

    switch (dataType) {
      case GLDepthDataType::DEPTH16:
        GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, GLsizei(width),
                        GLsizei(height), 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0));
        break;
      case GLDepthDataType::DEPTH24:
        GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, GLsizei(width),
                        GLsizei(height), 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0));
        break;
      case GLDepthDataType::DEPTH32:
        GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, GLsizei(width),
                        GLsizei(height), 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0));
        break;
    }
  }

  /** @name Introspection */
  ///@{
  /** @brief Face height in texels. */
  uint32_t getHeight() const {return height;}
  /** @brief Face width in texels. */
  uint32_t getWidth() const {return width;}
  /** @brief Depth storage type used for allocation. */
  GLDepthDataType getType() const {return dataType;}
  ///@}

  /**
   * @brief Update the filtering parameters on the bound texture.
   * @param magFilter Magnification filter.
   * @param minFilter Minification filter.
   */
  void setFilter(GLint magFilter, GLint minFilter) {
    GL(glBindTexture(GL_TEXTURE_2D, id));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
  }

private:
  GLuint id;                ///< GL name of the texture object.
  uint32_t width;           ///< Texture width in texels.
  uint32_t height;          ///< Texture height in texels.
  GLDepthDataType dataType; ///< Internal depth format (DEPTH16/DEPTH24/DEPTH32).
};
