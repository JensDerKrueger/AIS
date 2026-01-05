#pragma once

#include <stdint.h>
#include <vector>
#include <string>

#include "Vec4.h"

class Grid2D;

/**
 * @file Image.h
 * @brief Lightweight image container with basic processing utilities.
 *
 * Holds interleaved 8‑bit pixel data for 1–4 channels and provides helpers for
 * value access, alpha generation, grayscale conversion, ASCII art export,
 * simple resampling/cropping, flipping, and convolution filtering.
 *
 * Pixels are stored in row‑major order with contiguous interleaved components
 * per pixel. Component semantics follow RGB(A) when \c componentCount ≥ 3.
 */
class Image {
public:
  /** @name Dimensions and storage */
  ///@{
  uint32_t width;          ///< Image width in pixels.
  uint32_t height;         ///< Image height in pixels.
  uint8_t  componentCount; ///< Number of components per pixel (1–4).
  std::vector<uint8_t> data; ///< Interleaved pixel data of size width*height*componentCount.
  ///@}

  /**
   * @brief Construct a 1×1 RGBA image initialized from a color in [0,1].
   * @param color RGBA color; each component is multiplied by 255 and cast to byte.
   */
  Image(const Vec4& color);

  /**
   * @brief Construct an empty image with given geometry and channel count.
   * @param width  Width in pixels.
   * @param height Height in pixels.
   * @param componentCount Components per pixel (default 4 = RGBA).
   * @post Data buffer is allocated but contents are uninitialized.
   */
  Image(uint32_t width = 100,
        uint32_t height = 100,
        uint8_t componentCount = 4);

  /**
   * @brief Construct from raw data (moves or copies \p data).
   * @param width  Width in pixels.
   * @param height Height in pixels.
   * @param componentCount Components per pixel.
   * @param data   Interleaved pixel data of size width*height*componentCount.
   */
  Image(uint32_t width,
        uint32_t height,
        uint8_t componentCount,
        std::vector<uint8_t> data);

  /**
   * @brief Component‑wise multiply by a color; upgrades RGB→RGBA if needed.
   * @param color Color in [0,1] applied per channel.
   * @details For 3‑channel images, a new alpha channel is added (set to
   *          255*color.a) and \c componentCount becomes 4.
   */
  void multiply(const Vec4& color);

  /**
   * @brief Ensure an alpha channel exists and set it to \p alpha for all pixels.
   * @param alpha Alpha value to assign when creating/filling the A channel.
   * @details If the image has 3 channels, it is converted to RGBA by appending
   *          an alpha channel with the given value.
   */
  void generateAlpha(uint8_t alpha=255);

  /**
   * @brief Create or overwrite alpha from luminance of RGB.
   * @details If the image has 3 channels, converts to RGBA and sets A =
   *          0.299*R + 0.587*G + 0.114*B. If already RGBA, only A is updated.
   */
  void generateAlphaFromLuminance();

  /**
   * @brief Compute flat buffer index for a pixel component.
   * @param x Zero‑based x coordinate.
   * @param y Zero‑based y coordinate.
   * @param component Component index (0..componentCount-1).
   * @return Index into @ref data.
   */
  size_t computeIndex(uint32_t x, uint32_t y, uint8_t component) const;

  /**
   * @brief Read a component value at (x,y).
   * @param x Zero‑based x.
   * @param y Zero‑based y.
   * @param component Component index (0..componentCount-1).
   * @return Byte value.
   */
  uint8_t getValue(uint32_t x, uint32_t y, uint8_t component) const;

  /**
   * @brief Bilinear sample at normalized coordinates in [0,1].
   * @param x Normalized x (0 at left, 1 at right).
   * @param y Normalized y (0 at bottom, 1 at top).
   * @param component Component index (0..componentCount-1).
   * @return Interpolated byte value.
   */
  uint8_t sample(float x, float y, uint8_t component) const;

  /**
   * @brief Luminance from components at (x,y).
   * @return Grayscale value; for 1/2 channels uses first / average, for 3/4
   *         channels uses 0.299R+0.587G+0.114B.
   */
  uint8_t getLumiValue(uint32_t x, uint32_t y) const;

  /**
   * @brief Write a single component at (x,y).
   * @param x,y Pixel coordinates.
   * @param component Component index to write.
   * @param value Byte to store.
   */
  void setValue(uint32_t x, uint32_t y, uint8_t component, uint8_t value);

  /**
   * @brief Set RGB (or first three channels) at (x,y) to a uniform value.
   * @param x,y Pixel coordinates.
   * @param value Byte assigned to channels 0..2.
   */
  void setValue(uint32_t x, uint32_t y, uint8_t value);

  /**
   * @brief Set RGB at (x,y) from a normalized scalar in [0,1].
   * @param x,y Pixel coordinates.
   * @param value Scalar mapped to 0..255 for channels 0..2.
   */
  void setNormalizedValue(uint32_t x, uint32_t y, float value);

  /**
   * @brief Set one component at (x,y) from a normalized scalar in [0,1].
   * @param x,y Pixel coordinates.
   * @param component Component index to write.
   * @param value Scalar mapped to 0..255.
   */
  void setNormalizedValue(uint32_t x, uint32_t y, uint8_t component, float value);

  /**
   * @brief Emit C++ code that reconstructs this image.
   * @param varName Variable name used in the generated snippet.
   * @param padding If true, pad numeric literals for readability.
   * @return A string containing valid C++ initialization code.
   */
  std::string toCode(const std::string& varName="myImage", bool padding=false) const;

  /**
   * @brief Render a coarse ASCII art representation of the image.
   * @param bSmallTable Select a shorter LUT (true) or a more detailed one (false).
   * @return Multiline string with ASCII shading.
   */
  std::string toASCIIArt(bool bSmallTable=true) const;

  /**
   * @brief Convolve image with a small 2D filter kernel (no borders).
   * @param filter Convolution kernel (must expose size and getValue()).
   * @return Filtered image (borders untouched / not written).
   */
  Image filter(const Grid2D& filter) const;

  /**
   * @brief Convert to single‑channel grayscale using luminance.
   * @return Grayscale image with componentCount = 1.
   */
  Image toGrayscale() const;

  /**
   * @brief Generate a colored test pattern (RGB bands + luminance ramp).
   * @param width  Desired width.
   * @param height Desired height.
   * @return RGBA test image.
   */
  static Image genTestImage(uint32_t width,
                            uint32_t height);

  /**
   * @brief Crop a rectangle.
   * @param blX,blY Bottom‑left inclusive pixel.
   * @param trX,trY Top‑right exclusive pixel.
   * @return Cropped image with same component count.
   */
  Image crop(uint32_t blX, uint32_t blY, uint32_t trX, uint32_t trY) const;

  /**
   * @brief Aspect‑preserving resample to a new width.
   * @param newWidth Target width; height is chosen to preserve aspect.
   * @return Resampled image.
   */
  Image resample(uint32_t newWidth) const;

  /**
   * @brief Crop to match target aspect, then downsample to target size.
   * @param newWidth  Target width.
   * @param newHeight Target height.
   * @return Cropped and averaged image.
   */
  Image cropToAspectAndResample(uint32_t newWidth, uint32_t newHeight) const;

  /**
   * @brief Flip rows (vertical mirror around horizontal axis).
   * @return Flipped image with same dimensions.
   */
  Image flipHorizontal() const;

  /**
   * @brief Flip columns (horizontal mirror around vertical axis).
   * @return Flipped image with same dimensions.
   */
  Image flipVertical() const;

private:
  /**
   * @brief Linear interpolation of bytes.
   * @param a First endpoint.
   * @param b Second endpoint.
   * @param alpha Blend factor in [0,1].
   * @return uint8_t(round(a*(1−alpha) + b*alpha)).
   */
  uint8_t linear(uint8_t a, uint8_t b, float alpha) const;
};
