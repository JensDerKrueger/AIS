#include <fstream>
#include <iostream>
#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string_view>

#include "bmp.h"

namespace BMP {
  bool save(const std::string& filename, const Image& source, bool ignoreSize) {
      return save(filename, source.width, source.height,
                  source.data, source.componentCount, ignoreSize);
  }

  static uint8_t floatToByte(float x) {  return uint8_t(x*255); }

  bool save(const std::string& filename, uint32_t w, uint32_t h,
            const std::vector<float>& data, uint8_t iComponentCount,
            bool ignoreSize) {
    
    std::vector<uint8_t> byteData(data.size());
    std::transform(data.begin(), data.end(), byteData.begin(), floatToByte);
    
    return save(filename, w, h, byteData, iComponentCount, ignoreSize);
  }

  bool save(const std::string& filename, uint32_t w, uint32_t h,
            const std::vector<uint8_t>& data, uint8_t iComponentCount,
            bool ignoreSize) {
    
    std::ofstream outStream(filename.c_str(), std::ofstream::binary);
    if (!outStream.is_open()) return false;
    
    // write BMP-Header
    outStream.write((char*)"BM", 2); // all BMP-Files start with "BM"
    uint32_t header[3];
    int64_t rowPad= 4-((w*8*iComponentCount)%32)/8;
    if (rowPad == 4) rowPad = 0;
    
    // filesize = 54 (header) + sizeX * sizeY * numChannels
    size_t filesize = 54+size_t(w)*size_t(h)*size_t(iComponentCount)+size_t(rowPad)*size_t(h);
    
    if (!ignoreSize && uint32_t(filesize) != filesize)
      throw BMPException("File to big for BMP format");
    
    header[0] = uint32_t(filesize);
    header[1] = 0;						      // reserved = 0 (4 Bytes)
    
    
    header[2] = 54;						      // File offset to Raster Data
    outStream.write((char*)header, 4*3);
    // write BMP-Info-Header
    uint32_t infoHeader[10];
    infoHeader[0] = 40;	          // size of info header
    infoHeader[1] = w;            // Bitmap Width
    infoHeader[2] = h;//uint32_t(-(int32_t)h);           // Bitmap Height (negative to flip image)
    infoHeader[3] = 1+65536*8*iComponentCount;
    // first 2 bytes=Number of Planes (=1)
    // next  2 bytes=BPP
    infoHeader[4] = 0;				  	// compression (0 = none)
    infoHeader[5] = 0;					  // compressed file size (0 if no compression)
    infoHeader[6] = 11810;				// horizontal resolution: Pixels/meter (11810 = 300 dpi)
    infoHeader[7] = 11810;				// vertical resolution: Pixels/meter (11810 = 300 dpi)
    infoHeader[8] = 0;					  // Number of actually used colors
    infoHeader[9] = 0;					  // Number of important colors  0 = all
    outStream.write((char*)infoHeader, 4*10);
    
    // data in BMP is stored BGR, so convert scalar BGR
    const size_t totalSize = size_t(iComponentCount)*size_t(w)*size_t(h);
    std::vector<uint8_t> pData(totalSize);
    
    size_t sourceIndex = 0;
    size_t index = 0;
    for (size_t y = 0;y<h;++y) {
      for (size_t x = 0;x<w;++x) {
        
        uint8_t r = data[sourceIndex++];
        uint8_t g = data[sourceIndex++];
        uint8_t b = data[sourceIndex++];
        
        pData[index++] = b;
        pData[index++] = g;
        pData[index++] = r;
        if (iComponentCount==4) {
            uint8_t a = data[sourceIndex++];
            pData[index++] = a;
        }
      }
    }
    
    // write data (pad if necessary)
    if (rowPad==0) {
        outStream.write((char*)pData.data(), totalSize);
    }
    else {
      uint8_t zeroes[9]={0,0,0,0,0,0,0,0,0};
      for (size_t i=0; i<h; i++) {
        outStream.write((char*)&(pData[iComponentCount*i*w]), size_t(iComponentCount)*size_t(w));
        outStream.write((char*)zeroes, rowPad);
      }
    }
    
    outStream.close();
    return true;
  }

  Image load(const std::string& filename) {
    Image texture;
    
    // make sure file exists.
    std::ifstream file(filename.c_str(), std::ifstream::binary);
    if (!file.is_open()) {
      std::stringstream s;
      s << "Can't open BMP file " << filename;
      throw BMPException(s.str());
    }
    // make sure file can be read
    uint16_t bfType;
    if(!file.read((char*)&bfType, sizeof(short int)))
      throw BMPException("File could not be read");
    // check if file is a bitmap
    if (bfType != 19778)
      throw BMPException("Not a BMP file");
    // get the file size
    // skip file size and reserved fields of bitmap file header
    file.seekg(8, std::ios_base::cur);
    // get the position of the actual bitmap data
    int32_t bfOffBits;
    if (!file.read((char*)&bfOffBits, sizeof(int32_t)))
      throw BMPException("Bitmap offset could not be read");

    file.seekg(4, std::ios_base::cur);                   // skip size of bitmap info header
    file.read((char*)&texture.width, sizeof(int32_t));   // get the width of the bitmap
    
    int32_t height;
    file.read((char*)&height, sizeof(int32_t));  // get the height of the bitmap
    texture.height = abs(height);
    
    int16_t biPlanes;
    file.read((char*)&biPlanes, sizeof(int16_t));   // get the number of planes
      
    if (biPlanes != 1)
      throw BMPException("Number of bitplanes was not equal to 1\n");
    // get the number of bits per pixel
    int16_t biBitCount;
    if (!file.read((char*)&biBitCount, sizeof(int16_t)))
      throw BMPException("Error Reading file\n");
      
    // calculate the size of the image in bytes
    int32_t biSizeImage;
    if (biBitCount == 8 || biBitCount == 16 || biBitCount == 24 || biBitCount == 32) {
      biSizeImage = texture.width * texture.height * biBitCount/8;
      texture.componentCount = biBitCount/8;
    } else {
      std::stringstream s;
      s << "File is " << biBitCount << " bpp, but this reader only supports 8, 16, 24, or 32 Bpp";
      throw BMPException(s.str());
    }
    texture.data.resize(biSizeImage);
    
    int rowPad= 4-((texture.width*8*texture.componentCount)%32)/8;
    if (rowPad == 4) rowPad = 0;
    
    // seek to the actual data
    file.seekg(bfOffBits, std::ios_base::beg);
    
    if (rowPad == 0) {
      file.read((char*)texture.data.data(), biSizeImage);
      if (!file)
        throw BMPException("Error loading file");
    } else {
      for (uint32_t y = 0;y<texture.height;++y) {
        file.read((char*)texture.data.data()+y*texture.width*biBitCount/8, texture.width*biBitCount/8);
        file.seekg(rowPad, std::ios_base::cur);
        if (!file)
          throw BMPException("Error loading file");
      }
    }
    
    file.close();
    
    // swap red and blue (bgr -> rgb)
    if (texture.componentCount > 2) {
      for (int32_t i = 0; i < biSizeImage; i += texture.componentCount) {
        const uint8_t temp = texture.data[i];
        texture.data[i] = texture.data[i + 2];
        texture.data[i + 2] = temp;
      }
    }
    
    if (height < 0)
      return texture.flipHorizontal();
    else
      return texture;
  }

  void blit(const Image& source, const Vec2ui& rawSourceStart, const Vec2ui& rawSourceEnd,
            Image& target, const Vec2ui& targetStart, bool skipChecks) {
    
    Vec2ui sourceStart{rawSourceStart.x > rawSourceEnd.x ? rawSourceEnd.x : rawSourceStart.x,
                      rawSourceStart.y > rawSourceEnd.y ? rawSourceEnd.y : rawSourceStart.y};
    Vec2ui sourceEnd{rawSourceStart.x > rawSourceEnd.x ? rawSourceStart.x : rawSourceEnd.x,
                    rawSourceStart.y > rawSourceEnd.y ? rawSourceStart.y : rawSourceEnd.y};
    
    if (!skipChecks) {
      if (target.componentCount != source.componentCount) {
        std::stringstream s;
        s << "blit requires images with equal component count " << source.componentCount << " != " << target.componentCount;
        throw BMPException(s.str());
      }
                  
      if (sourceEnd.x > source.width || sourceEnd.y > source.height) {
        std::stringstream s;
        s << "blit source region out of bounds (w=" << source.width << " h=" << source.height << " x=" << sourceEnd.x << " y=" << sourceEnd.y << ")";
        throw BMPException(s.str());
      }

      Vec2ui blitSize = sourceEnd-sourceStart;
      if (targetStart.x + blitSize.x > target.width ||
          targetStart.y + blitSize.y > target.height) {

        Vec2ui newSize{(target.width >= targetStart.x + blitSize.x) ? target.width : (targetStart.x + blitSize.x),
                      (target.height >= targetStart.y + blitSize.y) ? target.height : (targetStart.y + blitSize.y)};
        
        Image tmp;
        tmp.width = newSize.x;
        tmp.height = newSize.y;
        tmp.componentCount = source.componentCount;
        tmp.data.resize(tmp.width*tmp.height*tmp.componentCount);
        
        blit(target,{0,0},{target.width,target.height},tmp,{0,0},true);
        
        target.width = tmp.width;
        target.height = tmp.height;
        target.data = tmp.data;
      }
    }
        
    for (uint32_t y = sourceStart.y;y < sourceEnd.y;++y) {
      for (uint32_t x = sourceStart.x;x < sourceEnd.x;++x) {
        for (uint32_t c = 0;c<target.componentCount;++c) {
            target.setValue(targetStart.x+x-sourceStart.x,targetStart.y+y-sourceStart.y,c,source.getValue(x,y,c));
        }
      }
    }
  }
}
