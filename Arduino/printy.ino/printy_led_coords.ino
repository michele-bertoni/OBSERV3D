/***************************************************************************************************
 * Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        *
 * You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    *
 * You can find a copy of the license inside the LICENSE file you received with this code          *
 * (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     *
 * or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       *
 ***************************************************************************************************/

#include <avr/pgmspace.h>

#define _COORD_MODE 1

#define X0A 190
#define Y0A 114
#define Z0A 189
#define X0B 217
#define Y1B 164
#define Z1B 218
#define X2C 25
#define Y2C 67
#define Z2C 24
#define X2D 41
#define Y3D 113
#define Z3D 42
#define Z0E 165
#define Z1E 242
#define Z2E 0
#define Z3E 66

uint8_t getNext(uint8_t currentLed, uint8_t precLed) {
  switch(currentLed) {
    case Z2C: if(precLed == Z2C-1) return random8(2) ? X2C : Y2C;
              else return Z2C-1;
    case X2C: if(precLed == X2C+1) return random8(2) ? Y2C : Z2C;
              else return X2C+1;
    case Y2C: if(precLed == Y2C+1) return random8(2) ? Z2C : X2C;
              else return Y2C+1;
    case X2D: if(precLed == X2D-1) return random8(2) ? Z3D : Y3D;
              else return X2D-1;
    case Z3D: if(precLed == Z3D+1) return random8(2) ? Y3D : X2D;
              else return Z3D+1;
    case Y3D: if(precLed == Y3D-1) return random8(2) ? X2D : Z3D;
              else return Y3D-1;
    case Z2E: if(precLed == Z2E+1) return random8(2) ? Z0E : Z1E;
              else return Z2E+1;
    case Z3E: if(precLed == Z3E-1) return random8(2) ? Z1E : Z0E;
              else return Z3E-1;
    case Z0A: if(precLed == Z0A-1) return random8(2) ? X0A : Y0A;
              else return Z0A-1;
    case X0A: if(precLed == X0A+1) return random8(2) ? Y0A : Z0A;
              else return X0A+1;
    case Y0A: if(precLed == Y0A+1) return random8(2) ? Z0A : X0A;
              else return Y0A+1;
    case X0B: if(precLed == X0B-1) return random8(2) ? Z1B : Y1B;
              else return X0B-1;
    case Z1B: if(precLed == Z1B+1) return random8(2) ? Y1B : X0B;
              else return Z1B+1;
    case Y1B: if(precLed == Y1B-1) return random8(2) ? X0B : Z1B;
              else return Y1B-1;
    case Z0E: if(precLed == Z0E+1) return random8(2) ? Z3E : Z2E;
              else return Z0E+1;
    case Z1E: if(precLed == Z1E-1) return random8(2) ? Z2E : Z3E;
              else return Z1E-1;
    default:  return precLed<=currentLed ? currentLed+1 : currentLed-1;
  }
}

#if _COORD_MODE
const uint8_t led_coord_x[] PROGMEM = { 
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,                //z2
   71, 78, 85, 92,100,107,114,121,128,135,142,149,156,164,171,178,185,                                                //x2
  192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,                //z3
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,                                                        //y2
   71, 78, 85, 92,100,107,114,121,128,135,142,149,156,164,171,178,185,                                                //x3
  192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,                                                        //y3
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,                                                                //y0
   43, 50, 57, 64, 71, 78, 85, 92,100,107,114,121,128,135,142,149,156,164,171,178,185,192,199,206,213,                //x1
  231,231,231,231,231,231,231,231,231,231,231,231,231,                                                                //y1
   18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,                //z0
   32, 39, 46, 53, 60, 68, 75, 82, 89, 96,103,110,117,124,132,139,146,153,160,167,174,181,188,196,203,210,217,224,    //x0
  238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,238,                //z1
};

const uint8_t led_coord_y[] PROGMEM = { 
  117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,                //z2
  117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,                                                //x2
  117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,117,                //z3
  110,103, 96, 89, 82, 75, 68, 60, 53, 46, 39, 32, 25, 18, 11,                                                        //y2
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,                                                //x3
   11, 18, 25, 32, 39, 46, 53, 60, 68, 75, 82, 89, 96,103,110,                                                        //y3
  238,231,224,217,210,203,196,188,181,174,167,160,153,                                                                //y0
  132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,                //x1
  153,160,167,174,181,188,196,203,210,217,224,231,238,                                                                //y1
  245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,                //z0
  245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,    //x0
  245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,245,                //z1
};

const uint8_t led_coord_z[] PROGMEM = { 
  217,210,203,196,188,181,174,167,160,153,146,139,132,124,117,110,103, 96, 89, 82, 75, 68, 60, 53, 46,                //z2
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,                                                //x2
   46, 53, 60, 68, 75, 82, 89, 96,103,110,117,124,132,139,146,153,160,167,174,181,188,196,203,210,217,                //z3
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,                                                        //y2
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,                                                //x3
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,                                                        //y3
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,                                                                //y0
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,                //x1
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,                                                                //y1
  217,210,203,196,188,181,174,167,160,153,146,139,132,124,117,110,103, 96, 89, 82, 75, 68, 60, 53, 46, 39,            //z0
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,    //x0
   39, 46, 53, 60, 68, 75, 82, 89, 96,103,110,117,124,132,139,146,153,160,167,174,181,188,196,203,210,217             //z1
};

uint8_t getLedX(uint8_t ledNum) {
  return pgm_read_byte_near(led_coord_x + ledNum);
}

uint8_t getLedY(uint8_t ledNum) {
  return pgm_read_byte_near(led_coord_y + ledNum);
}

uint8_t getLedZ(uint8_t ledNum) {
  return pgm_read_byte_near(led_coord_z + ledNum);
}

#else
const uint8_t coord[] PROGMEM = { 
  217,210,203,196,188,181,174,167,160,153,146,139,132,124,117,110,103, 96, 89, 82, 75, 68, 60, 53, 46,                //z2
   71, 78, 85, 92,100,107,114,121,128,135,142,149,156,164,171,178,185,                                                //x2
   46, 53, 60, 68, 75, 82, 89, 96,103,110,117,124,132,139,146,153,160,167,174,181,188,196,203,210,217,                //z3
  110,103, 96, 89, 82, 75, 68, 60, 53, 46, 39, 32, 25, 18, 11,                                                        //y2
   71, 78, 85, 92,100,107,114,121,128,135,142,149,156,164,171,178,185,                                                //x3
   11, 18, 25, 32, 39, 46, 53, 60, 68, 75, 82, 89, 96,103,110,                                                        //y3
  238,231,224,217,210,203,196,188,181,174,167,160,153,                                                                //y0
   43, 50, 57, 64, 71, 78, 85, 92,100,107,114,121,128,135,142,149,156,164,171,178,185,192,199,206,213,                //x1
  153,160,167,174,181,188,196,203,210,217,224,231,238,                                                                //y1
  217,210,203,196,188,181,174,167,160,153,146,139,132,124,117,110,103, 96, 89, 82, 75, 68, 60, 53, 46,                //z0
   32, 39, 46, 53, 60, 68, 75, 82, 89, 96,103,110,117,124,132,139,146,153,160,167,174,181,188,196,203,210,217,224,    //x0
   46, 53, 60, 68, 75, 82, 89, 96,103,110,117,124,132,139,146,153,160,167,174,181,188,196,203,210,217                 //z1
};

uint8_t getCoord(uint8_t ledNum) {
  return pgm_read_byte_near(coord + ledNum);
}

uint8_t getX(uint8_t ledNum) {
  if(ledNum <= 115) {
    if(ledNum <= 42) {
      if(ledNum <= 25) {
        return 64;                //z2
      }
      else {
        return getCoord(ledNum);  //x2
      }
    }
    else {
      if(ledNum <= 83) {
        if(ledNum <= 68) {
          return 192;             //z3
        }
        else {
          return 64;              //y2
        }
      }
      else {
        if(ledNum <= 100) {
          return getCoord(ledNum);//x3
        }
        else {
          return 192;             //y3
        }
      }
    }
  }
  else {
    if(ledNum <= 192) {
      if(ledNum <= 153) {
        if(ledNum <= 128) {
          return 25;              //y0
        }
        else {
          return getCoord(ledNum);//x1
        }
      }
      else {
        if(ledNum <= 166) {
          return 231;             //y1
        }
        else {
          return 18;              //z0
        }
      }
    }
    else {
      if(ledNum <= 220) {
        return getCoord(ledNum);  //x0
      }
      else {
        return 238;               //z1
      }
    }
  }
}

uint8_t getY(uint8_t ledNum) {
  if(ledNum <= 115) {
    if(ledNum <= 68) {
      return 117;                 //z2 x2 z3
    }
    else {
      if(ledNum <= 83 || ledNum >= 101) {
        return getCoord(ledNum);  //y2 y3
      }
      else {
        return 4;                 //x3
      }
    }
  }
  else {
    if(ledNum <= 166) {
      if(ledNum <= 128 || ledNum >= 154) {
        return getCoord(ledNum);  //y0 y1
      }
      else {
        return 132;               //x1
      }
    }
    else {
      return 245;                 //z0 x0 z1
    }
  }
}

uint8_t getZ(uint8_t ledNum) {
  if(ledNum <= 115) {
    if(ledNum <= 68) {
      if(ledNum <= 25 || ledNum >=43) {
        return getCoord(ledNum);  //z2 z3 
      }
      else {
        return 25;                //x2
      }
    }
    else {
      return 25;                  //y2 x3 y3
    }
  }
  else {
    if(ledNum <= 166) {
      return 25;                  //y0 x1 y1
    }
    else {
      if(ledNum <= 192 || ledNum >= 221) {
        return getCoord(ledNum);  //z0 z1
      }
      else {
        return 25;                //x0
      }
    }
  }
}
#endif
