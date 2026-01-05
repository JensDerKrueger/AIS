#pragma once

#include <vector>
#include <string>
#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"
#include "GLTexture2D.h"
#include "GLTexture3D.h"
#include "GLDepthTexture.h"
#ifndef __EMSCRIPTEN__
#include "GLTexture1D.h"
#include "GLTextureCube.h"
#endif

/**
 * @file GLProgram.h
 * @brief RAII wrapper around an OpenGL shader program with rich uniform/texture setters.
 *
 * Declares utilities to compile and link shader programs from files or strings
 * (vertex/fragment/optional geometry), query uniform/attribute locations, set a
 * wide variety of uniform types (scalars, vectors, matrices, arrays), and bind
 * textures (1D/2D/3D/depth/cube) to sampler uniforms.
 *
 * @note All GL calls in the implementation are routed through a \c GL() macro
 *       (see \c GLDebug.h). Matrix uploads invert the transpose flag to account
 *       for the row/column-major differences in this code base.
 */

/**
 * @brief Exception thrown for program/shader I/O or linkage failures.
 */
class ProgramException : public std::exception {
public:
  /**
   * @brief Construct with an explanatory message.
   * @param whatStr Human‑readable description.
   */
  ProgramException(const std::string& whatStr) : whatStr(whatStr) {}
  /**
   * @brief Retrieve the explanatory C‑string.
   */
  virtual const char* what() const throw() {
    return whatStr.c_str();
  }
private:
  std::string whatStr; ///< Stored message.
};

/**
 * @brief OpenGL shader program with helpers for creation and data binding.
 */
class GLProgram {
public:
  /** @brief Destroy and delete owned GL shader objects and program. */
  ~GLProgram();

  /**
   * @name Factory helpers
   * @brief Create a program from multiple shader sources (files or strings).
   */
  ///@{
  /**
   * @brief Build from lists of filenames per stage.
   * @param vs Vertex shader files (each is a full shader or snippet).
   * @param fs Fragment shader files.
   * @param gs Geometry shader files (optional; may contain empty strings).
   * @throw ProgramException on file read errors.
   */
  static GLProgram createFromFiles(const std::vector<std::string>& vs, const std::vector<std::string>& fs,
                                   const std::vector<std::string>& gs = std::vector<std::string>());
  /**
   * @brief Build directly from in‑memory source strings per stage.
   * @param vs Vertex shader source strings.
   * @param fs Fragment shader source strings.
   * @param gs Geometry shader source strings (optional; empty strings ignored).
   */
  static GLProgram createFromStrings(const std::vector<std::string>& vs, const std::vector<std::string>& fs,
                                     const std::vector<std::string>& gs = std::vector<std::string>());

  /**
   * @brief Convenience overload for a single file per stage.
   */
  static GLProgram createFromFile(const std::string& vs, const std::string& fs, const std::string& gs="");
  /**
   * @brief Convenience overload for a single source string per stage.
   */
  static GLProgram createFromString(const std::string& vs, const std::string& fs, const std::string& gs="");
  ///@}

  /** @brief Copy‑construct a new program with duplicated shaders and linkage. */
  GLProgram(const GLProgram& other);
  /** @brief Copy‑assign, releasing current GL objects and recreating from other. */
  GLProgram& operator=(const GLProgram& other);

  /**
   * @name Introspection
   */
  ///@{
  /**
   * @brief Query an attribute location by name.
   * @param id Attribute identifier in the shader.
   * @return Location index (>=0) or throws if not found.
   * @throw ProgramException when the attribute cannot be located.
   */
  GLint getAttributeLocation(const std::string& id) const;
  /**
   * @brief Query a uniform location by name.
   * @param id Uniform identifier in the shader.
   * @return Location index (>=0) or throws if not found.
   * @throw ProgramException when the uniform cannot be located.
   */
  GLint getUniformLocation(const std::string& id) const;
  ///@}

  /**
   * @name Uniform setters (by name)
   * @brief Upload scalar, vector, and matrix values.
   */
  ///@{
  void setUniform(const std::string& id, float value) const;
  void setUniform(const std::string& id, const Vec2& value) const;
  void setUniform(const std::string& id, const Vec3& value) const;
  void setUniform(const std::string& id, const Vec4& value) const;
  void setUniform(const std::string& id, int value) const;
  void setUniform(const std::string& id, const Vec2i& value) const;
  void setUniform(const std::string& id, const Mat4& value, bool transpose=false) const;
  ///@}

#ifndef __EMSCRIPTEN__
  /** @name Texture setters (by name) */
  ///@{
  void setTexture(const std::string& id, const GLTexture1D& texture, GLenum unit=0) const;
  void setTexture(const std::string& id, const GLTextureCube& texture, GLenum unit=0) const;
  ///@}
#endif
  /** @name Texture setters (by name) */
  ///@{
  void setTexture(const std::string& id, const GLDepthTexture& texture, GLenum unit=0) const;
  void setTexture(const std::string& id, const GLTexture2D& texture, GLenum unit=0) const;
  void setTexture(const std::string& id, const GLTexture3D& texture, GLenum unit=0) const;
  ///@}

  /**
   * @name Uniform setters (by location)
   * @brief Upload scalar, vector, matrix values and arrays using a known location.
   */
  ///@{
  void setUniform(GLint id, float value) const;
  void setUniform(GLint id, const Vec2& value) const;
  void setUniform(GLint id, const Vec3& value) const;
  void setUniform(GLint id, const Vec4& value) const;
  void setUniform(GLint id, int value) const;
  void setUniform(GLint id, const Vec2i& value) const;
  void setUniform(GLint id, const Vec3i& value) const;
  void setUniform(GLint id, const Vec4i& value) const;
  void setUniform(GLint id, const Mat4& value, bool transpose=false) const;

  void setUniform(GLint id, const std::vector<float>& value) const;
  void setUniform(GLint id, const std::vector<Vec2>& value) const;
  void setUniform(GLint id, const std::vector<Vec3>& value) const;
  void setUniform(GLint id, const std::vector<Vec4>& value) const;
  void setUniform(GLint id, const std::vector<int>& value) const;
  void setUniform(GLint id, const std::vector<Vec2i>& value) const;
  void setUniform(GLint id, const std::vector<Vec3i>& value) const;
  void setUniform(GLint id, const std::vector<Vec4i>& value) const;
  void setUniform(GLint id, const std::vector<Mat4>& value, bool transpose=false) const;
  ///@}

#ifndef __EMSCRIPTEN__
  /** @name Texture setters (by location) */
  ///@{
  void setTexture(GLint id, const GLTexture1D& texture, GLenum unit=0) const;
  void setTexture(GLint id, const GLTextureCube& texture, GLenum unit=0) const;
  ///@}
#endif
  /** @name Texture setters (by location) */
  ///@{
  void setTexture(GLint id, const GLDepthTexture& texture, GLenum unit=0) const;
  void setTexture(GLint id, const GLTexture2D& texture, GLenum unit=0) const;
  void setTexture(GLint id, const GLTexture3D& texture, GLenum unit=0) const;
  ///@}

#ifndef __EMSCRIPTEN__
  /** @brief Unbind any 1D texture from texture unit \p unit. */
  void unsetTexture1D(GLenum unit) const;
#endif
  /** @brief Unbind any 2D texture from texture unit \p unit. */
  void unsetTexture2D(GLenum unit) const;
  /** @brief Unbind any 3D texture from texture unit \p unit. */
  void unsetTexture3D(GLenum unit) const;

  /** @brief Bind this program for subsequent draw calls (glUseProgram). */
  void enable() const;
  /** @brief Unbind any program (glUseProgram(0)). */
  void disable() const;

private:
  GLuint glVertexShader;   ///< Compiled vertex shader name (0 if none).
  GLuint glFragmentShader; ///< Compiled fragment shader name (0 if none).
  GLuint glGeometryShader; ///< Compiled geometry shader name (0 if none).
  GLuint glProgram;        ///< Linked program name.

  std::vector<std::string> vertexShaderStrings;   ///< Source strings used to build the vertex shader.
  std::vector<std::string> fragmentShaderStrings; ///< Source strings used to build the fragment shader.
  std::vector<std::string> geometryShaderStrings; ///< Source strings used to build the geometry shader.

  /** @brief Load a text file completely into a string (throws on failure). */
  static std::string loadFile(const std::string& filename);

  /** @brief Create and compile a shader of \p type from \p count strings. */
  static GLuint createShader(GLenum type, const GLchar** src, GLsizei count);

  /** @brief Private ctor used by the factory helpers. */
  GLProgram(std::vector<std::string> vertexShaderStrings, std::vector<std::string> fragmentShaderStrings, std::vector<std::string> geometryShaderStrings);

  /** @brief Build and link GL objects from source vectors. */
  void programFromVectors(std::vector<std::string> vs, std::vector<std::string> fs, std::vector<std::string> gs);
};
