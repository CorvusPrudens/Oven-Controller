byte clockState = 0;
byte frameTick = 0;
byte secondTick = 0;
byte minuteTick = 0;

#define JUST_SECOND 1
#define JUST_MINUTE 2

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1325.h>

#include "utility.h"
#include "pt100.h"
#include "audio.h"
#include "gfx.h"
#include "Buttons.h"
//#include "EmuDisplay.h"
#include "Menus.h"



#define OLED_CS 8
#define OLED_RESET 7
#define OLED_DC 6

long delta;

void tick() {
  frameTick++;
  clockState = 0;
  if (frameTick == 30) {
    clockState |= JUST_SECOND;
    frameTick = 0;
    secondTick++;
  }
  if (secondTick == 60) {
    clockState |= JUST_MINUTE;
    secondTick = 0;
    minuteTick++;
  }
}

Adafruit_SSD1325 display(OLED_DC, OLED_RESET, OLED_CS);
volatile byte newFrame = 0;

void setup() {
  pinMode(5, OUTPUT);
  // TXB0108 level shifter enable
  // We'll just leave this high for now
  digitalWrite(5, HIGH);

  // MAX chip select
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);

  // SSR control
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  
  //Serial.begin(115200);
  display.begin();
  display.clearDisplay();
  b = display.getBuffer();
  setFont(tomFont);

  beginMAX();

  setTimer1();

  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  //delta = micros();

  
  
  Serial.begin(115200);
  // Buzzer pin
  pinMode(2, OUTPUT);
//  setTimer2();
//  setTimer0();
//  beginSequence(mario);
  disableAudio();

  //  // DEBUG MODE
  //  for (byte i = 0; i < PROFILES_HEADER; i++){
  //    EEPROM.update(i, 0);
  //  }
}

ISR(TIMER1_COMPA_vect) { //timer1 interrupt @30Hz
  newFrame = 1;
}

byte second = 0;
float reading = 150.0;
byte toggley = 0;
byte skytick = 0;
//long ti = micros();

void loop() {
  if (newFrame == 1) {

    long ti = micros();
    tick();
    display.clearDisplay();
    //    printVal(minuteTick, secondTick, secondTick);
    //    printVal(secondTick, secondTick + 15, secondTick);
    //    delta = micros();
    newFrame = 0;
    anim += 3;
    serialC = 255;
    //    if (Serial.available() > 0) {
    //      serialC = Serial.read();
    //    }
    getButtons(serialC);
    getButtonTimes();

    menuSys();


   // setTextColor(WHITE);
    //printVal(micros() - ti, 110, 22 + skytick);
    //printVal(freeRam(), 1, 29 + skytick);
   // printLiteral("dF:", 110, 15 + skytick);
    
    //printVal(1 + skytick, 100, 19);
    if ((secondTick & 1) > 0 && frameTick == 0) {
      skytick++;
      if (skytick > 20) {
        skytick = 0;
      }
    }

   // ti = micros();
    display.display();
    //ti = micros() - ti;
    //sendData(buttons, buttonTimes);
  }
}
