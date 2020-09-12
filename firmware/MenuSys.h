#include <EEPROM.h>
#include "Control.h"

#define BUTTON_W_XL 48

#define BUTTON_W_L 32
#define BUTTON_H_L 16

#define BUTTON_W_M 24
#define BUTTON_H_M 12

#define BUTTON_W_S 19
#define BUTTON_H_S 9

#define SCROLL_THRESH 7
#define SCROLL_PERIOD 6
#define SCROLL_ACCEL 1
#define SCROLL_MAX 5

#define SCROLL_THRESH16 7
#define SCROLL_PERIOD16 6
#define SCROLL_ACCEL16 2
#define SCROLL_MAX16 6

//#define WIDTH 128
//#define HEIGHT 64

#define MAIN_MENU     0
#define SETTINGS_MENU 1
#define REFLOW_MENU   2
#define PROFILES_MENU 3
#define DIALOGUE_MENU 4
#define SCREEN_SAVER  5

#define WRAP      0b1
#define INVERSE_Y 0b10
#define MOVED     0b100

// Profiles menu state 0
#define ADJUST   0b1
#define SELECT   0b10
#define ADD      0b100
#define DEL      0b1000
#define SAVE     0b10000
#define CHOOSE   0b100000
#define R2ZERO   0b1000000
#define ADJ_AXIS 0b10000000

// Profiles menu state 1
#define UNLOADED       0b1
#define CHANGED        0b10
#define INIT_LOADED    0b100
#define DISPLAY_BUFFER 0b1000
#define ADJ_X_AXIS     0b10000
#define ADJ_Y_AXIS     0b100000

// Reflow menu state
#define REFLOW_IDLE   0b10
#define REFLOW_ACTIVE 0b100
#define LOG_TEMP      0b1000


// This includes the number of points, the x and y axis range, and all the points.
// All of these are unsigned 16-bit integers, so the number of bytes is exactly twice
// the array length.
#define PROFILE_LENGTH 35

#define DEFAULT_PROFILE_LENGTH 15

// This is a byte array that indicated whether each profile slot has been written to
#define PROFILES_HEADER 8

// While modifying a profile, the points (and temp points) will be loaded into a buffer so they can be freely moved around.
// This buffer will be the same as the one that logs temperature over time, since these two actions won't
// happen simultaneously
uint16_t ttbuffer[128];

const uint16_t defaultProfile[] PROGMEM = {
  // x axis/2, y axis/4
  6,
  256,
  256,
  0, 25,
  30, 100,
  120, 150,
  150, 183,
  210, 235,
  240, 183
};

byte currentMenu = MAIN_MENU;
byte prevMenu = MAIN_MENU;
byte buttonxy = 0;
uint16_t anim = 0;
uint16_t idleTime = 0;
uint16_t prevMove = 0;
byte accel = 0;

void updatePos(byte * x, byte * y, byte maxx, byte maxy, byte wrapORysignORmoved) {

  // JUST_PRESSED interactivity
  // This if else if structure ensures that pressing up/down
  // or left/right at the same time is ignored but other combinations are not

  if (maxy > 0) {
    if (buttons[ARR_UP] & JUST_PRESSED == 1) {
      wrapORysignORmoved |= MOVED;
      if ((wrapORysignORmoved & INVERSE_Y) > 0) {
        *y -= 1;
        if ((wrapORysignORmoved & WRAP) == 1) {
          if (*y == 255) {
            *y = maxy;
          }
        } else {
          if (*y == 255) {
            *y = 0;
          }
        }
      } else {
        *y += 1;
        if ((wrapORysignORmoved & WRAP) == 1) {
          if (*y > maxy) {
            *y = 0;
          }
        } else {
          if (*y > maxy) {
            *y = maxy;
          }
        }
      }
    } else if (buttons[ARR_DOWN] & JUST_PRESSED == 1) {
      wrapORysignORmoved |= MOVED;
      if ((wrapORysignORmoved & INVERSE_Y) == 0) {
        *y -= 1;
        if ((wrapORysignORmoved & WRAP) == 1) {
          if (*y == 255) {
            *y = maxy;
          }
        } else {
          if (*y == 255) {
            *y = 0;
          }
        }
      } else {
        *y += 1;
        if ((wrapORysignORmoved & WRAP) == 1) {
          if (*y > maxy) {
            *y = 0;
          }
        } else {
          if (*y > maxy) {
            *y = maxy;
          }
        }
      }
    }

    // HELD INTERACTIVITY
    if ((buttons[ARR_UP] & HELD) > 0) {
      if (buttonTimes[ARR_UP] > SCROLL_THRESH && (buttonTimes[ARR_UP] - prevMove) > SCROLL_PERIOD - accel) {
        wrapORysignORmoved |= MOVED;
        prevMove = buttonTimes[ARR_UP];
        accel += SCROLL_ACCEL;
        if (accel > SCROLL_MAX) {
          accel = SCROLL_MAX;
        }

        if ((wrapORysignORmoved & INVERSE_Y) > 0) {
          *y -= 1;
          if ((wrapORysignORmoved & WRAP) > 0) {
            if (*y == 255) {
              *y = maxy;
            }
          } else {
            if (*y == 255) {
              *y = 0;
            }
          }
        } else {
          *y += 1;
          if ((wrapORysignORmoved & WRAP) > 0) {
            if (*y > maxy) {
              *y = 0;
            }
          } else {
            if (*y > maxy) {
              *y = maxy;
            }
          }
        }
      }

    } else if ((buttons[ARR_DOWN] & HELD) > 0) {
      if (buttonTimes[ARR_DOWN] > SCROLL_THRESH && (buttonTimes[ARR_DOWN] - prevMove) > SCROLL_PERIOD - accel) {
        wrapORysignORmoved |= MOVED;
        prevMove = buttonTimes[ARR_DOWN];
        accel += SCROLL_ACCEL;
        if (accel > SCROLL_MAX) {
          accel = SCROLL_MAX;
        }

        if ((wrapORysignORmoved & INVERSE_Y) == 0) {
          *y -= 1;
          if ((wrapORysignORmoved & WRAP) > 0) {
            if (*y == 255) {
              *y = maxy;
            }
          } else {
            if (*y == 255) {
              *y = 0;
            }
          }
        } else {
          *y += 1;
          if ((wrapORysignORmoved & WRAP) > 0) {
            if (*y > maxy) {
              *y = 0;
            }
          } else {
            if (*y > maxy) {
              *y = maxy;
            }
          }
        }
      }
    }
  }

  if (maxx > 0) {
    if (buttons[ARR_LEFT] & JUST_PRESSED == 1) {
      wrapORysignORmoved |= MOVED;
      *x -= 1;
      // Menu wrapping functionality
      if ((wrapORysignORmoved & WRAP) > 0) {
        if (*x == 255) {
          *x = maxx;
        }
      } else {
        if (*x == 255) {
          *x = 0;
        }
      }
    } else if (buttons[ARR_RIGHT] & JUST_PRESSED == 1) {
      wrapORysignORmoved |= MOVED;
      *x += 1;
      // Menu wrapping functionality
      if ((wrapORysignORmoved & WRAP) > 0) {
        if (*x > maxx) {
          *x = 0;
        }
      } else {
        if (*x > maxx) {
          *x = maxx;
        }
      }
    }

    // HELD INTERACTIVITY
    if ((buttons[ARR_LEFT] & HELD) > 0) {
      if (buttonTimes[ARR_LEFT] > SCROLL_THRESH && (buttonTimes[ARR_LEFT] - prevMove) > SCROLL_PERIOD - accel) {
        wrapORysignORmoved |= MOVED;
        prevMove = buttonTimes[ARR_LEFT];
        accel += SCROLL_ACCEL;
        if (accel > SCROLL_MAX) {
          accel = SCROLL_MAX;
        }

        *x -= 1;
        if ((wrapORysignORmoved & WRAP) > 0) {
          if (*x == 255) {
            *x = maxx;
          }
        } else {
          if (*x == 255) {
            *x = 0;
          }
        }
      }
    } else if ((buttons[ARR_RIGHT] & HELD) > 0) {
      if (buttonTimes[ARR_RIGHT] > SCROLL_THRESH && (buttonTimes[ARR_RIGHT] - prevMove) > SCROLL_PERIOD - accel) {
        wrapORysignORmoved |= MOVED;
        prevMove = buttonTimes[ARR_RIGHT];
        accel += SCROLL_ACCEL;
        if (accel > SCROLL_MAX) {
          accel = SCROLL_MAX;
        }

        *x += 1;
        if ((wrapORysignORmoved & WRAP) > 0) {
          if (*x > maxx) {
            *x = 0;
          }
        } else {
          if (*x > maxx) {
            *x = maxx;
          }
        }
      }
    }
  }

  if ((wrapORysignORmoved & MOVED) > 0) {
    beginSequence(blip);
    anim = 0;
    idleTime = 0;
  } else {
    prevMove = 0;
    accel = 0;
    idleTime++;
  }
}

// hacky solution for now to make it work with bigger types
void updatePos(uint16_t * x, uint16_t * y, uint16_t maxx, uint16_t maxy, byte wrapORysignORmoved) {

  // JUST_PRESSED interactivity
  // This if else if structure ensures that pressing up/down
  // or left/right at the same time is ignored but other combinations are not
  //  uint16_t 65535 = 65535;
  //  if (isByte == 1){
  //    65535 = 255;
  //  }

  if (maxy > 0) {
    if (buttons[ARR_UP] & JUST_PRESSED == 1) {
      wrapORysignORmoved |= MOVED;
      if ((wrapORysignORmoved & INVERSE_Y) > 0) {
        *y -= 1;
        if ((wrapORysignORmoved & WRAP) == 1) {
          if (*y == 65535) {
            *y = maxy;
          }
        } else {
          if (*y == 65535) {
            *y = 0;
          }
        }
      } else {
        *y += 1;
        if ((wrapORysignORmoved & WRAP) == 1) {
          if (*y > maxy) {
            *y = 0;
          }
        } else {
          if (*y > maxy) {
            *y = maxy;
          }
        }
      }
    } else if (buttons[ARR_DOWN] & JUST_PRESSED == 1) {
      wrapORysignORmoved |= MOVED;
      if ((wrapORysignORmoved & INVERSE_Y) == 0) {
        *y -= 1;
        if ((wrapORysignORmoved & WRAP) == 1) {
          if (*y == 65535) {
            *y = maxy;
          }
        } else {
          if (*y == 65535) {
            *y = 0;
          }
        }
      } else {
        *y += 1;
        if ((wrapORysignORmoved & WRAP) == 1) {
          if (*y > maxy) {
            *y = 0;
          }
        } else {
          if (*y > maxy) {
            *y = maxy;
          }
        }
      }
    }

    // HELD INTERACTIVITY
    if ((buttons[ARR_UP] & HELD) > 0) {
      if ((buttonTimes[ARR_UP] > SCROLL_THRESH16 && (buttonTimes[ARR_UP] - prevMove) >= SCROLL_PERIOD16 - accel) || buttonTimes[ARR_UP] > SCROLL_THRESH16 * 2) {
        wrapORysignORmoved |= MOVED;
        prevMove = buttonTimes[ARR_UP];
        accel += SCROLL_ACCEL16;
        if (accel > SCROLL_MAX16) {
          accel = SCROLL_MAX16;
        }

        if ((wrapORysignORmoved & INVERSE_Y) > 0) {
          *y -= 1;
          if ((wrapORysignORmoved & WRAP) > 0) {
            if (*y == 65535) {
              *y = maxy;
            }
          } else {
            if (*y == 65535) {
              *y = 0;
            }
          }
        } else {
          *y += 1;
          if ((wrapORysignORmoved & WRAP) > 0) {
            if (*y > maxy) {
              *y = 0;
            }
          } else {
            if (*y > maxy) {
              *y = maxy;
            }
          }
        }
      }

    } else if ((buttons[ARR_DOWN] & HELD) > 0) {
      if ((buttonTimes[ARR_DOWN] > SCROLL_THRESH16 && (buttonTimes[ARR_DOWN] - prevMove) > SCROLL_PERIOD16 - accel)  || buttonTimes[ARR_DOWN] > SCROLL_THRESH16 * 2) {
        wrapORysignORmoved |= MOVED;
        prevMove = buttonTimes[ARR_DOWN];
        accel += SCROLL_ACCEL16;
        if (accel > SCROLL_MAX16) {
          accel = SCROLL_MAX16;
        }

        if ((wrapORysignORmoved & INVERSE_Y) == 0) {
          *y -= 1;
          if ((wrapORysignORmoved & WRAP) > 0) {
            if (*y == 65535) {
              *y = maxy;
            }
          } else {
            if (*y == 65535) {
              *y = 0;
            }
          }
        } else {
          *y += 1;
          if ((wrapORysignORmoved & WRAP) > 0) {
            if (*y > maxy) {
              *y = 0;
            }
          } else {
            if (*y > maxy) {
              *y = maxy;
            }
          }
        }
      }
    }
  }

  if (maxx > 0) {
    if (buttons[ARR_LEFT] & JUST_PRESSED == 1) {
      wrapORysignORmoved |= MOVED;
      *x -= 1;
      // Menu wrapping functionality
      if ((wrapORysignORmoved & WRAP) > 0) {
        if (*x == 65535) {
          *x = maxx;
        }
      } else {
        if (*x == 65535) {
          *x = 0;
        }
      }
    } else if (buttons[ARR_RIGHT] & JUST_PRESSED == 1) {
      wrapORysignORmoved |= MOVED;
      *x += 1;
      // Menu wrapping functionality
      if ((wrapORysignORmoved & WRAP) > 0) {
        if (*x > maxx) {
          *x = 0;
        }
      } else {
        if (*x > maxx) {
          *x = maxx;
        }
      }
    }

    // HELD INTERACTIVITY
    if ((buttons[ARR_LEFT] & HELD) > 0) {
      if ((buttonTimes[ARR_LEFT] > SCROLL_THRESH16 && (buttonTimes[ARR_LEFT] - prevMove) > SCROLL_PERIOD16 - accel) || buttonTimes[ARR_LEFT] > SCROLL_THRESH16 * 2) {
        wrapORysignORmoved |= MOVED;
        prevMove = buttonTimes[ARR_LEFT];
        accel += SCROLL_ACCEL16;
        if (accel > SCROLL_MAX16) {
          accel = SCROLL_MAX16;
        }

        *x -= 1;
        if ((wrapORysignORmoved & WRAP) > 0) {
          if (*x == 65535) {
            *x = maxx;
          }
        } else {
          if (*x == 65535) {
            *x = 0;
          }
        }
      }
    } else if ((buttons[ARR_RIGHT] & HELD) > 0) {
      if ((buttonTimes[ARR_RIGHT] > SCROLL_THRESH && (buttonTimes[ARR_RIGHT] - prevMove) > SCROLL_PERIOD16 - accel) || buttonTimes[ARR_RIGHT] > SCROLL_THRESH16 * 2) {
        wrapORysignORmoved |= MOVED;
        prevMove = buttonTimes[ARR_RIGHT];
        accel += SCROLL_ACCEL16;
        if (accel > SCROLL_MAX16) {
          accel = SCROLL_MAX16;
        }

        *x += 1;
        if ((wrapORysignORmoved & WRAP) > 0) {
          if (*x > maxx) {
            *x = 0;
          }
        } else {
          if (*x > maxx) {
            *x = maxx;
          }
        }
      }
    }
  }

  if ((wrapORysignORmoved & MOVED) > 0) {
    beginSequence(blip);
    anim = 0;
    idleTime = 0;
  } else {
    prevMove = 0;
    accel = 0;
    idleTime++;
  }
}

void uiButtonConst(byte x, byte y, byte w, byte h, const byte* text, int t, byte xy, byte currentxy) {
  // NOTE -- length cannot be greater than 255 characters!
  byte textx = 1 + x + (w / 2) - (getLengthConst(text) * (currentFont.width + 1)) / 2;
  byte texty = y + (h / 2) - 2;
  if (currentxy == xy && (t & 32) == 0) {
    rect(x, y, w, h, WHITE);
    setTextColor(BLACK);
    printStringConst(text, textx , texty);
  } else {
    rectBorder(x, y, w, h, WHITE);
    setTextColor(WHITE);
    printStringConst(text, textx, texty);
  }
}

void uiButtonDyn(byte x, byte y, byte w, byte h, byte* text, int t, byte xy, byte currentxy) {
  // NOTE -- length cannot be greater than 255 characters!
  byte textx = 1 + x + (w / 2) - (getLengthDyn(text) * (currentFont.width + 1)) / 2;
  byte texty = y + (h / 2) - 2;
  if (currentxy == xy && (t & 32) == 0) {
    rect(x, y, w, h, WHITE);
    setTextColor(BLACK);
    printString(text, textx , texty);
  } else {
    rectBorder(x, y, w, h, WHITE);
    setTextColor(WHITE);
    printString(text, textx, texty);
  }
}

// ONLY WORKS WITH POSITIVE INTEGERS
void uiButtonVal(byte x, byte y, byte w, byte h, uint16_t val, int t, byte xy, byte currentxy) {
  // NOTE -- length cannot be greater than 255 characters!
  byte len = 1;
  if (val < 10) {
    len = 1;
  } else if (val < 100) {
    len = 2;
  } else if (val < 1000) {
    len = 3;
  } else if (val < 10000) {
    len = 4;
  } else {
    len = 5;
  }
  byte textx = 1 + x + (w / 2) - (len * (currentFont.width + 1)) / 2;
  byte texty = y + (h / 2) - 2;
  if (currentxy == xy && (t & 32) == 0) {
    rect(x, y, w, h, WHITE);
    setTextColor(BLACK);
    printVal(val, textx , texty);
  } else {
    rectBorder(x, y, w, h, WHITE);
    setTextColor(WHITE);
    printVal(val, textx, texty);
  }
}

void displayStats(uint16_t x, uint16_t y) {
  byte text[3] = {31, 'C', 0};
  setTextColor(WHITE);
  printString(text, 104, 48);
  printVal(y, 90, 48);
  text[0] = 'S';
  text[1] = ' ';
  setTextColor(WHITE);
  printString(text, 104, 55);
  printVal(x, 90, 55);
}

void loadTempProfile(uint16_t profile, byte profileVal, uint16_t* tempProfile) {
  if (EEPROM.read(profileVal) == 1) {
    for (byte i = 0; i < PROFILE_LENGTH; i++) {
      EEPROM.get(profile + i * 2, tempProfile[i]);
    }
  } else {
    for (byte i = 0; i < DEFAULT_PROFILE_LENGTH; i++) {
      tempProfile[i] = pgm_read_word_near(defaultProfile + i);
    }
  }
}

void saveTempProfile(uint16_t profile, byte profileVal,  uint16_t* tempProfile) {
  for (uint16_t i = 0; i < PROFILE_LENGTH; i++) {
    EEPROM.put(profile + i * 2, tempProfile[i]);
  }
  EEPROM.update(profileVal, 1);
}

void chooseProfile(byte *state, const byte* message, uint16_t *curprof, byte* curprofval) {
  byte tempstr[3];
  toDec(tempstr, (*curprofval) + 1);
  byte temptemp[] = "--";
  temptemp[0] = tempstr[1];
  temptemp[1] = tempstr[2];

  updatePos(curprofval, NULL, PROFILES_HEADER - 1, 0, WRAP);
  *curprof = *curprofval * PROFILE_LENGTH * 2 + PROFILES_HEADER;
  // should probably include some safety checks to make sure any corrupted profiles don't crash the
  // boy

  byte yoffset = 14;
  uiButtonConst(27, 32 - BUTTON_H_M / 2, 74, BUTTON_H_M, message, anim, 0, 0);
  uiButtonDyn(64 - BUTTON_W_S / 2, (32 + yoffset) - BUTTON_H_S / 2, BUTTON_W_S, BUTTON_H_S, temptemp, 0, 0, 0);
  byte xoff = 0;
  if ((anim & 32) > 0) {
    xoff = 1;
  }
  sprite(rotTriBit, 64 - BUTTON_W_S / 2 - 6 - xoff, (30 + yoffset), WHITE, FLIPX, 0);
  sprite(rotTriBit, 64 + BUTTON_W_S / 2 + 2 + xoff, (30 + yoffset), WHITE, 0, 0);
}

void profileToScreen(byte profile, byte point, byte* vector) {
  uint16_t x, y, maxx, maxy;
  // Point times four because the EEPROM is addressed on a byte by byte basis
  EEPROM.get(profile + point * 4 + 6, x);
  EEPROM.get(profile + point * 4 + 8, y);
  EEPROM.get(profile + 2, maxx);
  EEPROM.get(profile + 4, maxy);
  // if the user selects zero as maxx or maxy, the program will crash
  vector[0] = round(float(x) * 128.0 / float(maxx));
  vector[1] = 64 + round(float(y) * (-64.0) / float(maxy));
}

void logToScreen(uint16_t profile, byte index, byte* vector) {
  uint16_t y, maxy;
  // Point times four because the EEPROM is addressed on a byte by byte basis
  y = ttbuffer[index];
  EEPROM.get(profile + 4, maxy);
  // if the user selects zero as maxx or maxy, the program will crash
  vector[0] = index;
  vector[1] = 64 + round(float(y) * (-64.0) / float(maxy));
}

void updateTempLog(uint16_t temperature, uint16_t tim, uint16_t profile, byte reset) {
  static byte numPoints = 0;
  if (reset == 0) {
    uint16_t xscale = 256;
    EEPROM.get(profile + 2, xscale);
    byte trans = round(float(tim) * 128.0 / float(xscale));
    if (trans > numPoints && trans < 128) {
      // This should track with the mapped tim exactly unless
      // tim starts at a value other than zero
      ttbuffer[numPoints++] = temperature;
    }
    byte coords0[2];
    byte coords1[2];
    for (byte i = 1; i < numPoints; i++) {
      logToScreen(profile, i - 1, coords0);
      logToScreen(profile, i, coords1);
      line(coords0[0], coords0[1], coords1[0], coords1[1], WHITE);
    }
  }else{
    numPoints = 0;
  }
}

void defaultProfileToScreen(byte point, byte* vector) {
  float x = pgm_read_word_near(defaultProfile + point * 2 + 3);
  float y = pgm_read_word_near(defaultProfile + point * 2 + 4);
  float maxx = pgm_read_word_near(defaultProfile + 1);
  float maxy = pgm_read_word_near(defaultProfile + 2);
  vector[0] = round(x * 128.0 / maxx);
  vector[1] = 64 + round(y * (-64.0) / maxy);
}

void bufferProfileToScreen(uint16_t* buff, byte point, byte* vector) {
  float x = buff[point * 2 + 3];
  float y = buff[point * 2 + 4];
  float maxx = buff[1];
  float maxy = buff[2];
  vector[0] = round(x * 128.0 / maxx);
  vector[1] = 64 + round(y * (-64.0) / maxy);
}

void displayTicks(uint16_t xscale, uint16_t yscale, uint16_t xinterval, uint16_t yinterval) {
  float screenInterval = (float)xinterval * 128.0 / (float)xscale;
  for (byte i = 1; i < 128 / screenInterval - 1; i++) {
    line(i * screenInterval, 0, i * screenInterval, 3, WHITE);
  }

  screenInterval = (float)yinterval * 64.0 / (float)yscale;
  for (byte i = 1; i < 64 / screenInterval; i++) {
    line(125, 64 - i * screenInterval, 128, 64 - i * screenInterval, WHITE);
  }
}

void displayProfile(uint16_t profile, byte profileVal, uint16_t* buff, byte tempProfileState, byte includeLines) {
  // GRIDLINE NEED TO BE PROPORTIONAL TO AXES!!!
  byte p1[2];
  byte p2[2];
  if (tempProfileState == 1) {
    byte numPoints = buff[0];
    bufferProfileToScreen(buff, 0, p1);
    rect(p1[0] - 1, p1[1] - 1, 3, 3, (anim & 32) == 0);
    for (byte i = 1; i < numPoints; i++) {
      bufferProfileToScreen(buff, i - 1, p1);
      bufferProfileToScreen(buff, i, p2);
      if (includeLines == 1) {
        line(p1[0], p1[1], p2[0], p2[1], WHITE);
      }
      rect(p2[0] - 1, p2[1] - 1, 3, 3, (anim & 32) == 0);
    }
    displayTicks(buff[1], buff[2], 30, 50);
  } else {
    if (EEPROM.read(profileVal) == 1) {
      uint16_t numPoints;
      EEPROM.get(profile, numPoints);

      profileToScreen(profile, 0, p1);
      rect(p1[0] - 1, p1[1] - 1, 3, 3, (anim & 32) == 0);

      for (byte i = 1; i < numPoints; i++) {
        profileToScreen(profile, i - 1, p1);
        profileToScreen(profile, i, p2);
        rect(p2[0] - 1, p2[1] - 1, 3, 3, (anim & 32) == 0);
        if (includeLines == 1) {
          line(p1[0], p1[1], p2[0], p2[1], WHITE);
        }
      }
      uint16_t othervar;
      EEPROM.get(profile + 2, othervar);
      EEPROM.get(profile + 4, numPoints);
      displayTicks(othervar, numPoints, 30, 50);
    } else {
      byte numPoints = pgm_read_word_near(defaultProfile);
      defaultProfileToScreen(0, p1);
      rect(p1[0] - 1, p1[1] - 1, 3, 3, (anim & 32) == 0);

      for (byte i = 1; i < numPoints; i++) {
        defaultProfileToScreen(i - 1, p1);
        defaultProfileToScreen(i, p2);
        rect(p2[0] - 1, p2[1] - 1, 3, 3, (anim & 32) == 0);
        if (includeLines == 1) {
          line(p1[0], p1[1], p2[0], p2[1], WHITE);
        }
      }
      displayTicks(pgm_read_word_near(defaultProfile + 1), pgm_read_word_near(defaultProfile + 2), 30, 50);
    }
  }
}

//void displayProfilePoints(uint16_t profile, byte profileVal, uint16_t* buff, byte tempProfileState) {
//  if (tempProfileState == 1) {
//    byte p1[2];
//    byte p2[2];
//    byte numPoints = buff[0];
//    for (byte i = 0; i < numPoints; i++) {
//      bufferProfileToScreen(buff, i, p1);
//      bufferProfileToScreen(buff, i + 1, p2);
//      rect(p1[0] - 1, p1[1] - 1, 3, 3, (anim & 64) == 0);
//    }
//  } else {
//    if (EEPROM.read(profileVal) == 1) {
//      byte p1[2];
//      byte p2[2];
//      uint16_t numPoints;
//      EEPROM.get(profile, numPoints);
//      for (byte i = 0; i < numPoints; i++) {
//        profileToScreen(profile, i, p1);
//        profileToScreen(profile, i + 1, p2);
//        rect(p1[0] - 1, p1[1] - 1, 3, 3, (anim & 64) == 0);
//      }
//    } else {
//      byte p1[2];
//      byte p2[2];
//      byte numPoints = pgm_read_word_near(defaultProfile);
//      for (byte i = 0; i < numPoints; i++) {
//        defaultProfileToScreen(i, p1);
//        defaultProfileToScreen(i + 1, p2);
//        rect(p1[0] - 1, p1[1] - 1, 3, 3, (anim & 64) == 0);
//      }
//    }
//  }
//}
