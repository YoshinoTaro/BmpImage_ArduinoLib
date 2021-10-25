/**
 * Simple bitmap image library for Arduino
 * LGPL version 2.1 Copyright 2021 Yoshino Taro
 */
#ifndef __SPRESENSE_BMP_IMAGE_CLASS_H__
#define __SPRESENSE_BMP_IMAGE_CLASS_H__
#include <stdint.h>
#include <stdlib.h>
#include <File.h>

//#define BRD_DEBUG

class BmpImage {
private:
  const uint16_t m_bfType = 0x4D42;
  uint32_t m_bfSize;
  uint16_t m_bfReserved1;
  uint16_t m_bfReserved2;
  uint32_t m_bfOffBits;
  uint32_t m_biSize;
  uint32_t m_biWidth;
  uint32_t m_biHeight;
  uint16_t m_biPlanes;
  uint16_t m_biBitCount;
  uint32_t m_biCompression;
  uint32_t m_biSizeImage;
  uint32_t m_biXPelsPerMeter;
  uint32_t m_biYPelsPerMeter;
  uint32_t m_biClrUsed;
  uint32_t m_biClrImportant;
  uint32_t m_biRmask;
  uint32_t m_biGmask;
  uint32_t m_biBmask; 
  
  uint8_t* m_bmp_buff;  
  uint8_t* m_img_buff;
  uint8_t** m_palette;
  
  uint8_t  m_word16[2];
  uint8_t  m_word32[4];

  uint8_t* swap16(uint16_t word16);
  uint8_t* swap32(uint32_t word32);

  uint16_t rswap16(uint8_t word16[2]);
  uint32_t rswap32(uint8_t dword32[4]);

  void allocPalette();
  void freePalette();


  const int HEADER_SIZE = 54;
  const int INFO_HEADER_SIZE = 40;
  const int MASK_SIZE = 12;
  const int PALETTE_SIZE_256  = 1024;
  const int BI_BITFIELD = 3; // only use in case of RGB565
  const uint32_t R5_MASK = 0x0000f800;
  const uint32_t G6_MASK = 0x000007e0;
  const uint32_t B5_MASK = 0x0000001f;
  
public:
  
  enum BMP_IMAGE_PIX_FMT {
    BMP_IMAGE_NONE = 0
   ,BMP_IMAGE_GRAY8
   ,BMP_IMAGE_RGB565
   ,BMP_IMAGE_RGB888
  };

  BmpImage();
  ~BmpImage();
  BMP_IMAGE_PIX_FMT begin(File& myFile);
  BMP_IMAGE_PIX_FMT begin(BMP_IMAGE_PIX_FMT fmt ,int width ,int height ,uint8_t* img, bool reverse=true);
  BMP_IMAGE_PIX_FMT convertPixFormat(BMP_IMAGE_PIX_FMT fmt);
  
  int getBmpSize();
  int getImgSize();
  uint8_t* getBmpBuff();
  uint8_t* getImgBuff();
  int getWidth();
  int getHeight();
  BmpImage::BMP_IMAGE_PIX_FMT getPixFormat();
  void end();
};


#endif // __SPRESENSE_BMP_IMAGE_CALSS_H__
