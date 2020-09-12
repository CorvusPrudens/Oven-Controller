#define UTILITY_HEADER 1

void toBin(byte* arr, byte input) {
  for (byte i = 0; i < 8; i++) {
    arr[7 - i] = ((input >> i) & 1) + '0';
  }
}

void toDec(byte* arr, byte input) {
  if (input == 0) {
    arr[0] = '0';
    arr[1] = '0';
    arr[2] = '0';
  }
  arr[0] = '0' + input / 100;
  arr[1] = '0' + (input % 100) / 10;
  arr[2] = '0' + (input % 100) % 10;
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setTimer1() {

  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 20hz increments
  OCR1A = 650;// = (20*10^6) / (30Hz*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bit for 1024 prescaler
  TCCR1B |= (1 << CS10) | (1 << CS12);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
}

byte getLengthDyn(byte* arr) {
  byte len = 0;
  while (arr[len++] != 0 && len < 255);
  return len - 1;
}

byte getLengthConst(const byte* arr) {
  byte len = 0;
  while (pgm_read_byte_near(arr + len++) != 0 && len < 255);
  return len - 1;
}

void retrieveStringConst(const byte* progstring, byte* buffstring, byte buffsize) {
  byte i = 0;
  while (pgm_read_byte_near(progstring + i) != 0 && i < buffsize - 1) {
    buffstring[i] = pgm_read_byte_near(progstring + i);
    i++;
  }
  buffstring[i] = 0;
}

void retrieveStringDyn(byte* progstring, byte* buffstring, byte buffsize) {
  byte i = 0;
  while (progstring[i] != 0 && i < buffsize - 1) {
    buffstring[i] = progstring[i];
    i++;
  }
  buffstring[i] = 0;
}

float emuReadMAXtemp() {
  static float temp = 24.0;
  if (Serial.available() > 0) {
    temp = Serial.parseFloat();
    while (Serial.read() >= 0);
  }

  return temp;
}

void sendTemp(float val) {
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
    Serial.write('<');
    for (byte i = place; i < 255; i--) {
      Serial.write(finalnum[i]);
    }
    Serial.write('>');
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
    Serial.write('<');
    for (byte i = place; i < 255; i--) {
      Serial.write(finalnum[i]);
    }
    Serial.write('>');
  } else {
    // efficient? maybe not, consider revamping
    Serial.write('<');
    Serial.write('0');
    Serial.write('.');
    Serial.write('0');
    Serial.write('0');
    Serial.write('>');
  }
}

//// 91 x 2 bytes ==> 182 bytes
//const unsigned int isinTable16[] PROGMEM = {
//  0, 1144, 2287, 3430, 4571, 5712, 6850, 7987, 9121, 10252, 11380,
//  12505, 13625, 14742, 15854, 16962, 18064, 19161, 20251, 21336, 22414,
//  23486, 24550, 25607, 26655, 27696, 28729, 29752, 30767, 31772, 32768,
//
//  33753, 34728, 35693, 36647, 37589, 38521, 39440, 40347, 41243, 42125,
//  42995, 43851, 44695, 45524, 46340, 47142, 47929, 48702, 49460, 50203,
//  50930, 51642, 52339, 53019, 53683, 54331, 54962, 55577, 56174, 56755,
//
//  57318, 57864, 58392, 58902, 59395, 59869, 60325, 60763, 61182, 61583,
//  61965, 62327, 62671, 62996, 63302, 63588, 63855, 64103, 64331, 64539,
//  64728, 64897, 65047, 65176, 65286, 65375, 65445, 65495, 65525, 65535,
//};
//
//// 91 bytes
//const uint8_t isinTable8[] PROGMEM = {
//  0, 4, 9, 13, 18, 22, 27, 31, 35, 40, 44,
//  49, 53, 57, 62, 66, 70, 75, 79, 83, 87,
//  91, 96, 100, 104, 108, 112, 116, 120, 124, 128,
//
//  131, 135, 139, 143, 146, 150, 153, 157, 160, 164,
//  167, 171, 174, 177, 180, 183, 186, 190, 192, 195,
//  198, 201, 204, 206, 209, 211, 214, 216, 219, 221,
//
//  223, 225, 227, 229, 231, 233, 235, 236, 238, 240,
//  241, 243, 244, 245, 246, 247, 248, 249, 250, 251,
//  252, 253, 253, 254, 254, 254, 255, 255, 255, 255,
//};
//
//
//float isin(long x)
//{
//  boolean pos = true;  // positive - keeps an eye on the sign.
//  if (x < 0) {
//    x = -x;
//    pos = !pos;
//  }
//  if (x >= 360) x %= 360;
//  if (x > 180) {
//    x -= 180;
//    pos = !pos;
//  }
//  if (x > 90) x = 180 - x;
//  //  if (pos) return isinTable8[x] * 0.003921568627; // = /255.0
//  //  return pgm_read_word_near(isinTable8 + x) * -0.003921568627 ;
//  if (pos) return pgm_read_word_near(isinTable16 + x) * 0.0000152590219; // = /65535.0
//  return pgm_read_word_near(isinTable16 + x) * -0.0000152590219 ;
//}
//
//float icos(long x) {
//  return isin(x + 90);
//}
//
//float itan(long x) {
//  return isin(x) / icos(x);
//}
//
//float fsin(float d) {
//  float a = isin(d);
//  float b = isin(d + 1);
//  return a + (d - int(d)) * (b - a);
//}
