#include "text.h"

#define BUFFER_WIDTH 128
#define BUFFER_HEIGHT 64/8

#define FLIPX 0b1
#define FLIPY 0b10

#define WHITE 1
#define BLACK 0

void pixel(byte x, byte y, byte col) {
  static byte white[] = {
    0b1,
    0b10,
    0b100,
    0b1000,
    0b10000,
    0b100000,
    0b1000000,
    0b10000000
  };
  static byte black[] = {
    0b11111110,
    0b11111101,
    0b11111011,
    0b11110111,
    0b11101111,
    0b11011111,
    0b10111111,
    0b01111111
  };
  uint16_t index = x + (y / 8) * BUFFER_WIDTH;
  if (index < 1024 && x < 128) {
    if (col == WHITE) {
      b[index] |= white[y & 7];
    } else {
      b[index] &= black[y & 7];
    }
  }
}

void fastVertLine(byte x, byte y, byte height, byte col) {
  const static byte white[] = {
    0b0, // Nothing will have a height of zero, but this will fix the off by one issue
    0b1,
    0b11,
    0b111,
    0b1111,
    0b11111,
    0b111111,
    0b1111111,
    0b11111111
  };

  uint16_t index = x + (y / 8) * BUFFER_WIDTH;
  if (y >= 64) {
    index = x + ((y / 8) + 480) * BUFFER_WIDTH;
  }

  //printVal(index, 2, 5);
  byte ybit = y & 7;
  byte numBytes = ((height) / 8) + 1 + (ybit + (height & 7)) / 8;

  //printVal(numBytes, 100, 2);

  if (x < 128) {
    if (index < 1024) { // y wrapping doesnt work quite like i'd want, look into further
      if (numBytes == 1) {
        // If the line is uint16_t and only lives on one byte
        if (col == WHITE) {
          b[index] |= white[height] << ybit;
        } else {
          b[index] &= ~(white[height] << ybit);
        }
        return;
      }
      // First byte from initial y
      if (col == WHITE) {
        b[index] |= 255 << ybit;
      } else {
        b[index] &= 255 >> (8 - ybit);
      }
    }

    // Y will now be a factor of 8 for rest of sequence and is
    // effectively incremented by adding the buffer width to the index
    height -= 8 - ybit;
    index += BUFFER_WIDTH;
    for (byte i = 1; i < numBytes; i++) {
      if (height <= 8) {
        break;
      }
      if (index < 1024) {
        if (col == WHITE) {
          b[index] = 255;
        } else {
          b[index] = 0;
        }
      }
      index += BUFFER_WIDTH;
      height -= 8;
    }
    // Final byte if the y position doesn't line up
    //printVal(height, 100, 9);
    if (index < 1024) {
      if (col == WHITE) {
        b[index] |= white[height];
      } else {
        b[index] &= ~(white[height]);
      }
    }
  }
}


void swapByte(byte* abyte, byte* bbyte) {
  byte temp = *abyte;
  *abyte = *bbyte;
  *bbyte = temp;
}

void line(byte x1, byte y1, byte x2, byte y2, byte col) {
  // This doesn't overflow on the y axis nicely like the others

  byte topy;
  byte boty;
  if (y2 < y1) {
    topy = y2;
    boty = y1;
  } else {
    topy = y1;
    boty = y2;
  }
  byte leftx;
  byte rightx;
  if (x2 < x1) {
    leftx = x2;
    rightx = x1;
  } else {
    leftx = x1;
    rightx = x2;
  }

  if (x1 == x2) {
    fastVertLine(x1, topy, boty - topy, col);
  } else if (y1 == y2) {
    for (byte i = 0; i < rightx - leftx; i++) {
      pixel(leftx + i, y1, col);
    }
  } else {
    //real line code
    byte steep = boty - topy > rightx - leftx;

    if (steep) {
      swapByte(&x1, &y1);
      swapByte(&x2, &y2);
    }

    if (x1 > x2) {
      swapByte(&x1, &x2);
      swapByte(&y1, &y2);
    }

    byte dx, dy;
    dx = x2 - x1;

    byte ystep;
    if (y1 < y2) {
      ystep = 1;
      dy = y2 - y1;
    } else {
      ystep = -1;
      dy = y1 - y2;
    }

    byte err = dx / 2;

    for (; x1 <= x2; x1++) {
      if (steep) {
        pixel(y1, x1, col);
      } else {
        pixel(x1, y1, col);
      }
      err -= dy;
      // bit of a hack this one
      if (err > 200) {
        y1 += ystep;
        err += dx;
      }
    }
  }
}

void rect(byte x, byte y, byte width, byte height, byte col) {
  for (int i = 0; i < width; i++) {
    fastVertLine(x + i, y, height, col);
  }
}

void rectNoFill(byte x, byte y, byte width, byte height, byte col) {
  fastVertLine(x, y, height, col);
  if (width > 1) {
    fastVertLine(x + width - 1, y, height, col);
    for (int i = x + 1; i < x + width - 1; i++) {
      pixel(i, y, col);
      pixel(i, y + height - 1, col);
    }
  }
}

void rectBorder(byte x, byte y, byte width, byte height, byte col) {
  rectNoFill(x, y, width, height, col & 1);
  rect(x + 1, y + 1, width - 2, height - 2, ~(col) & 1);
}

// This could probably be optimized by restricting the number of arguments
// and creating specific functions (ie a spriteBlack and spriteWhite function)

void sprite(Sprite sp, byte x, byte y, byte col, byte flipState, byte frame) {

  byte height = sp.height;
  if (frame > sp.frames - 1) {
    frame = sp.frames - 1;
  }

  uint16_t spAddress = (uint16_t)sp.bm;
  if (frame > 0) {
    spAddress += (1 + (sp.height - 1) / 8) * sp.width * frame;
  }

  byte ybit = y & 7;
  byte numBytes;
  byte widthFlip = sp.width - 1;

  if (height > 7) {
    numBytes = ((height - 1) / 8) + 1 + (ybit + 7) / 8;
  } else {
    numBytes = 1 + (ybit + height) / 8;
  }


  uint16_t index = x + (y / 8) * BUFFER_WIDTH;
  if (y >= 64) {
    index = x + ((y / 8) + 480) * BUFFER_WIDTH;
  }
  if (x > 127) {
    index -= BUFFER_WIDTH * 2;
  }
  //  // debug hack
  //  if (height == 64){
  //    printVal((index - x)/BUFFER_WIDTH, 110, 32);
  //    printVal(numBytes, 110, 32);
  //  }

  //if (index < 1024) {
  if (numBytes == 1) {
    for (byte tempx = 0; tempx < sp.width; tempx++) {
      byte xindex = tempx;
      if ((flipState & FLIPX) > 0) {
        xindex = widthFlip - tempx;
      }
      if (byte(x + xindex) < 128 && index + xindex < 1024) {
        if (col == WHITE) {
          b[index + xindex] |= pgm_read_byte_near(spAddress + tempx) << ybit;
        } else {
          b[index + xindex] &= ~(pgm_read_byte_near(spAddress + tempx) << ybit);
        }
      }
    }
    return;
  } else if (height < 8) {
    for (byte tempx = 0; tempx < sp.width; tempx++) {
      byte xindex = tempx;
      if ((flipState & FLIPX) > 0) {
        xindex = widthFlip - tempx;
      }
      uint16_t img = pgm_read_byte_near(spAddress + tempx) << ybit;
      if (byte(x + xindex) < 128 && index + xindex < 1024) {
        if (col == WHITE) {
          b[index + xindex] |= img;
          if (index + BUFFER_WIDTH < 1024) {
            img >>= 8;
            b[index + xindex + BUFFER_WIDTH] |= img;
          }
        } else {
          b[index + tempx] &= ~img;
          if (index + BUFFER_WIDTH < 1024) {
            img >>= 8;
            b[index + xindex + BUFFER_WIDTH] &= ~img;
          }
        }
      }
    }
    return;
  }

  // First byte from initial y
  for (byte tempx = 0; tempx < sp.width; tempx++) {
    byte xindex = tempx;
    if ((flipState & FLIPX) > 0) {
      xindex = widthFlip - tempx;
    }
    if (byte(x + xindex) < 128 && index + xindex < 1024) {
      if (col == WHITE) {
        b[index + xindex] |= pgm_read_byte_near(spAddress + tempx) << ybit;
      } else {
        b[index + xindex] &= ~(pgm_read_byte_near(spAddress + tempx) << ybit);
      }
    }
  }
  //}

  // Y will now be a factor of 8 for rest of sequence and is
  // effectively incremented by adding the buffer witdth to the index
  byte bmya = 8 - ybit; //this actually wonn't change!
  height -= bmya;
  index += BUFFER_WIDTH;
  spAddress += sp.width;

  for (byte i = 1; i < numBytes; i++) {
    //if (index < 1024) { // unfortunately this doesn't work in all cases, hopefully checking every byte isn't too slw
    for (byte tempx = 0; tempx < sp.width; tempx++) {
      byte xindex = tempx;
      if ((flipState & FLIPX) > 0) {
        xindex = widthFlip - tempx;
      }
      if (byte(x + xindex) < 128 && index + xindex < 1024) {

        // For any position, there will either be 1 bitmap byte per screen byte (y coordinate is factor of 8)
        // or there will be 2 bitmap bytes. It ought to be faster to if-else it...
        if ((bmya & 7) == 0) {
          // Very easy! Since we know we've already written one byte, the numBytes val (i) tells us
          //exactly what our ypos is
          if (col == WHITE) {
            b[index + xindex] |= pgm_read_byte_near(spAddress + tempx);
          } else {
            b[index + xindex] &= ~pgm_read_byte_near(spAddress + tempx);
          }
        } else {
          byte upper = pgm_read_byte_near(spAddress + tempx - sp.width) >> bmya;
          byte lower;
          if (col == WHITE) {
            if (i == numBytes - 1) {
              b[index + xindex] |= upper;
              if (tempx == sp.width - 1) {
                break;
              }
            } else {
              lower = pgm_read_byte_near(spAddress + tempx) << (8 - bmya);
              b[index + xindex] |= upper | lower;
            }
          } else {
            if (i == numBytes - 1) {
              b[index + xindex] &= ~upper;
              if (tempx == sp.width - 1) {
                break;
              }
            } else {
              lower = pgm_read_byte_near(spAddress + tempx) << (8 - bmya);
              b[index + xindex] &= ~(upper | lower);
            }
          }
        }
      }
    }
    //}
    index += BUFFER_WIDTH;
    height -= 8;
    spAddress += sp.width;
  }
}

// Much simpler and more elegant code, but also a bit
// slower, and sometimes much slower
//void pixelBitmap(Sprite sp, byte x, byte y, byte col) {
//  for (byte i = 0; i < sp.width; i++) {
//    for (int j = 0; j < 1 + (sp.height - 1) / 8; j++) {
//      byte line = pgm_read_byte_near(sp.bm + i + j * sp.width);
//      byte xpos = x + i;
//      for (int k = 0; k < 8; k++) {
//        if ((line & 1) == 1) {
//          pixel(xpos, y + k + j * 8, 1);
//        }
//        if (k + j * 8 == sp.height) {
//          break;
//        }
//        line /= 2;
//      }
//    }
//  }
//}

// Much like a sprite, but restricted on the y axis to the byte grid (8 possible y positions)
// Also only white. Just make a different bitmap if you wanna invert it.
void tile(Sprite sp, byte x, byte y, byte flipState, byte frame) {
  y /= 8;
  byte numBytes = 1 + (sp.height - 1) / 8;
  if (frame >= sp.frames) {
    frame = sp.frames - 1;
  }
  byte widthFlip = sp.width - 1;
  uint16_t spAddress = (uint16_t)sp.bm;
  if (frame > 0) {
    spAddress += numBytes * sp.width * frame;
  }
  uint16_t index = x + y * BUFFER_WIDTH;

  if (x > 127) {
    index -= BUFFER_WIDTH * 2;
  }

  if (index < 1024) {
    for (byte i = 0; i < numBytes; i++) {
      for (byte tempx = 0; tempx < sp.width; tempx++) {
        byte xindex = tempx;
        if ((flipState & FLIPX) > 0) {
          xindex = widthFlip - tempx;
        }
        if (byte(x + xindex) < 128) {
          b[index + xindex] = pgm_read_byte_near(spAddress + tempx);
        }
      }
      spAddress += sp.width;
      index += BUFFER_WIDTH;
    }
  }
}
