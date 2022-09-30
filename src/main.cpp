#include <Arduino.h>


/*
 Adapted from the Adafruit graphicstest sketch, see original header at end
 of sketch.

 This sketch uses the GLCD font (font 1) only.

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
*/


/*
 Adapted from the Adafruit and Xark's PDQ graphicstest sketch.

 See end of file for original header text and MIT license info.
 
 This sketch uses the GLCD font only.

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */

/*
  The TFT_eSPI library incorporates an Adafruit_GFX compatible
  button handling class, this sketch is based on the Arduin-o-phone
  example.

  This example diplays a keypad where numbers can be entered and
  send to the Serial Monitor window.

  The sketch has been tested on the ESP8266 (which supports SPIFFS)

  The minimum screen size is 320 x 240 as that is the keypad size.
*/

// The SPIFFS (FLASH filing system) is used to hold touch screen
// calibration data

/*
  The TFT_eSPI library incorporates an Adafruit_GFX compatible
  button handling class, this sketch is based on the Arduin-o-phone
  example.

  This example diplays a keypad where numbers can be entered and
  send to the Serial Monitor window.

  The sketch has been tested on the ESP8266 (which supports SPIFFS)

  The minimum screen size is 320 x 240 as that is the keypad size.
*/

// The SPIFFS (FLASH filing system) is used to hold touch screen
// calibration data

#include "FS.h"

#include <SPI.h>
#include <TFT_eSPI.h>
#include "Wire.h"  
#include <SimpleKalmanFilter.h>
#include "uFire_SHT20.h"
uFire_SHT20 sht20;
                                  // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite graph1 = TFT_eSprite(&tft);
SimpleKalmanFilter kf = SimpleKalmanFilter(0.5, 0.5, 0.01);

float HighY = 100;
float LowY = 40;
uint16_t t_x = 0, t_y = 0;

#define KEY_X 40 // Centre of key
#define KEY_Y 20
#define KEY_W 62 // Width and height
#define KEY_H 30
#define KEY_SPACING_X 18 // X and Y gap
#define KEY_SPACING_Y 20
#define KEY_TEXTSIZE 1   // Font size multiplier

// Using two fonts since numbers are nice when bold
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font 1
#define LABEL2_FONT &FreeSansBold12pt7b 

char keyLabel[15][5] = {"New", "Del", "Send", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0","#" };
uint16_t keyColor[15] = {TFT_RED, TFT_DARKGREY, TFT_DARKGREEN,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE,TFT_BLUE
                        };
TFT_eSPI_Button key[15];

int stage = 0;

 // Invoke custom library
#include "setting.h"
// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
void DrawHomescreen(){
    tft.fillScreen(TFT_BLACK);
    // tft.fillRoundRect(180, 0, 60, 60,30, TFT_BLACK);
    // tft.drawRoundRect(180, 0, 60, 60, 30, TFT_WHITE);
    tft.setSwapBytes(true);
    tft.pushImage(180, 260, settingWidth  ,settingHeight, setting);
    
}
// void OTAScreen(){
//   // DrawHomescreen();
//   tft.setTextColor(TFT_WHITE, TFT_BLACK);
//   tft.drawString("OTA",100, 50,3);

// }

void drawKeypad()
{
  // Draw the keys
  for (uint8_t row = 0; row < 5; row++) {
    for (uint8_t col = 0; col < 3; col++) {
      uint8_t b = col + row * 3;

      if (b < 3) tft.setFreeFont(LABEL1_FONT);
      else tft.setFreeFont(LABEL2_FONT);

      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
    }
  }
}

void ResetXY(){
  t_x = 0;
  t_y = 0;
}

//------------------------------------------------------------------------------------------
void setup(){
  Wire.begin();
  sht20.begin();
  tft.init();

  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  graph1.createSprite(200, 100);
  DrawHomescreen();
  
}

void loop(){
  if(stage == 0){
        float T = millis();
        float H = sht20.humidity();
        float F = kf.updateEstimate(H);
        if(T>2000){

              graph1.drawFastVLine(199,100-100*((F-LowY)/(HighY-LowY)),3,TFT_YELLOW);
              graph1.scroll(-1);

        } 
          graph1.pushSprite(20, 32);
        tft.setTextFont(1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawFloat(float(HighY),0,1, 32,1);
        tft.drawFloat(float(LowY),0,10, 132,1);

        tft.drawFloat(float(F), 1, 70, 180, 6);
        
        tft.drawString("King's ",1, 250,2);
        tft.drawString("Technologies",1,270,1);
        tft.drawString("Phase",1,280,2);
        
      
  }


    if(tft.getTouch(&t_x, &t_y)){
      printf("%d\n", t_x);
      printf("%d\n", t_y);
      if(stage == 0 || stage ==2 || stage ==3 || stage ==4){
        if(t_x > 0 && t_x < 35  && t_y > 245 && t_y < 290){

            tft.fillScreen(TFT_BLACK);
            tft.fillRoundRect(10, 10, 220, 60,30 ,TFT_RED);
            tft.drawRoundRect(10, 10, 220, 60,30, TFT_WHITE);
            tft.setTextColor(TFT_WHITE, TFT_RED);
            tft.drawString("OTA Setting",80,30,2);

            tft.fillRoundRect(10, 90, 220, 60,30 ,TFT_RED);
            tft.drawRoundRect(10, 90, 220, 60, 30,TFT_WHITE);
            // tft.setTextColor(TFT_WHITE);
            tft.drawString("Collaboration",80,110,2);
            
            tft.fillRoundRect(10, 170, 220, 60,30 ,TFT_RED);
            tft.drawRoundRect(10, 170, 220, 60,30, TFT_WHITE);
            // tft.setTextColor(TFT_WHITE);
            tft.drawString("PID Controller",80,190,2);

            tft.fillRoundRect(10, 250, 220, 60,30 ,TFT_RED);
            tft.drawRoundRect(10, 250, 220, 60,30, TFT_WHITE);
            tft.drawString("Return",100,270,2);

            stage= 1;
         
        
            ResetXY();
            delay(400);
        }
      }
      if(stage == 1){ 
      
        if(t_x > 10 && t_x < 50  && t_y >20  && t_y < 290){
                                  //return button
          DrawHomescreen();
          stage = 0;
          ResetXY();
        }

        if(t_x > 75 && t_x < 115  && t_y >20  && t_y < 290){ //PID_controller
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(180, 260, settingWidth  ,settingHeight, setting);
          tft.fillRoundRect(10, 10, 220, 30,15 ,TFT_RED);
          tft.setTextColor(TFT_WHITE, TFT_RED);
          // tft.drawString("PID_controller", 80,15,2);

          drawKeypad();

          stage = 2;
          printf("stage2 \n");
          ResetXY();
        } 

        if(t_x > 140 && t_x < 180  && t_y >20  && t_y < 290){ //Collaboration
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(180, 260, settingWidth  ,settingHeight, setting);
          tft.fillRoundRect(10, 10, 220, 30,15 ,TFT_RED);
          tft.setTextColor(TFT_WHITE, TFT_RED);
          tft.drawString("Collaboration", 80,15,2);

          printf("stage3 \n");
          stage = 3;
          ResetXY();
        } 

        if(t_x > 200 && t_x < 240  && t_y >20  && t_y < 290){ //OTA Setting
          tft.fillScreen(TFT_BLACK);
          tft.pushImage(180, 260, settingWidth  ,settingHeight, setting);
          tft.fillRoundRect(10, 10, 220, 30,15 ,TFT_RED);
          tft.setTextColor(TFT_WHITE, TFT_RED);
          tft.drawString("OTA Setting", 80,15,2);
          printf("stage4 \n");
          
          stage = 4;
          ResetXY();
        } 

      }
    }

}
//------------------------------------------------------------------------------------------

