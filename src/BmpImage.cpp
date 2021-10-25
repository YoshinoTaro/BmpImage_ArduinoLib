/**
 * Simple bitmap image library for Arduino
 * LGPL version 2.1 Copyright 2021 Yoshino Taro
 */
#include "BmpImage.h"
#ifdef BRD_DEBUG
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(x...)
#endif

BmpImage::BmpImage():
    m_bfSize(0) ,m_bfReserved1(0) ,m_bfReserved2(0)
   ,m_bfOffBits(0)
   ,m_biSize(INFO_HEADER_SIZE)
   ,m_biWidth(0)
   ,m_biHeight(0)
   ,m_biPlanes(1)
   ,m_biBitCount(0)
   ,m_biCompression(0)
   ,m_biSizeImage(0)
   ,m_biXPelsPerMeter(0x4724) /* dummy */
   ,m_biYPelsPerMeter(0x4724) /* dummy */
   ,m_biClrUsed(0)
   ,m_biClrImportant(0)
   ,m_biRmask(0)
   ,m_biGmask(0)
   ,m_biBmask(0)
   ,m_bmp_buff('\0')
   ,m_img_buff('\0')
   ,m_palette('\0')
   {} 
   
BmpImage::~BmpImage() {
  end();
}

BmpImage::BMP_IMAGE_PIX_FMT BmpImage::begin(File& myFile) {
  uint16_t data16;
  uint32_t data32;
  int n = 0;

  for (int i = 0; i < sizeof(uint16_t); ++i) {
    m_word16[i] = myFile.read(); ++n;
  }
  data16 = rswap16(m_word16);
  if (data16 != m_bfType) {
    DEBUG_PRINTF("Bitmap header is not BM : 0x%4X\n", data16);
    return BMP_IMAGE_NONE;
  }
  DEBUG_PRINTF("Bitmap header 0x%4X\n", data16);

  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  }
  DEBUG_PRINTF("\n");
  m_bfSize = rswap32(m_word32);
  DEBUG_PRINTF("Bitmap size: %d\n", m_bfSize);

  /* Reserved1 */
  for (int i = 0; i < sizeof(uint16_t); ++i) {
    m_word16[i] = myFile.read(); ++n;
  }
  
  /* Reserved2 */
  for (int i = 0; i < sizeof(uint16_t); ++i) {
    m_word16[i] = myFile.read(); ++n;
  }
  
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_bfOffBits = rswap32(m_word32);
  DEBUG_PRINTF("OffBits size: %d\n", m_bfOffBits);

  
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biSize = rswap32(m_word32);
  DEBUG_PRINTF("Image size: %d\n", m_biSize);

  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biWidth = rswap32(m_word32);
  DEBUG_PRINTF("Image width: %d\n", m_biWidth);
  
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biHeight = rswap32(m_word32);
  DEBUG_PRINTF("Image height: %d\n", m_biHeight);

  for (int i = 0; i < sizeof(uint16_t); ++i) {
    m_word16[i] = myFile.read(); ++n;
  }
  m_biPlanes = rswap16(m_word16);
  DEBUG_PRINTF("Planes: %d\n", m_biPlanes);

  for (int i = 0; i < sizeof(uint16_t); ++i) {
    m_word16[i] = myFile.read(); ++n;
  }
  m_biBitCount = rswap16(m_word16);
  DEBUG_PRINTF("BitCount: %d\n", m_biBitCount);

  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biCompression = rswap32(m_word32);
  DEBUG_PRINTF("Compression: %d\n", m_biCompression);


  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biSizeImage = rswap32(m_word32);
  DEBUG_PRINTF("Image size: %d\n", m_biSizeImage);

  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biXPelsPerMeter = rswap32(m_word32);
  DEBUG_PRINTF("Pixels per meter in X: %d\n", m_biXPelsPerMeter);
  
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biYPelsPerMeter = rswap32(m_word32);
  DEBUG_PRINTF("Pixels per meter in Y: %d\n", m_biYPelsPerMeter);

  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biClrUsed = rswap32(m_word32);
  DEBUG_PRINTF("Color Used: %d\n", m_biClrUsed);

  for (int i = 0; i < sizeof(uint32_t); ++i) {
    m_word32[i] = myFile.read(); ++n;
  } 
  m_biClrImportant = rswap32(m_word32);
  DEBUG_PRINTF("Color Important: %d\n", m_biClrImportant);

  /* in case of RGB565 */
  if (m_biBitCount == 16) {  
    for (int i = 0; i < sizeof(uint32_t); ++i) {
      m_word32[i] = myFile.read(); ++n;
    } 
    m_biRmask = rswap32(m_word32);
    DEBUG_PRINTF("Rmask: 0x%04X\n", m_biRmask);
    for (int i = 0; i < sizeof(uint32_t); ++i) {
      m_word32[i] = myFile.read(); ++n;
    } 
    m_biGmask = rswap32(m_word32);
    DEBUG_PRINTF("Gmask: 0x%04X\n", m_biGmask);
    for (int i = 0; i < sizeof(uint32_t); ++i) {
      m_word32[i] = myFile.read(); ++n;
    } 
    m_biBmask = rswap32(m_word32);
    DEBUG_PRINTF("Bmask: 0x%04X\n", m_biBmask);
  }

  /* in case of GRAY8 */
  if (m_biBitCount == 8) { 
    allocPalette();
    for (int i = 0; i < PALETTE_SIZE_256/4; ++i) {
      m_palette[i][0] = myFile.read();
      m_palette[i][1] = myFile.read();
      m_palette[i][2] = myFile.read();
      myFile.read(); // dummy read
    }
  }
 
  if (m_img_buff != '\0') free(m_img_buff);
  m_img_buff = (uint8_t*)malloc(m_biSizeImage);
  if (m_img_buff == '\0') {
    DEBUG_PRINTF("not enough memory\n");
    return BMP_IMAGE_NONE;
  }
  
  for (int h = m_biHeight-1; h >= 0; --h) {
    for (int w = 0; w < m_biWidth; ++w) {
      if (!myFile.available()) {
        printf("Fatal Error: File Size Mismatch\n");
        break;
      }
      int n = h*m_biWidth + w;
      if (m_biBitCount == 8) {
        int p = myFile.read();
        uint8_t r = m_palette[p][0];
        uint8_t g = m_palette[p][1];
        uint8_t b = m_palette[p][2];
        m_img_buff[n] = (uint8_t)(0.299*r + 0.587*g + 0.114*b);
      } else {
        m_img_buff[n] = myFile.read();
      }
    }
  }

  return m_biBitCount/8;
}

BmpImage::BMP_IMAGE_PIX_FMT BmpImage::begin(BmpImage::BMP_IMAGE_PIX_FMT fmt
                  , int width, int height, uint8_t* img, bool reverse) {

  this->end();
  DEBUG_PRINTF("fmt %d width %d height %d\n",fmt ,width ,height);
  if (fmt == BMP_IMAGE_NONE || width == 0 || height == 0 || img == '\0') 
    return BMP_IMAGE_NONE;
   
  m_biSizeImage = width*height*fmt;
  DEBUG_PRINTF("biSizeImage: %d\n", m_biSizeImage);
  if (fmt == BMP_IMAGE_RGB565) {
    m_bfOffBits = HEADER_SIZE + MASK_SIZE;
    m_bfSize = HEADER_SIZE + MASK_SIZE + m_biSizeImage;
    m_biCompression = BI_BITFIELD;
    m_biRmask = R5_MASK; /* 16bit RED mask */
    m_biGmask = G6_MASK; /* 16bit GREEN mask */
    m_biBmask = B5_MASK; /* 16bit BLUE mask */
  } else if (fmt == BMP_IMAGE_GRAY8) {
    m_bfOffBits = HEADER_SIZE + PALETTE_SIZE_256;
    m_bfSize = HEADER_SIZE + PALETTE_SIZE_256 + m_biSizeImage;
    m_biCompression = 0;
  } else if (fmt == BMP_IMAGE_RGB888) {
    m_bfOffBits = HEADER_SIZE;
    m_bfSize = HEADER_SIZE + m_biSizeImage;  
    m_biCompression = 0;
  } else {
    return BMP_IMAGE_NONE;
  }

  m_biSize = INFO_HEADER_SIZE;
  m_biWidth = width;
  m_biHeight = height;
  m_biBitCount = fmt*8;

  m_bmp_buff = (uint8_t*)malloc(m_bfSize);
  memset(m_bmp_buff, 0, m_bfSize);
  m_img_buff = m_bmp_buff + m_bfOffBits;

  /* create bitmap header */
  uint8_t* word16;
  uint8_t* word32;
  int n = 0;

  uint8_t* bmp_header = m_bmp_buff;
  word16 = swap16(m_bfType); /* "BM" */
  for (int i = 0; i < sizeof(uint16_t); ++i) {
    bmp_header[n++] = word16[i]; 
  }
  word32 = swap32(m_bfSize); /* File Size */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }
  word16 = swap16(m_bfReserved1); /* Reserved */
  for (int i = 0; i < sizeof(uint16_t); ++i) {
    bmp_header[n++] = word16[i];  
  }
  word16 = swap16(m_bfReserved2); /* Reserved */
  for (int i = 0; i < sizeof(uint16_t); ++i) {
    bmp_header[n++] = word16[i];  
  }
  word32 = swap32(m_bfOffBits); /* Offset to image data */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }
  word32 = swap32(m_biSize); /* Bitmap info structure size (40) */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }  
  word32 = swap32(m_biWidth); /* Image width */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }  
  word32 = swap32(m_biHeight); /* Image height */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }    
  word16 = swap16(m_biPlanes); /* Image plane (almost 1) */
  for (int i = 0; i < sizeof(uint16_t); ++i) {
    bmp_header[n++] = word16[i];  
  }   
  word16 = swap16(m_biBitCount); /* Pixel per bits */
  for (int i = 0; i < sizeof(uint16_t); ++i) {
    bmp_header[n++] = word16[i];  
  }   
  word32 = swap32(m_biCompression); /* Complession type */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }
  word32 = swap32(m_biSizeImage); /* Image size */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }
  word32 = swap32(m_biXPelsPerMeter); /* Resolution (dummy) */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }
  word32 = swap32(m_biYPelsPerMeter); /* Resolution (dummy) */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }
  word32 = swap32(m_biClrUsed); /* Color used */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }
  word32 = swap32(m_biClrImportant); /* Important Color */
  for (int i = 0; i < sizeof(uint32_t); ++i) {
    bmp_header[n++] = word32[i];  
  }
  if (fmt == BMP_IMAGE_RGB565) {
    word32 = swap32(m_biRmask); /* Bitmask for red in case of 16bits color */
    for (int i = 0; i < sizeof(uint32_t); ++i) {
      bmp_header[n++] = word32[i];  
    }
    word32 = swap32(m_biGmask); /* Bitmask for green in case of 16bits color */
    for (int i = 0; i < sizeof(uint32_t); ++i) {
      bmp_header[n++] = word32[i];  
    }
    word32 = swap32(m_biBmask); /* Bitmask for blue in case of 16bits color */
    for (int i = 0; i < sizeof(uint32_t); ++i) {
      bmp_header[n++] = word32[i];  
    }
  }
  if (fmt == BMP_IMAGE_GRAY8) {
    for (int i = 0; i < PALETTE_SIZE_256/sizeof(uint32_t); ++i) {
      bmp_header[n++] = i;
      bmp_header[n++] = i;
      bmp_header[n++] = i;
      bmp_header[n++] = 0;
    }
  }

  /* copy image content to bitmap container */
  if (reverse == false) memcpy(m_img_buff, img, m_biSizeImage);
  else {
    int n = 0;
    for (int y = height - 1; y >= 0; --y, ++n) {
      for (int x = 0; x < width; ++x) {
        m_img_buff[width*y + x]  = img[width*n + x];
      }
    }
  }
  return fmt;
}



BmpImage::BMP_IMAGE_PIX_FMT BmpImage::convertPixFormat(BmpImage::BMP_IMAGE_PIX_FMT fmt) {

  if (m_bfSize == 0 || m_biWidth == 0 || m_biHeight == 0) {
    DEBUG_PRINTF("Invalid parameter\n");
    return BMP_IMAGE_NONE;
  }

  uint8_t*  tmp_rgbBuff;
  uint16_t* tmp_rgb565Buff;
  uint8_t*  tmp_grayBuff;  
  BMP_IMAGE_PIX_FMT pre_fmt = m_biBitCount/8;
  DEBUG_PRINTF("From FMT %d to FMT %d\n", pre_fmt, fmt);
  if (fmt == pre_fmt || fmt == BMP_IMAGE_NONE) return fmt;

  /** to RGB888 from RGB565 or GRAY8 **/
  if (fmt == BMP_IMAGE_RGB888) {
    tmp_rgbBuff = (uint8_t*)malloc(m_biWidth*m_biHeight*fmt);
    if (tmp_rgbBuff == '\0') {
      DEBUG_PRINTF("not enough memory\n");
      return BMP_IMAGE_NONE;
    }
    /* from RGB565 */
    if (pre_fmt == BMP_IMAGE_RGB565) {
      uint16_t* rgb565 = (uint16_t*)m_img_buff;
      int n = 0;
      for (int i = 0; i < m_biWidth*m_biHeight; ++i) {
        uint8_t r = (uint8_t)((rgb565[i] & R5_MASK) >> 8);
        uint8_t g = (uint8_t)((rgb565[i] & G6_MASK) >> 3);
        uint8_t b = (uint8_t)((rgb565[i] & B5_MASK) << 3);
        tmp_rgbBuff[n++] = b;
        tmp_rgbBuff[n++] = g;
        tmp_rgbBuff[n++] = r;
      }
    }
    /* from GRAY8 */
    else if (pre_fmt == BMP_IMAGE_GRAY8) {
      uint8_t* gray8 = (uint8_t*)m_img_buff;
      int n = 0;
      for (int i = 0; i < m_biWidth*m_biHeight; ++i) {
        tmp_rgbBuff[n++] = gray8[i];
        tmp_rgbBuff[n++] = gray8[i];
        tmp_rgbBuff[n++] = gray8[i];
      }      
    }
    this->begin(BMP_IMAGE_RGB888, m_biWidth, m_biHeight, tmp_rgbBuff);
    free(tmp_rgbBuff);
  }
  
  /** to RGB565 from RGB888 or GRAY8 **/
  if (fmt == BMP_IMAGE_RGB565) {
    tmp_rgb565Buff = (uint16_t*)malloc(m_biWidth*m_biHeight*fmt);
    if (tmp_rgb565Buff == '\0') {
      DEBUG_PRINTF("not enough memory\n");
      return BMP_IMAGE_NONE;
    }
    /* from RGB888 */
    if (pre_fmt == BMP_IMAGE_RGB888) {
      uint8_t* rgb888 = (uint8_t*)m_img_buff;
      int n = 0;
      for (int i = 0; i < m_biWidth*m_biHeight; ++i) {
	uint16_t b = ((uint16_t)(rgb888[n++]) >> 3) & B5_MASK;
	uint16_t g = ((uint16_t)(rgb888[n++]) << 3) & G6_MASK;
	uint16_t r = ((uint16_t)(rgb888[n++]) << 8) & R5_MASK;
        tmp_rgb565Buff[i] = r | g | b;
      }
    }
    /* from GRAY8 */
    else if (pre_fmt == BMP_IMAGE_GRAY8) {
      uint8_t* gray8 = (uint8_t*)m_img_buff;
      int n = 0;
      for (int i = 0; i < m_biWidth*m_biHeight; ++i) {
        uint16_t b = ((uint16_t)(gray8[i]) >> 3) & B5_MASK; 
        uint16_t g = ((uint16_t)(gray8[i]) << 3) & G6_MASK; 
        uint16_t r = ((uint16_t)(gray8[i]) << 8) & R5_MASK;   
        tmp_rgb565Buff[i] = r | g | b;
      }      
    }
    this->begin(BMP_IMAGE_RGB565, m_biWidth, m_biHeight, (uint8_t*)tmp_rgb565Buff);
    free(tmp_rgb565Buff);
  }

  /** to GRAY8 from RGB888 or RGB565 **/
  if (fmt == BMP_IMAGE_GRAY8) {
    tmp_grayBuff = (uint8_t*)malloc(m_biWidth*m_biHeight*fmt);
    if (tmp_grayBuff == '\0') {
      DEBUG_PRINTF("not enough memory\n");
      return BMP_IMAGE_NONE;
    }
    /* from RGB888 */
    if (pre_fmt == BMP_IMAGE_RGB888) {
      uint16_t* rgb888 = (uint16_t*)m_img_buff;
      int n = 0;
      for (int i = 0; i < m_biWidth*m_biHeight; ++i) {
        float r = rgb888[n++];
        float g = rgb888[n++];
        float b = rgb888[n++];
        float gray = 0.299*r + 0.587*g + 0.114*b;
        tmp_grayBuff[i] = (uint8_t)gray;
      }
    }
    /* from RGB565 */
    else if (pre_fmt == BMP_IMAGE_RGB565) {
      uint16_t* rgb565 = (uint16_t*)m_img_buff;
      int n = 0;
      for (int i = 0; i < m_biWidth*m_biHeight; ++i) {
        float r = (float)((rgb565[i] & R5_MASK) >> 8); 
        float g = (float)((rgb565[i] & G6_MASK) >> 3); 
        float b = (float)((rgb565[i] & B5_MASK) << 3);   
        float gray = 0.299*r + 0.587*g + 0.114*b;
        tmp_grayBuff[i] = (uint8_t)gray;
      }      
    }
    this->begin(BMP_IMAGE_GRAY8, m_biWidth, m_biHeight, tmp_grayBuff);
    free(tmp_grayBuff);
  }
  
  return fmt;
}



void BmpImage::end() {
  if (m_bmp_buff != '\0') {
    memset(m_bmp_buff, 0, m_bfSize);
    free(m_bmp_buff);
    m_bmp_buff = '\0';
    m_img_buff = '\0';
  }
  if (m_img_buff != '\0') {
    memset(m_img_buff, 0, m_biSizeImage);
    free(m_img_buff);
    m_img_buff = '\0';
  }
}

uint8_t* BmpImage::getBmpBuff() {
  return m_bmp_buff;
}

uint8_t* BmpImage::getImgBuff() {
  return m_img_buff;
}

int BmpImage::getBmpSize() {
  return m_bfSize;
}

int BmpImage::getImgSize() {
  return m_biSizeImage;
}

int BmpImage::getWidth() {
  return m_biWidth;
}

int BmpImage::getHeight() {
  return m_biHeight;
}

BmpImage::BMP_IMAGE_PIX_FMT BmpImage::getPixFormat() {
  return m_biBitCount/8;
}

/* private functions */
uint8_t* BmpImage::swap16(uint16_t word16) {
  m_word16[0] = (uint8_t)(word16 & 0x00ff);
  m_word16[1] = (uint8_t)((word16 & 0xff00) >> 8);
  return m_word16;
}

uint8_t* BmpImage::swap32(uint32_t dword32) {
  m_word32[0] = (uint8_t)(dword32 & 0x000000ff);
  m_word32[1] = (uint8_t)((dword32 & 0x0000ff00) >> 8);
  m_word32[2] = (uint8_t)((dword32 & 0x00ff0000) >> 16);
  m_word32[3] = (uint8_t)((dword32 & 0xff000000) >> 24);
  return m_word32;
}

uint16_t BmpImage::rswap16(uint8_t word16[2]) {
  uint16_t l_data = (uint16_t)(word16[0]);
  uint16_t u_data = (uint16_t)(word16[1]);
  return u_data << 8 | l_data;
}

uint32_t BmpImage::rswap32(uint8_t word32[4]) {
  uint32_t data32;
  uint32_t u_data16;
  uint32_t l_data16;
  uint16_t l_data;
  uint16_t u_data;

  l_data = (uint16_t)(word32[0]);
  u_data = (uint16_t)(word32[1]);
  l_data16 = u_data << 8 | l_data;
  
  l_data = (uint16_t)(word32[2]);
  u_data = (uint16_t)(word32[3]);
  u_data16 = u_data << 8 | l_data;

  data32 = u_data16 << 16 | l_data16;
  
  return data32;
}


void BmpImage::allocPalette() {
  if (m_palette != '\0') freePalette();
  m_palette = (uint8_t**)malloc(sizeof(uint8_t*)*(PALETTE_SIZE_256/4));
  for (int i = 0; i < PALETTE_SIZE_256/4; ++i) {
    m_palette[i] = (uint8_t*)malloc(sizeof(uint8_t)*3);
  }
}

void BmpImage::freePalette() {
  if (m_palette == '\0') return;
  for (int i = 0; i < PALETTE_SIZE_256/4; ++i) {
    memset(m_palette[i], '\0', 3);
    free(m_palette[i]);
    m_palette[i] = '\0';
  }
  free(m_palette);
  m_palette = '\0';
}
