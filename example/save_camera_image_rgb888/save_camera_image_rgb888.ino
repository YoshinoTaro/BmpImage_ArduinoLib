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

    Serial.println("Shrink the captured image");
    CamImage small;
    img.resizeImageByHW(small, width, height);

    Serial.println("Save the shrunk image as a rgb888 image");
    /* to make the rgb888 image from the camera image, you should take 3 step */
    /* 1. convert the camera image from yuv422 to the rgb565 */
    small.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    /* 2. make the bmp container based on rgb565 */
    bmp.begin(BmpImage::BMP_IMAGE_RGB565, width, height, small.getImgBuff());
    /* 3. convert the rgb565 image to the rgb888 image */
    bmp.convertPixFormat(BmpImage::BMP_IMAGE_RGB888);

    char filename[16] = {};
    sprintf(filename, "rgb24%03d.bmp", g_counter);
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
