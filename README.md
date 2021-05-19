# BmpImage_ArduinoLib
 Simple Bitmap Library for Arduino.
 This library may be used for Arduino based boards, but please note that the sample codes are written for Spresense, and I have not checked the library with other Arduino-based board. 
 
## Support Format:
   - RGB888 color: 24bit full color
   - RGB565 color: 16bit compressed color
   - Gray Scale:   8bit gray scale
   
   
## How to write
   *RGB888*
   ```
       // the image pointing imgptr must be true color
       bmp.begin(BmpImage::BMP_IMAGE_RGB888, width, height, imgptr);
       myFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
       bmp.end(); /* free memory */
   ```
   *RGB565*
   ```
       // the image pointing imgptr must be 16bit color
       bmp.begin(BmpImage::BMP_IMAGE_RGB565, width, height, imgptr);
       myFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
       bmp.end();
   ```
   *GRAY*
   ```
       // the image pointing imgptr must be 8bit gray scale
       bmp.begin(BmpImage::BMP_IMAGE_GRAY8, width, height, imgptr);
       myFile.write(bmp.getBmpBuff(), bmp.getBmpSize());
       bmp.end(); /* free memory */
   ```

## How to read
 ```
   bmp.begin(myFile);
   BmpImage::BMP_IMAGE_PIX_FMT fmt = bmp.getPixFormat();

   if (fmt > 0) {
     Serial.println(BMPFILE);
     Serial.println("Image format : " + String(format[fmt-1]));
     Serial.println("Image width  : " + String(bmp.getWidth()));
     Serial.println("Image height : " + String(bmp.getHeight()));
   } else {
     Serial.println("Fail: cannot read : " + String(bmp.getHeight()));
   }
  end:
    bmp.end(); /* free memory */
 ```
