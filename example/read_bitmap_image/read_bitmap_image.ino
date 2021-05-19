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

const char BMPFILE[] = "test.bmp";
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
    goto end;
  }


  bmp.convertPixFormat(BmpImage::BMP_IMAGE_RGB565);
  tft.drawRGBBitmap(0, 0, (uint16_t*)bmp.getImgBuff(), bmp.getWidth(), bmp.getHeight());

end:
  bmp.end();
  myFile.close();
  
}

void loop() {
  // put your main code here, to run repeatedly:
}
