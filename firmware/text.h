#include "fonts.h"
#include "bitmaps.h"

typedef struct {
  const byte* table PROGMEM;
  byte asciiOffset;
  byte width;
  byte height;
} Font;

// It's much easier to unify everything under the sprite struct
// and it's only one more byte
//typedef struct {
//  const byte* bm PROGMEM;
//  byte width;
//  byte height;
//} Bitmap;

typedef struct {
  const byte* bm PROGMEM;
  byte width;
  byte height;
  byte frames;
} Sprite;

byte* b;
Font currentFont;
byte textSize = 1;
byte textColor = 1;

const byte testString[] PROGMEM = "Testing PROGMEM access...";
byte testString2[] = "Testing against regular SRAM";

const byte tribm[] PROGMEM = {
  0b00000011,
  0b00001101,
  0b00010001,
  0b00001101,
  0b00000011
};

const byte rottribm[] PROGMEM = {
  0b0011111,
  0b0010001,
  0b0001010,
  0b0001010,
  0b0000100,
};

// These should be written to progmem
//Sprite column = {
//  columnbm,
//  39,
//  63,
//  1
//};

Sprite block = {
  blockbm,
  14,
  14,
  1
};

Sprite torch = {
  torches,
  6,
  22,
  3
};

//Sprite crumbling = {
//  crumblingTile,
//  16,
//  16,
//  1
//};

Sprite crumbling = {
  ceiling,
  16,
  16,
  1
};


//Sprite ceilingCrumble = {
//  ceiling,
//  16,
//  16,
//  1
//};

Sprite triBit = {
  tribm,
  5,
  5,
  1
};

Sprite rotTriBit = {
  rottribm,
  5,
  5,
  1
};

//Sprite snow = {
//  snowflake,
//  9,
//  16,
//  1
//};

Sprite downwell = {
  downwellSprites,
  16,
  16,
  14
};

Sprite tree = {
  downwellTree,
  113,
  64,
  1,
};

//Sprite moon = {
//  downwellMoon,
//  30,
//  30,
//  1
//};

Font tomFont = {
  tom,
  30,
  3,
  6
};

//Font adaFont = {
//  ada,
//  0,
//  5,
//  7
//};

void setFont(Font f) {
  currentFont = f;
}

void setTextSize(byte siz) {
  textSize = siz;
}

void setTextColor(byte c) {
  textColor = c;
}

void printChar(byte ch, byte x, byte y) {
  //  x &= 127;
  //  y &= 63;
  //  if (y + currentFont.height > 63) {
  //    y = 63 - currentFont.height;
  //  }
  int16_t index = x + (y / 8) * 128;
  // Speedy way
  // Tells us whether the text will run across one byte of vram or two
  if (textSize == 1) {
    byte ybit = (y & 7);
    byte repeat;
    if (currentFont.height > 7) {
      repeat = ((currentFont.height - 1) / 8) + 1 + (ybit + 7) / 8;
    } else {
      repeat = 1 + (ybit + currentFont.height) / 8;
    }
    if (textColor == 1) {
      if (repeat == 0) {
        // superspeed
        if (index < 1024) {
          for (byte i = 0; i < currentFont.width; i++) {
            if (byte(x + i) < 128) {
              b[index + i] |= pgm_read_byte_near(currentFont.table + (ch - currentFont.asciiOffset) * currentFont.width + i) << ybit;
            }
          }
        }
        return;
      } else {
        if (index < 1024) {
          for (byte i = 0; i < currentFont.width; i++) {
            short charind = (uint16_t)currentFont.table + (ch - currentFont.asciiOffset) * currentFont.width + i;
            if (byte(x + i) < 128) {
              b[index + i] |= pgm_read_byte_near(charind) << ybit;
              if (index + 128 < 1024) {
                b[index + 128 + i] |= pgm_read_byte_near(charind) >> (8 - ybit);
              }
            }
          }
        }
      }
    } else {
      //Black
      if (repeat == 0) {
        // superspeed
        if (index < 1024) {
          for (byte i = 0; i < currentFont.width; i++) {
            if (x + i < 128) {
              b[index + i] &= ~(pgm_read_byte_near(currentFont.table + (ch - currentFont.asciiOffset) * currentFont.width + i) << ybit);
            }
          }
        }
        return;
      } else {
        if (index < 1024) {
          for (byte i = 0; i < currentFont.width; i++) {
            if (byte(x + i) < 128) {
              uint16_t charind = (uint16_t)currentFont.table + (ch - currentFont.asciiOffset) * currentFont.width + i;
              b[index + i] &= ~(pgm_read_byte_near(charind) << ybit);
              if (index + 128 < 1024) {
                b[index + 128 + i] &= ~(pgm_read_byte_near(charind) >> (8 - ybit));
              }
            }
          }
        }
      }
    }
  }
}

void printString(byte* st, byte x, byte y) {
  byte character = 0;
  while (st[character] != 0) {
    printChar(st[character], x + character * (currentFont.width + 1), y);
    character++;
  }
}


void printStringConst(const byte* st, byte x, byte y) {
  byte pos = 0;
  byte character = pgm_read_byte_near(st);
  while (character != 0) {
    printChar(character, x + pos * (currentFont.width + 1), y);
    character = pgm_read_byte_near(++pos + st);
  }
}

void printLiteral(const char* st, byte x, byte y) {
  byte character = 0;
  while (st[character] != 0) {
    printChar(st[character], x + character * (currentFont.width + 1), y);
    character++;
  }
}

// Note -- for unsigned values only
void printVal(uint16_t val, byte x, byte y) {
  if (val > 0) {
    byte tempnum[5];
    uint16_t quotient = val;
    byte place = 0;

    for (byte i = 0; i < 5; i++) {
      tempnum[i] = 48 + (quotient % 10);
      if (quotient < 10) {
        place = i;
        break;
      }
      quotient /= 10;
    }

    byte pos = 0;
    for (byte i = place; i < 255; i--) {
      printChar(tempnum[i], x + pos, y);
      pos += (currentFont.width + 1);
    }
  } else {
    printChar('0', x, y);
  }
}

// Only prints with 2 digits rn
void printFloat(float val, byte x, byte y) {
  byte tempnum[5];
  byte finalnum[7];
  if (val > 0) {
    uint16_t quotient = val * 100;
    byte place = 0;

    for (byte i = 0; i < 5; i++) {
      tempnum[i] = 48 + (quotient % 10);
      if (quotient < 10) {
        place = i;
        break;
      }
      quotient /= 10;
    }

    quotient = val * 100;
    finalnum[5] = tempnum[4];
    finalnum[4] = tempnum[3];
    finalnum[3] = tempnum[2];
    finalnum[2] = '.';
    finalnum[1] = tempnum[1];
    finalnum[0] = tempnum[0];

    if (quotient < 100) {
      finalnum[3] = '0';
      place = 3;
    } else {
      place += 1;
    }

    byte pos = 0;
    for (byte i = place; i < 255; i--) {
      printChar(finalnum[i], x + pos, y);
      pos += (currentFont.width + 1);
    }
  } else if (val < 0) {
    
    uint16_t quotient = abs(val) * 100;
    byte place = 0;

    for (byte i = 0; i < 5; i++) {
      tempnum[i] = 48 + (quotient % 10);
      if (quotient < 10) {
        place = i;
        break;
      }
      quotient /= 10;
    }

    quotient = val * 100;
    finalnum[6] = '-';
    finalnum[5] = tempnum[4];
    finalnum[4] = tempnum[3];
    finalnum[3] = tempnum[2];
    finalnum[2] = '.';
    finalnum[1] = tempnum[1];
    finalnum[0] = tempnum[0];

    if (quotient < 100) {
      finalnum[4] = '-';
      finalnum[3] = '0';
      place = 3;
    } else {
      place += 2;
      finalnum[place] = '-';
    }

    byte pos = 0;
    for (byte i = place; i < 255; i--) {
      printChar(finalnum[i], x + pos, y);
      pos += (currentFont.width + 1);
    }
  } else {
    printChar('0', x, y);
  }
}
