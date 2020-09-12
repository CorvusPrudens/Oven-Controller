#define SSD1325_128_64 //!< Way to specify 128x32 screen
/*=========================================================================*/

#define SSD1325_LCDWIDTH 128 //!< Width w/SSD1325_128_64 defined
#define SSD1325_LCDHEIGHT 64 //!< Height w/SSD1325_128_64 defined

#define SSD1325_SETCOLADDR 0x15  //!< Set column address
#define SSD1325_SETROWADDR 0x75  //!< Set row address
#define SSD1325_SETCONTRAST 0x81 //!< Set contrast current
#define SSD1325_SETCURRENT 0x84  //!< Set current range

#define SSD1325_SETREMAP 0xA0      //!< Set re-map
#define SSD1325_SETSTARTLINE 0xA1  //!< Set display start line
#define SSD1325_SETOFFSET 0xA2     //!< Set display offset
#define SSD1325_NORMALDISPLAY 0xA4 //!< Set display mode normal
#define SSD1325_DISPLAYALLON 0xA5  //!< Set entire display on
#define SSD1325_DISPLAYALLOFF 0xA6 //!< Set entire display off
#define SSD1325_INVERTDISPLAY 0xA7 //!< Invert display
#define SSD1325_SETMULTIPLEX 0xA8  //!< Set multiplex ratio
#define SSD1325_MASTERCONFIG 0xAD  //!< Set master configuration
#define SSD1325_DISPLAYOFF 0xAE    //!< Set display off
#define SSD1325_DISPLAYON 0xAF     //!< Set display on

#define SSD1325_SETPRECHARGECOMPENABLE                                         \
  0xB0                                   //!< Set pre-charge compensation enable
#define SSD1325_SETPHASELEN 0xB1         //!< Set phase length
#define SSD1325_SETROWPERIOD 0xB2        //!< Set row period
#define SSD1325_SETCLOCK 0xB3            //!< Set display clock divide ratio
#define SSD1325_SETPRECHARGECOMP 0xB4    //!< Set pre-charge
#define SSD1325_SETGRAYTABLE 0xB8        //!< Set gray scale table
#define SSD1325_SETPRECHARGEVOLTAGE 0xBC //!< Set precharge voltage
#define SSD1325_SETVCOMLEVEL 0xBE        //!< Set Vcomh Voltage
#define SSD1325_SETVSL 0xBF              //!< Set segment low voltage (VSL)

#define SSD1325_GFXACCEL 0x23 //!< Graphic acceleration command options
#define SSD1325_DRAWRECT 0x24 //!< Draw rectangle
#define SSD1325_COPY 0x25     //!< Copy

SPISettings oledspi = SPISettings(4000000, MSBFIRST, SPI_MODE0);
uint8_t b[SSD1325_LCDWIDTH*SSD1325_LCDHEIGHT/8]; // frame buffer

void command(uint8_t c) {
  // This, on the other hand, is important
  // *cs_port_pointer |= cs_bitmask;
  // *dc_port_pointer &= ~dc_bitmask;
  // delayMicroseconds(1);

  // Screw it, we're hardcoding this
  PORTB |= 0b1;
  PORTD &= 0b10111111;
  delayMicroseconds(1);
  // digitalWrite(cs, HIGH);
  // digitalWrite(dc, LOW);
  // delay(1);

  SPI.beginTransaction(oledspi);

  PORTB &= 0b11111110;
  //digitalWrite(cs, LOW);

  SPI.transfer(c);
  PORTB |= 0b1;
  //digitalWrite(cs, HIGH);

  SPI.endTransaction(); // release the SPI bus
}

void data(uint8_t c) {
  // digitalWrite(cs, HIGH);
  // digitalWrite(dc, HIGH);

  PORTB |= 0b1;
  PORTD |= 0b01000000;

  SPI.beginTransaction(oledspi);

  PORTB &= 0b11111110;
  //digitalWrite(cs, LOW);
  SPI.transfer(c);
  PORTB |= 0b1;
  //digitalWrite(cs, HIGH);

  SPI.endTransaction(); // release the SPI bus
}

void display() {

  command(0x15); /* set column address */
  command(0x00); /* set column start address */
  command(0x3f); /* set column end address */
  command(0x75); /* set row address */
  command(0x00); /* set row start address */
  command(0x3f); /* set row end address */

  SPI.beginTransaction(oledspi);

  PORTB |= 0b1;
  PORTD |= 0b01000000;
  PORTB &= 0b11111110;
  // Necessary? I dunno, probably not.
  //delayMicroseconds(1);
  //
  // digitalWrite(cs, HIGH);
  // digitalWrite(dc, HIGH);
  // digitalWrite(cs, LOW);
  // delay(1);

  // This speeds up writing massively,
  // nearly 70% at the cost of only 8 bytes
  static byte lookup[] = {
    0b1,
    0b10,
    0b100,
    0b1000,
    0b10000,
    0b100000,
    0b1000000,
    0b10000000
  };

  for (uint16_t x = 0; x < 128; x += 2) {
    for (uint16_t y = 0; y < 64; y += 8) { // we write 8 pixels at once
      // Funnily enough, y*16 is a faster method than
      // lookups AND just precalculating with the
      // index (i.e. y < 1024; y += 128)
      // Guess I gotta learn more about compiler optimizations
      uint8_t left8 = b[y * 16 + x];
      uint8_t right8 = b[y * 16 + x + 1];
      for (uint8_t p = 0; p < 8; p++) {
        uint8_t d = 0;
        if (left8 & lookup[p])
          d |= 0xF0;
        if (right8 & lookup[p])
          d |= 0x0F;
        SPI.transfer(d);
      }
    }
  }

  PORTB |= 0b1;
  //digitalWrite(cs, HIGH);
  SPI.endTransaction(); // release the SPI bus
}

// clear everything
void clearDisplay(void) {
  memset(b, 0, (SSD1325_LCDWIDTH*SSD1325_LCDHEIGHT/8));
}

void display_begin(uint8_t dc, uint8_t rst, uint8_t cs) {
  // set pin directions
  SPI.begin();

  pinMode(dc, OUTPUT);
  pinMode(rst, OUTPUT);
  pinMode(cs, OUTPUT);

  //I suck and this didn't work?
  // cs_port_pointer = (uint16_t*)portOutputRegister(cs);
  // dc_port_pointer = (uint16_t*)portOutputRegister(dc);
  // cs_bitmask = digitalPinToBitMask(cs);
  // dc_bitmask = digitalPinToBitMask(dc);

  // None of this really matters in terms of speed for us,
  // so I won't modify it to use port manipulation
  digitalWrite(rst, HIGH);
  // VDD (3.3V) goes high at start, lets just chill for a ms
  delay(1);
  // bring reset low
  digitalWrite(rst, LOW);
  // wait 10ms
  delay(10);
  // bring out of reset
  digitalWrite(rst, HIGH);

  // Serial.println("reset");
  delay(500);
  command(SSD1325_DISPLAYOFF);   /* display off */
  command(SSD1325_SETCLOCK);     /* set osc division */
  command(0xF1);                 /* 145 */
  command(SSD1325_SETMULTIPLEX); /* multiplex ratio */
  command(0x3f);                 /* duty = 1/64 */
  command(SSD1325_SETOFFSET);    /* set display offset --- */
  command(0x4C);                 /* 76 */
  command(SSD1325_SETSTARTLINE); /*set start line */
  command(0x00);                 /* ------ */
  command(SSD1325_MASTERCONFIG); /*Set Master Config DC/DC Converter*/
  command(0x02);
  command(SSD1325_SETREMAP); /* set segment remap------ */
  command(0x56);
  // command(0x52);
  command(SSD1325_SETCURRENT + 0x2); /* Set Full Current Range */
  command(SSD1325_SETGRAYTABLE);
  command(0x01);
  command(0x11);
  command(0x22);
  command(0x32);
  command(0x43);
  command(0x54);
  command(0x65);
  command(0x76);

  command(SSD1325_SETCONTRAST); /* set contrast current */
  command(0x7F);                // max!

  command(SSD1325_SETROWPERIOD);
  command(0x51);
  command(SSD1325_SETPHASELEN);
  command(0x55);
  command(SSD1325_SETPRECHARGECOMP);
  command(0x02);
  command(SSD1325_SETPRECHARGECOMPENABLE);
  command(0x28);
  command(SSD1325_SETVCOMLEVEL); // Set High Voltage Level of COM Pin
  command(0x1C);                 //?
  command(SSD1325_SETVSL);       // set Low Voltage Level of SEG Pin
  command(0x0D | 0x02);

  command(SSD1325_NORMALDISPLAY); /* set display mode */
  /* Clear Screen */
  // command(0x23); /*set graphic acceleration commmand */
  // command(SSD1325_GFXACCEL);
  // command(SSD1325_DRAWRECT); /* draw rectangle */
  // command(0x00); /* Gray Scale Level 1 */
  // command(0x00); /* Gray Scale Level 3 & 2 */
  // command(0x3f); /* Gray Scale Level 3 & 2 */
  // command(0x3f); /* Gray Scale Level 3 & 2 */
  // command(0x00); /* Gray Scale Level 3 & 2 */

  command(SSD1325_DISPLAYON); /* display ON */
}
