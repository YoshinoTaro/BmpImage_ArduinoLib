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

const char BMPFILE[] = "RGB565.bmp";
const char format[3][8] = {"GRAY8", "RGB565", "RGB888" };

void openBmp() {
  File myFile = SD.open(BMPFILE);
  if (!myFile) {
    Serial.println(String(BMPFILE) + " not found");
    return;
  }

  bmp.begin(myFile);
  BmpImage::BMP_IMAGE_PIX_FMT fmt = bmp.getPixFormat();
  if (fmt > 0) {
    Serial.println(BMPFILE);
    Serial.println("Orginal  Image format : " + String(format[fmt-1]));
    Serial.println("Original File size    : " + String(bmp.getBmpSize()));
    Serial.println("Orginal  Image width  : " + String(bmp.getWidth()));
    Serial.println("Orgunal  Image height : " + String(bmp.getHeight()));
  } else {
    Serial.println("Fail: cannot read : " + String(bmp.getHeight()));
    return;
  }
  myFile.close();
  tft.fillRect(0, 0, 320, 240, 0);
  tft.drawRGBBitmap(0, 0, (uint16_t*)bmp.getImgBuff(), bmp.getWidth(), bmp.getHeight());
}

void saveBmp(BmpImage::BMP_IMAGE_PIX_FMT fmt) {
  
  bmp.convertPixFormat(fmt);
  bmp.alignImageLine(true);
  
  char filename[] = {0};
  sprintf(filename, "o%s.bmp", format[fmt-1]);
  if (SD.exists(filename)) SD.remove(filename);
  File saveFile = SD.open(filename, FILE_WRITE);
  saveFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
  saveFile.close();

  fmt = bmp.getPixFormat();
  if (fmt > 0) {
    Serial.println(filename);
    Serial.println("Saved Image format : " + String(format[fmt-1]));
    Serial.println("Saved File size    : " + String(bmp.getBmpSize()));
    Serial.println("Saved Image width  : " + String(bmp.getWidth()));
    Serial.println("Saved Image height : " + String(bmp.getHeight()));
  } else {
    Serial.println("Fail: cannot read : " + String(bmp.getHeight()));
    return;
  }

  bmp.end();
}

void setup() {
  Serial.begin(115200);
  while (!SD.begin()) { Serial.println("Insert SD Card"); }
  tft.begin();
  tft.setRotation(3);

  Serial.println();
  Serial.println("=============================");
  Serial.println("Convert RGB565 to RGB888");
  Serial.println("=============================");
  Serial.println();
  openBmp();
  saveBmp(BmpImage::BMP_IMAGE_RGB888);

  Serial.println();
  Serial.println("=============================");
  Serial.println("Convert RGB565 to GRAY8");
  Serial.println("=============================");
  Serial.println();
  openBmp();
  saveBmp(BmpImage::BMP_IMAGE_GRAY8);
}

void loop() {
  // put your main code here, to run repeatedly:
}
