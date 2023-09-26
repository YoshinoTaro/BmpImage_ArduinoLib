/**
 * Simple bitmap image library for Arduino
 * LGPL version 2.1 Copyright 2021 Yoshino Taro
 */

#include <SDHCI.h>
#include "BmpImage.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC  9
#define TFT_CS  10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

SDClass SD;
BmpImage bmp;

const char BMPFILE[] = "RGB888.bmp";
const char format[3][8] = {"GRAY", "RGB565", "RGB888" };

void setup() {
  Serial.begin(115200);
  while (!SD.begin()) { Serial.println("Insert SD Card"); }
  tft.begin();
  tft.setRotation(3);

  /* open a bmp image */
  File myFile = SD.open(BMPFILE);
  if (!myFile) {
    Serial.println(String(BMPFILE) + " not found");
    return;
  }

  /* show the image */
  bmp.begin(myFile);
  BmpImage::BMP_IMAGE_PIX_FMT fmt = bmp.getPixFormat();
  /*
   *  BMP_IMAGE_NONE   = 0
   *  BMP_IMAGE_GRAY8  = 1
   *  BMP_IMAGE_RGB565 = 2
   *  BMP_IMAGE_RGB888 = 3
   */
  if (fmt > 0) {
    Serial.println(BMPFILE);
    Serial.println("Image format : " + String(format[fmt-1]));
    Serial.println("Image width  : " + String(bmp.getWidth()));
    Serial.println("Image height : " + String(bmp.getHeight()));
  } else {
    Serial.println("Fail: cannot read : " + String(bmp.getHeight()));
    return;
  }
  myFile.close();

  Serial.println("----------------------------------");

  // bmp.convertPixFormat(BmpImage::BMP_IMAGE_RGB888);
  tft.fillRect(0, 0, 320, 240, 0x0000);
  Serial.println("Height: " + String(bmp.getHeight()));
  Serial.println("Width : " + String(bmp.getWidth()));


#if 0
  File saveFile;
  char filename[] = "out888.bmp";
  bmp.convertPixFormat(BmpImage::BMP_IMAGE_RGB888_ALIGNED);
  if (SD.exists(filename)) SD.remove(filename);
  saveFile = SD.open(filename, FILE_WRITE);
  saveFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
  bmp.end();
  saveFile.close();
#endif

#if 0
  bmp.convertPixFormat(BmpImage::BMP_IMAGE_RGB565);
  tft.drawRGBBitmap(100, 100, (uint16_t*)bmp.getImgBuff(), bmp.getWidth(), bmp.getHeight()); 
  char filename[] = "out565.bmp";
  if (SD.exists(filename)) SD.remove(filename);
  File saveFile = SD.open(filename, FILE_WRITE);
  saveFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
  saveFile.close();
  bmp.end();
#endif

#if 1
  char filename[] = "out8.bmp";
  bmp.convertPixFormat(BmpImage::BMP_IMAGE_GRAY8);
  if (SD.exists(filename)) SD.remove(filename);
  File saveFile = SD.open(filename, FILE_WRITE);
  saveFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
  bmp.end();
  saveFile.close();
#endif

  tft.drawRect(100, 100, bmp.getWidth(), bmp.getHeight(), ILI9341_RED);
}

void loop() {
  // put your main code here, to run repeatedly:
}
