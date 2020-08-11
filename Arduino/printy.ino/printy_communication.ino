/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include "printy_communication.h"

Communication::Communication() {
  awaitingData = 0;
}

void Communication::setup(long baud) {
  Serial.begin(baud);
}

void Communication::read() {
  if(awaitingData > 0) {
    handleExtraData();
    #if _COMM_DEBUG
      Serial.println();
    #endif
  }
  else if(Serial.available() > 0) {
    awaitingData = handleMessage(Serial.read());
    #if _COMM_DEBUG
      Serial.println();
    #endif
  }
}

void Communication::write(byte data) {
  
}

uint8_t Communication::handleMessage(byte message) {
  #if _COMM_DEBUG
    Serial.print(message);
    Serial.print("\t");
  #endif
  if(message >= 0b10000000) {   //1xxxxxxx
    if(message >= 0b11000000) {   //11xxxxxx
      if(message >= 0b11100000) {   //111xxxxx
        ledManager.setFadingMode(message & 0b00011111);
        #if _COMM_DEBUG
          Serial.print("fadingMode: ");
          Serial.print(message & 0b00011111);
        #endif
      }
      else {                        //110xxxxx
        if(message >= 0b11010000) {   //1101xxxx
          if(message >= 0b11011100) {   //11011100 -> 11011111
            if(message >= 0b11011110) {   //1101111x
              ledManager.setRandomFadingDuration(message & 0b00000001);
              #if _COMM_DEBUG
                Serial.print("isRandomFadingDuration: ");
                Serial.print(message & 0b00000001);
              #endif
            }
            else {                        //11011110x
              ledManager.setRandomEffectDuration(message & 0b00000001);
              #if _COMM_DEBUG
                Serial.print("isRandomEffectDuration: ");
                Serial.print(message & 0b00000001);
              #endif
            }
          }
          else {                        //11010000 -> 11011011                       
            if(message >= 0b11011000) {   //110110xx
              if(message >= 0b11011010) {   //1101101x
                ledManager.setRandomFadingMode(message & 0b00000001);
                #if _COMM_DEBUG
                  Serial.print("isRandomFadingMode: ");
                  Serial.print(message & 0b00000001);
                #endif
              }
              else {                        //1101100x
                ledManager.setRandomEffectMode(message & 0b00000001);
                #if _COMM_DEBUG
                  Serial.print("isRandomEffectMode: ");
                  Serial.print(message & 0b00000001);
                #endif
              }
            }
            else {                        //11010xxx 
              if(message >= 0b11010110) {   //1101011x
                ledManager.setRandomHue(message & 0b00000001);
                #if _COMM_DEBUG
                  Serial.print("isRandomHue: ");
                  Serial.print(message & 0b00000001);
                #endif
              }
              else if(message >= 0b11010010) { //11010010 -> 11010101
                ledManager.setExtruderMode(message & 0b00000011);
                #if _COMM_DEBUG
                  Serial.print("extruderMode: ");
                  Serial.print(message & 0b00000011);
                #endif
              }
            }
          }
        }
        else {                        //1100xxxx
          ledManager.setSaturation ((message & 0b00001111)*17);
          #if _COMM_DEBUG
            Serial.print("saturation: ");
            Serial.print((message & 0b00001111)*17);
          #endif
        }
      }
    }
    else {                        //10xxxxxx
      if(message >= 0b10100000) {   //101xxxxx
        ledManager.setEffectDuration(message >= 0b10110000 ? (message-171)<<2 : (message-159));
        #if _COMM_DEBUG
          Serial.print("effectDuration: ");
          Serial.print(message >= 0b10110000 ? (message-171)<<2 : (message-159));
        #endif    
      }
      else {                        //100xxxxx
        if(message >= 0b10010000) {   //1001xxxx
          if(message >= 0b10011000) {   //10011xxx
            ledManager.setFadingDuration(message & 0b00000111);
            #if _COMM_DEBUG
              Serial.print("fadingDuration: ");
              Serial.print(message & 0b00000111);
            #endif   
          }
          else {                         //10010xxx
            if(message >= 0b10010100) {    //100101xx
              if(message >= 0b10010110) {    //1001011x
                printerStatus.schedulePowerOff(message & 0b00000001);
                printerStatus.scheduleReboot(false);
                #if _COMM_DEBUG
                  Serial.print(message & 0b00000001 ? "Schedule shutdown" : "Unschedule shutdown");
                #endif
              }
              else {                         //1001011x
                printerStatus.scheduleReboot(message & 0b00000001);
                printerStatus.schedulePowerOff(message & 0b00000001);
                #if _COMM_DEBUG
                  Serial.print(message & 0b00000001 ? "Scheduled reboot" : "Unscheduled reboot");
                #endif
              }
            }
            else {                            //100100xx
              if(message >= 0b10010010) {       //1001001x
                if(message >= 0b10010011) {       //10010011
                  
                  #if _COMM_DEBUG
                    Serial.print("Stored current settings");
                  #endif
                }
                else {                            //10010010

                  #if _COMM_DEBUG
                    Serial.print("Loaded stored settings");
                  #endif
                }
              }
              else {                            //1001000x
                if(message >= 0b10010001) {       //10010001

                  #if _COMM_DEBUG
                    Serial.print("Loaded default settings");
                  #endif
                }
              }
            }
          }
        }
        else {                        //1000xxxx
          if(message <= 0b10001001) {   //10000000 -> 10001001
            #if _COMM_DEBUG
              Serial.print("Printer status: ");
              Serial.print((message & 0b00001111)+3);
            #endif
            if(message & 0b00000001) {
              awaitingStatus = (message & 0b00001111)+3;
              return (message==0b10000111 || message==0b10001001) ? 9 : (message&0b00001111)+1;
            }
            else {
              printerStatus.setNewPrinterStatus((message & 0b00001111)+3);
            }
          }
        }
      }
    }
  }
  else {                        //0xxxxxxx
    if(message >= 0b01000000) {   //01xxxxxx
      ledManager.setHue((message & 0b00111111)<<2);
      #if _COMM_DEBUG
        Serial.print("hue: ");
        Serial.print((message & 0b00111111)<<2);
      #endif
    }
    else {                        //00xxxxxx
      if(message >= 0b00100000) {   //001xxxxx
        ledManager.setBrightness(((message & 0b00011111)<<3)+7);
        #if _COMM_DEBUG
          Serial.print("brightness: ");
          Serial.print(((message & 0b00011111)<<3)+7);
        #endif
      }
      else {                        //000xxxxx
        if(message >= 0b00000100) {   //00000100 -> 00011111
          ledManager.setEffectMode(message-4);
          #if _COMM_DEBUG
            Serial.print("effectMode: ");
            Serial.print(message-4);
          #endif
        }
        else {                        //000000xx
          if(message >= 0b00000010) {   //0000001x
            ledManager.setExtruderLedsOn(message & 0b00000001);
            #if _COMM_DEBUG
              Serial.print("extruderLedsOn: ");
              Serial.print(message & 0b00000001);
            #endif
          }
          else {                        //0000000x
            ledManager.setChamberLedsOn(message);
            #if _COMM_DEBUG
              Serial.print("ledsOn: ");
              Serial.print(message);
            #endif
          }
        }
      }
    }
  }
  #if _COMM_DEBUG
    Serial.println();
  #endif
  return 0;
}

void Communication::handleExtraData() {
  if(Serial.available()<awaitingData)
    return;

  #if _COMM_DEBUG
    Serial.print("\tExtra data (");
    Serial.print(awaitingData);
    Serial.print(" bytes):\t");
  #endif
  
  switch(awaitingData) {
    case 9:   printerStatus.dtb = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.dtb);
                Serial.print("\t");
              #endif
    case 8:   printerStatus.dtc = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.dtc);
                Serial.print("\t");
              #endif
    case 7:   printerStatus.dte = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.dte);
                Serial.print("\t");
              #endif
    case 6:   printerStatus.tb = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.tb);
                Serial.print("\t");
              #endif
    case 5:   printerStatus.tc = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.tc);
                Serial.print("\t");
              #endif
    case 4:   printerStatus.te = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.te);
                Serial.print("\t");
              #endif
    case 3:   printerStatus.y = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.y);
                Serial.print("\t");
              #endif
    case 2:   printerStatus.z = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.z);
                Serial.print("\t");
              #endif
    case 1:   printerStatus.x = Serial.read();
              #if _COMM_DEBUG
                Serial.print(printerStatus.x);
                Serial.print("\t");
              #endif
    default:  awaitingData=0;
              printerStatus.setNewPrinterStatus(awaitingStatus);
  }
}

/*
  00000000  0 lights off
  00000001  1 lights on
  ------------------------------------------------------------------------------------------------------------
  00000010  2 extruder lights off
  00000011  3 extruder lights on
  ------------------------------------------------------------------------------------------------------------
  00000100  4 lights mode 0
  000xxxxx  n lights mode n-4
  00011111  31  lights mode 27
  ------------------------------------------------------------------------------------------------------------
  00100000  32  global brightness 7
  00100001  33  global brightness 15
  001xxxxx  n   global brightness (n-32)*8+7
  00111111  63  global brightness 255
  ------------------------------------------------------------------------------------------------------------
  01000000  64  hue 0
  01000001  65  hue 4
  01xxxxxx  n   hue (n-64)*4
  01111111  127 hue 252 
  ------------------------------------------------------------------------------------------------------------
  10000000  128 I   Idle            
  10000001  129 P   Printing                                              z   x
  10000010  130 S   Paused            
  10000011  131 C   Changing filament                             te  y   z   x
  10000100  132 E   Error           
  10000101  133 W   Temperature warning                   tb  tc  te  y   z   x
  10000110  134 H   Halted (emergency stop)   
  10000111  135 Q   Heated                    dtb dtc dte tb  tc  te  y   z   x 
  10001000  136 F   Firmware update       
  10001001  137 B   Busy (performing macro)   dtb dtc dte tb  tc  te  y   z   x  
  10001010  138 Undefined
  10001011  139 Undefined
  10001100  140 Undefined
  10001101  141 Undefined
  10001110  142 Undefined
  10001111  143 Undefined
  ------------------------------------------------------------------------------------------------------------
  10010000  144 Undefined
  10010001  145 Load default settings
  10010010  146 Load stored settings
  10010011  147 Store current settings
  ------------------------------------------------------------------------------------------------------------
  10010100  148 Unschedule reboot
  10010101  149 Schedule reboot
  ------------------------------------------------------------------------------------------------------------
  10010110  150 Unschedule shutdown
  10010111  151 Schedule shutdown
  ------------------------------------------------------------------------------------------------------------  
  10011000  152 Fading duration 0ms               0         (<<9)   
  10011001  153 Fading duration 512ms             1         (<<9)
  10011xxx  n   Fading duration (n-152)*2^9 ms    n-152     (<<9)
  10011111  159 Fading duration 3584ms            7         (<<9)
  ------------------------------------------------------------------------------------------------------------    
  10100000  160 Effect duration 8192ms            1         (<<13)
  10100001  161 Effect duration 16384ms           2         (<<13)
  1010xxxx  n   Effect duration (n-159)*2^13 ms   n-159     (<<13)
  10101111  175 Effect duratiom 131072ms          16        (<<13)
  10110000  176 Effect duration 163840ms          20        (<<13)
  1011xxxx  n   Effect duration (n-171)*2^15 ms   4*(n-171) (<<13)
  10111111  191 Effect duration 655360 ms         80        (<<13)
  ------------------------------------------------------------------------------------------------------------
  11000000  192 Saturation 0
  1000xxxx  n   Saturation (n-192)*17
  11001111  207 Saturation 255
  ------------------------------------------------------------------------------------------------------------  
  11010000  208 Undefined
  11010001  209 Undefined
  11010010  210 Extruder leds as chamber                  2
  11010011  211 Extruder leds complementary to chamber    3
  11010100  212 Extruder leds white                       0
  11010101  213 Extruder leds random hue                  1
  11010110  214 Disable random hue color: hsv (rand, 255, 255); last color will be kept until next change
  11010111  215 Enable random hue color: hsv (rand, 255, 255); color changes every fading
  11011000  216 Disable random effect mode; last effect mode will be kept until next change
  11011001  217 Enable random effect mode; effect mode changes after every fading
  11011010  218 Disable random fading mode; last fading mode will be kept until next change
  11011011  219 Enable random fading mode; fading mode changes after every fading
  11011100  220 Disable random effect duration; last effect duration will be kept until next change
  11011101  221 Enable random effect duration; effect duration changes every fading
  11011110  222 Disable random fading duration; last fading duration will be kept until next change
  11011111  223 Enable random fading duration; fading duration changes every fading
  ------------------------------------------------------------------------------------------------------------
  11100000  224 Fading mode 0
  111xxxxx  n   Fading mode n-224
  11111111  255 Fading mode 31
*/
 
