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
  private:
    bool allocated;
    int m_width, m_height;
    uint32_t *m_data;
};

#endif