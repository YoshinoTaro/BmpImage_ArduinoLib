/**
 * Simple bitmap image library for Arduino
 * LGPL version 2.1 Copyright 2021 Yoshino Taro
 */

#include <Camera.h>
#include <SDHCI.h>
#include "BmpImage.h"

SDClass SD;
BmpImage bmp;

const int intPin = 0;

bool bButtonPressed = false;
void changeState() {
  bButtonPressed = true;
}

void CamCB(CamImage img) {

  const static int width = 160;
  const static int height = 120;
  static int g_counter = 0;

  if (img.isAvailable() && bButtonPressed) {

    Serial.println("Shrink the caputred image");
    CamImage small;
    img.resizeImageByHW(small, width, height);

    Serial.println("Save the shrunk image as a gray image ");
    /** make the gray image from yuv422 */
    uint8_t* gray = (uint8_t*)malloc(width*height);  
    uint16_t* imgbuff = (uint16_t*)small.getImgBuff();
    for (int i = 0; i < width*height; ++i) {
      gray[i] = (uint8_t)(((imgbuff[i] & 0xf000) >> 8) | ((imgbuff[i] & 0x00f0) >> 4));     
    }

    /* make the bmp container and copy the data*/
    bmp.begin(BmpImage::BMP_IMAGE_GRAY8, width, height, gray);
    free(gray);
    
    char filename[16] = {};
    sprintf(filename, "gray%03d.bmp", g_counter);
    if (SD.exists(filename)) SD.remove(filename);
    File myFile = SD.open(filename, FILE_WRITE);
    myFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
    myFile.close();
    bmp.end();
    Serial.println("Saved as " + String(filename));

    bButtonPressed = false;
    ++g_counter;
  }
}

void setup() {
  Serial.begin(115200);
  while (!SD.begin()) { Serial.println("Insert SD Card"); }

  attachInterrupt(digitalPinToInterrupt(intPin) ,changeState ,FALLING);
  sleep(1);
  theCamera.begin();
  theCamera.startStreaming(true, CamCB);
  Serial.println("Ready to capture");
}

void loop() {
  // put your main code here, to run repeatedly:

}
