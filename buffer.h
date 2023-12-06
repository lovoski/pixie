#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>
#include <fstream>
#include <iostream>

class Buffer {
  public:
    Buffer(const int width, const int height) {
      allocated = true;
      m_width = width;
      m_height = height;
    }
    Buffer(uint32_t *data, const int width, const int height) {
      allocated = false;
      m_data = data;
      m_width = width;
      m_height = height;
    }
    ~Buffer() {
      if (allocated) delete[] m_data;
    }
    void clear() {
      memset(m_data, 0, m_width*m_height*sizeof(uint32_t));
    }
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
      if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        printf("setPixel out of range: x=%d, y=%d\n", x, y);
        return;
      }
      uint32_t color = (a<<24)|(r<<16)|(g<<8)|b;
      m_data[y*m_width+x] = color;
    }
    void getPixel(int x, int y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) {
      if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        printf("setPixel out of range: x=%d, y=%d\n", x, y);
        return;
      }
      uint32_t color = m_data[y*m_width+x];
      a = (color&0xff000000)>>24;
      r = (color&0x00ff0000)>>16;
      g = (color&0x0000ff00)>>8;
      b = (color&0x000000ff);
    }
    // Save buffer as bmp file
    void saveAsBMP(const char *filename) {
      std::ofstream bmpFile(filename, std::ios::out | std::ios::binary);
      if (!bmpFile.is_open()) {
          // Handle error opening file
          return;
      }
      // BMP file header
      const uint16_t fileType = 0x4D42; // BM in little-endian
      const uint32_t fileSize = 54 + (m_width * m_height * 4); // BMP header size + pixel data size
      const uint16_t reserved1 = 0;
      const uint16_t reserved2 = 0;
      const uint32_t dataOffset = 54; // BMP header size
      // BMP info header
      const uint32_t infoHeaderSize = 40;
      const int32_t imageWidth = m_width;
      const int32_t imageHeight = m_height;
      const uint16_t planes = 1;
      const uint16_t bitsPerPixel = 32; // 4 bytes per pixel (ARGB)
      const uint32_t compression = 0; // no compression
      const uint32_t imageSize = m_width * m_height * 4; // size of the pixel data
      const int32_t xPixelsPerMeter = 0; // not used
      const int32_t yPixelsPerMeter = 0; // not used
      const uint32_t totalColors = 0; // not used
      const uint32_t importantColors = 0; // not used
      // Write BMP file header
      bmpFile.write(reinterpret_cast<const char*>(&fileType), sizeof(fileType));
      bmpFile.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
      bmpFile.write(reinterpret_cast<const char*>(&reserved1), sizeof(reserved1));
      bmpFile.write(reinterpret_cast<const char*>(&reserved2), sizeof(reserved2));
      bmpFile.write(reinterpret_cast<const char*>(&dataOffset), sizeof(dataOffset));
      // Write BMP info header
      bmpFile.write(reinterpret_cast<const char*>(&infoHeaderSize), sizeof(infoHeaderSize));
      bmpFile.write(reinterpret_cast<const char*>(&imageWidth), sizeof(imageWidth));
      bmpFile.write(reinterpret_cast<const char*>(&imageHeight), sizeof(imageHeight));
      bmpFile.write(reinterpret_cast<const char*>(&planes), sizeof(planes));
      bmpFile.write(reinterpret_cast<const char*>(&bitsPerPixel), sizeof(bitsPerPixel));
      bmpFile.write(reinterpret_cast<const char*>(&compression), sizeof(compression));
      bmpFile.write(reinterpret_cast<const char*>(&imageSize), sizeof(imageSize));
      bmpFile.write(reinterpret_cast<const char*>(&xPixelsPerMeter), sizeof(xPixelsPerMeter));
      bmpFile.write(reinterpret_cast<const char*>(&yPixelsPerMeter), sizeof(yPixelsPerMeter));
      bmpFile.write(reinterpret_cast<const char*>(&totalColors), sizeof(totalColors));
      bmpFile.write(reinterpret_cast<const char*>(&importantColors), sizeof(importantColors));
      // Write pixel data (in reverse order because BMP stores pixels bottom-up)
      for (int y = m_height - 1; y >= 0; --y) {
          for (int x = 0; x < m_width; ++x) {
              bmpFile.write(reinterpret_cast<const char*>(&m_data[y*m_width+x]), sizeof(uint32_t));
          }
      }
      bmpFile.close();
    }
  private:
    bool allocated;
    int m_width, m_height;
    uint32_t *m_data;
};

#endif