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
  if(message >= 0b10000000) {   //1xxxxxxx  [128, 255]
    if(message >= 0b11000000) {   //11xxxxxx  [192, 255]
      if(message >= 0b11100000) {   //111xxxxx  [224, 255]
        ledManager.setFadingMode(message & 0b00011111);
        #if _COMM_DEBUG
          Serial.print("fadingMode: ");
          Serial.print(message & 0b00011111);
        #endif
      }
      else {                        //110xxxxx  [192, 223]
        if(message >= 0b11010000) {   //1101xxxx  [208, 223]
          if(message >= 0b11011000) {   //11011xxx  [216, 223]
            #if _COMM_DEBUG
              Serial.print("Undefined");
            #endif
          }
          else {                        //11010xxx  [208, 215]                       
            if(message >= 11010100) {     //110101xx  [212, 215]
              #if _COMM_DEBUG
                Serial.print("Undefined");
              #endif 
            }
            else {                        //110100xx  [208, 211] 
              if(message >= 0b11010110) {   //1101001x  [210, 211]
                ledManager.setEditExtruderHSV(message & 0b00000001);
                #if _COMM_DEBUG
                  Serial.print("editExtruderHSV: ");
                  Serial.print(message & 0b00000001);
                #endif
              }
              else {                        //1101000x  [208, 209]
                printerStatus.setKeepRaspberryOn(message & 0b00000001);
                #if _COMM_DEBUG
                  Serial.print("keepRaspberryOn: ");
                  Serial.print(message & 0b00000001);
                #endif
              }
            }
          }
        }
        else {                        //1100xxxx  [192, 207]
          ledManager.setSaturation ((message & 0b00001111)*17);
          #if _COMM_DEBUG
            Serial.print("saturation: ");
            Serial.print((message & 0b00001111)*17);
          #endif
        }
      }
    }
    else {                        //10xxxxxx  [128, 191]
      if(message >= 0b10100000) {   //101xxxxx  [160, 191]
        ledManager.setEffectDuration(message >= 0b10110000 ? (message-171)<<2 : (message-159));
        #if _COMM_DEBUG
          Serial.print("effectDuration: ");
          Serial.print(message >= 0b10110000 ? (message-171)<<2 : (message-159));
        #endif    
      }
      else {                        //100xxxxx  [128, 159]
        if(message >= 0b10010000) {   //1001xxxx  [144, 159]
          if(message >= 0b10011000) {   //10011xxx  [152, 159]
            ledManager.setFadingDuration(message & 0b00000111);
            #if _COMM_DEBUG
              Serial.print("fadingDuration: ");
              Serial.print(message & 0b00000111);
            #endif   
          }
          else {                        //10010xxx  [144, 151]
            if(message >= 0b10010100) {   //100101xx  [148, 151]
              if(message >= 0b10010110) {   //1001011x  [150, 151]
                printerStatus.schedulePowerOff(message & 0b00000001);
                printerStatus.scheduleReboot(false);
                #if _COMM_DEBUG
                  Serial.print(message & 0b00000001 ? "Schedule shutdown" : "Unschedule shutdown");
                #endif
              }
              else {                        //1001011x  [148, 149]
                printerStatus.scheduleReboot(message & 0b00000001);
                printerStatus.schedulePowerOff(message & 0b00000001);
                #if _COMM_DEBUG
                  Serial.print(message & 0b00000001 ? "Scheduled reboot" : "Unscheduled reboot");
                #endif
              }
            }
            else {                        //100100xx  [144, 147]
              if(message >= 0b10010010) {   //1001001x  [146, 147]
                if(message >= 0b10010011) {   //10010011  147
                  ledManager.storeSettings();
                  printerStatus.storeSettings();
                  #if _COMM_DEBUG
                    Serial.print("Stored current settings");
                  #endif
                }
                else {                        //10010010  146
                  ledManager.loadSettings();
                  printerStatus.loadSettings();
                  #if _COMM_DEBUG
                    Serial.print("Loaded stored settings");
                  #endif
                }
              }
              else {                        //1001000x  [144, 145]
                if(message >= 0b10010001) {   //10010001  145
                  ledManager.resetSettings();
                  printerStatus.resetSettings();
                  #if _COMM_DEBUG
                    Serial.print("Loaded default settings");
                  #endif
                }
                else {                        //10010000  144
                  #if _COMM_DEBUG
                    Serial.print("Undefined");
                  #endif 
                }
              }
            }
          }
        }
        else {                        //1000xxxx  [128, 143]
          if(message <= 0b10001001) {   //10000000 -> 10001001  [128, 137]
            #if _COMM_DEBUG
              Serial.print("Printer status: ");
              Serial.print((message & 0b00001111)+3);
            #endif
            if(message & 0b00000001) {
              awaitingStatus = (message & 0b00001111)+3;
              return (message==0b10000111 || message==0b10001001) ? 9 : (message&0b00001111)+1;
            }
            else {
              if(printerStatus.getPrinterStatus()!=STATUS_D && printerStatus.getNewPrinterStatus()!=STATUS_D) {
                printerStatus.setNewPrinterStatus((message & 0b00001111)+3);
              }
            }
          }
          else {                        //10001010 -> 10001111  [138, 143]
            #if _COMM_DEBUG
              Serial.print("Undefined");
            #endif 
          }
        }
      }
    }
  }
  else {                        //0xxxxxxx  [0, 127]
    if(message >= 0b01000000) {   //01xxxxxx  [64, 127]
      ledManager.setHue((message & 0b00111111)<<2);
      #if _COMM_DEBUG
        Serial.print("hue: ");
        Serial.print((message & 0b00111111)<<2);
      #endif
    }
    else {                        //00xxxxxx  [0, 63]
      if(message >= 0b00100000) {   //001xxxxx  [32, 63]
        ledManager.setBrightness(((message & 0b00011111)<<3)+7);
        #if _COMM_DEBUG
          Serial.print("brightness: ");
          Serial.print(((message & 0b00011111)<<3)+7);
        #endif
      }
      else {                        //000xxxxx  [0, 31]
        if(message >= 0b00000100) {   //00000100 -> 00011111  [4, 31]
          ledManager.setEffectMode(message-4);
          #if _COMM_DEBUG
            Serial.print("effectMode: ");
            Serial.print(message-4);
          #endif
        }
        else {                        //000000xx  [0, 3]
          if(message >= 0b00000010) {   //0000001x  [2, 3]
            ledManager.setExtruderLedsOn(message & 0b00000001);
            #if _COMM_DEBUG
              Serial.print("extruderLedsOn: ");
              Serial.print(message & 0b00000001);
            #endif
          }
          else {                        //0000000x  [0, 1]
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
              if(printerStatus.getPrinterStatus()!=STATUS_D && printerStatus.getNewPrinterStatus()!=STATUS_D) {
                printerStatus.setNewPrinterStatus(awaitingStatus);
              }
  }
}

/*
  00000000  0   chamber lights off
  00000001  1   chamber lights on
  ------------------------------------------------------------------------------------------------------------
  00000010  2   extruder lights off
  00000011  3   extruder lights on
  ------------------------------------------------------------------------------------------------------------
  00000100  4   lights mode 0
  000xxxxx  n   lights mode n-4
  00011111  31  lights mode 27
  ------------------------------------------------------------------------------------------------------------
  00100000  32  brightness 7
  00100001  33  brightness 15
  001xxxxx  n   brightness (n-32)*8+7
  00111111  63  brightness 255
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
  11010000  208 Switch off Raspberry when the printer is off
  11010001  209 Keep the Raspberry always on
  ------------------------------------------------------------------------------------------------------------
  11010010  210 Following commands will edit chamber HSV color
  11010011  211 Following commands will edit extruder HSV color
  ------------------------------------------------------------------------------------------------------------
  110101xx  n   Undefined
  ------------------------------------------------------------------------------------------------------------
  11011xxx  n   Undefined
  ------------------------------------------------------------------------------------------------------------
  11100000  224 Fading mode 0
  111xxxxx  n   Fading mode n-224
  11111111  255 Fading mode 31
*/
 
