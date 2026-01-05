#pragma once

#include <ostream>
#include <string>
#include <array>

#include "Vec2.h"
#include "Vec3.h"

/**
 * @file Vec4.h
 * @brief 4D vector template with arithmetic, length helpers, and utilities.
 *
 * Defines a compact 4D vector type template, \c Vec4t<T>, storing four
 * components with multiple alias views (x/y/z/w and r/g/b/a) and sub-vectors
 * (xy, xyz). Provides component-wise and scalar arithmetic, length helpers,
 * dot product, normalization, clamping, and random generation.
 *
 * @note This type is implemented as a \c union with several member layouts to
 *       allow ergonomic access (e.g., \c v.x vs. \c v.e[0]). Access only one
 *       active view at a time.
 */

template <typename T>
union Vec4t {
public:
  /** @brief Component storage and named aliases. */
  std::array<T, 4> e;          ///< Indexed access: e[0], e[1], e[2], e[3].
  struct { T x; T y; T z; T w;}; ///< Cartesian aliases for components.
  struct { T r; T g; T b; T a;}; ///< Color aliases for components.
  Vec2t<T> xy;                 ///< 2D view of the first two components.
  Vec3t<T> xyz;                ///< 3D view of the first three components.

  /** @brief Default-construct to (0,0,0,0). */
  Vec4t():
  e{0,0,0,0}
  {}

  /**
   * @brief Construct from four component values.
   * @param x First component.
   * @param y Second component.
   * @param z Third component.
   * @param w Fourth component.
   */
  Vec4t(T x, T y, T z, T w):
  e{x,y,z,w}
  {}

  /**
   * @brief Copy-construct from the same type.
   * @param other Source vector to copy.
   */
  Vec4t(const Vec4t& other) :
  e{other.x, other.y, other.z, other.w}
  {}

  /**
   * @brief Construct from a 3D vector and a w value.
   * @param other Source 3D vector for x, y, z.
   * @param w     Fourth component.
   */
  Vec4t(const Vec3t<T>& other, T w):
  e{other.x, other.y, other.z, w}
  {}

  /**
   * @brief Construct from a 2D vector and z, w values.
   * @param other Source 2D vector for x and y.
   * @param z     Third component.
   * @param w     Fourth component.
   */
  Vec4t(const Vec2t<T>& other, T z, T w) :
  e{other.x, other.y, z, w}
  {}

  /**
   * @brief Explicitly convert from another \c Vec4t with different component type.
   * @tparam U Source component type.
   * @param other Source vector to convert from.
   */
  template <typename U>
  explicit Vec4t(const Vec4t<U>& other) :
  e{other.x, other.y, other.z, other.w}
  {}

  /**
   * @brief Explicitly convert from a 3D vector of a different type and a w value.
   * @tparam U Source component type.
   * @param other Source 3D vector for x, y, z.
   * @param w     Fourth component.
   */
  template <typename U>
  explicit Vec4t(const Vec3t<U>& other, T w):
  e{other.x, other.y, other.z, w}
  {}

  /**
   * @brief Explicitly convert from a 2D vector of a different type and z, w values.
   * @tparam U Source component type.
   * @param other Source 2D vector for x and y.
   * @param z     Third component.
   * @param w     Fourth component.
   */
  template <typename U>
  explicit Vec4t(const Vec2t<U>& other, T z, T w) :
  e{other.x, other.y, z, w}
  {}

  /**
   * @brief String representation "[x, y, z, w]".
   * @return A new std::string describing the vector.
   */
  const std::string toString() const {
    std::stringstream s;
    s << "[" << e[0] << ", " << e[1] << ", " << e[2] << ", " << e[3] << "]";
    return s.str();
  }

  /**
   * @brief Component-wise addition.
   * @param val Vector to add.
   * @return Result of this + val.
   */
  Vec4t operator+(const Vec4t& val) const {
    return {e[0]+val.e[0],
      e[1]+val.e[1],
      e[2]+val.e[2],
      e[3]+val.e[3]};
  }

  /**
   * @brief Component-wise subtraction.
   * @param val Vector to subtract.
   * @return Result of this - val.
   */
  Vec4t operator-(const Vec4t& val) const {
    return {e[0]-val.e[0],
      e[1]-val.e[1],
      e[2]-val.e[2],
      e[3]-val.e[3]};
  }

  /**
   * @brief Component-wise multiplication.
   * @param val Vector to multiply component-wise.
   * @return Result of this * val.
   */
  Vec4t operator*(const Vec4t& val) const {
    return {e[0]*val.e[0],
      e[1]*val.e[1],
      e[2]*val.e[2],
      e[3]*val.e[3]};
  }

  /**
   * @brief Component-wise division.
   * @param val Vector to divide by component-wise.
   * @return Result of this / val.
   */
  Vec4t operator/(const Vec4t& val) const {
    return {e[0]/val.e[0],
      e[1]/val.e[1],
      e[2]/val.e[2],
      e[3]/val.e[3]};
  }

  /**
   * @brief Scalar multiplication.
   * @param val Scalar factor.
   * @return Result of this * val.
   */
  Vec4t operator*(const T& val) const {
    return {e[0]*val, e[1]*val, e[2]*val, e[3]*val};
  }

  /**
   * @brief Scalar division.
   * @param val Scalar divisor.
   * @return Result of this / val.
   */
  Vec4t operator/(const T& val) const {
    return {e[0]/val, e[1]/val, e[2]/val, e[3]/val};
  }

  /**
   * @brief Add scalar to all components.
   * @param val Scalar to add.
   * @return Result of this + val.
   */
  Vec4t operator+(const T& val) const {
    return {e[0]+val,e[1]+val,e[2]+val,e[3]+val};
  }

  /**
   * @brief Subtract scalar from all components.
   * @param val Scalar to subtract.
   * @return Result of this - val.
   */
  Vec4t operator-(const T& val) const{
    return {e[0]-val,e[1]-val,e[2]-val,e[3]-val};
  }

  /**
   * @brief Equality comparison (component-wise equality).
   * @param other Vector to compare against.
   * @return True iff all components are equal.
   */
  bool operator == ( const Vec4t& other ) const {
    return e[0] == other.e[0] &&
    e[1] == other.e[1] &&
    e[2] == other.e[2] &&
    e[3] == other.e[3];
  }

  /**
   * @brief Inequality comparison (component-wise inequality).
   * @param other Vector to compare against.
   * @return True if any component differs.
   */
  bool operator != ( const Vec4t& other ) const {
    return e[0] != other.e[0] ||
    e[1] != other.e[1] ||
    e[2] != other.e[2] ||
    e[3] != other.e[3];
  }

  /**
   * @brief Euclidean length (magnitude).
   * @return sqrt(x*x + y*y + z*z + w*w).
   */
  T length() const {
    return sqrt(sqlength());
  }

  /**
   * @brief Squared length (avoids sqrt).
   * @return x*x + y*y + z*z + w*w.
   */
  T sqlength() const {
    return e[0]*e[0]+e[1]*e[1]+e[2]*e[2]+e[3]*e[3];
  }

  /**
   * @brief Drop the last component and return a 3D vector (x,y,z).
   * @return {x, y, z}.
   */
  Vec3t<T> vec3() const {
    return {e[0], e[1], e[2]};
  }

  /**
   * @brief Implicit pointer conversion to raw component storage.
   * @return Pointer to first component (non-const overload).
   * @warning Raw pointer exposes internal layout; use with care.
   */
  operator T*(void) {return e.data();}

  /**
   * @brief Implicit pointer conversion to raw component storage (const).
   * @return Pointer to first component (const overload).
   */
  operator const T*(void) const  {return e.data();}

  /**
   * @brief Dot product of two 4D vectors.
   * @param a First vector.
   * @param b Second vector.
   * @return a·b.
   */
  static T dot(const Vec4t& a, const Vec4t& b) {
    return a.e[0]*b.e[0]+a.e[1]*b.e[1]+a.e[2]*b.e[2]+a.e[3]*b.e[3];
  }

  /**
   * @brief Return the normalized (unit-length) version of a vector.
   * @param a Input vector.
   * @return a / |a|.
   * @note If |a| == 0 the behavior follows the underlying division semantics.
   */
  static Vec4t normalize(const Vec4t& a) {
    return a/a.length();
  }

  /**
   * @brief Create a vector with components sampled from [0,1).
   * @return Random vector (x,y,z,w) in [0,1) each.
   * @note Depends on a global/namespace-scope RNG named \c staticRand
   *       provided by @c Rand.h .
   */
  static Vec4t<float> random() {
    return {staticRand.rand01(),staticRand.rand01(),staticRand.rand01(),staticRand.rand01()};
  }

  /**
   * @brief Clamp each component to [minVal, maxVal].
   * @param val    Input vector.
   * @param minVal Lower bound.
   * @param maxVal Upper bound.
   * @return New vector with components clamped to the given range.
   */
  static Vec4t<float> clamp(const Vec4t& val, float minVal, float maxVal) {
    return { std::clamp(val.x, minVal, maxVal),
      std::clamp(val.y, minVal, maxVal),
      std::clamp(val.z, minVal, maxVal),
      std::clamp(val.w, minVal, maxVal)
    };
  }
};

/**
 * @brief Stream insertion for string representation of a vector.
 * @tparam T Component type.
 * @param os Output stream.
 * @param v  Vector to print.
 * @return Reference to the output stream.
 */
template <typename T>
std::ostream & operator<<(std::ostream & os, const Vec4t<T> & v) {
  os << v.toString();
  return os;
}

/**
 * @brief Scalar multiplication with scalar on the left.
 * @tparam T Component type of vector.
 * @param scalar Scalar factor.
 * @param vec    Vector to multiply.
 * @return Result of vec * scalar.
 */
template <typename T>
Vec4t<T> operator*(float scalar, const Vec4t<T>& vec) {
  return vec * scalar;
}

/** @brief 4D float vector (x,y,z,w). */
typedef Vec4t<float> Vec4;
/** @brief 4D 32-bit signed integer vector (x,y,z,w). */
typedef Vec4t<int32_t> Vec4i;
/** @brief 4D 32-bit unsigned integer vector (x,y,z,w). */
typedef Vec4t<uint32_t> Vec4ui;
