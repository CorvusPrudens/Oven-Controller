#include "asongs.h"

#define DETACHE 1
#define LEGATO 0

volatile const byte* currentSequence;
volatile byte currentLen = 0;
volatile byte currentMode = 0;
volatile byte sequenceStep = 0;
volatile byte duration = 1;

byte audioState = 1;

void enableAudio() {
  audioState = 1;
}

void disableAudio() {
  audioState = 0;
}

void setTimer2() {
  cli();
  TCCR2A = 0;
  TCCR2B = 0;
  // Set timer 2 to clear on compare mode
  TCCR2A |= (1 << WGM21);
  // Just starting with a prescaler of 1
  TCCR2B |= (1 << CS20);
  // Setting counter register to 0
  TCNT2 = 0;
  // compare register
  OCR2A = 0;
  sei();
}

void setTimer0() {
  cli();
  TCCR0A = 0;
  TCCR0B = 0;
  // Set timer 0 to clear on compare mode
  TCCR0A |= (1 << WGM01);
  // This timer will always run with a prescaler of 1024
  TCCR0B |= (1 << CS02) | (1 << CS00);
  // Setting counter register to 0
  TCNT0 = 0;
  // compare register
  OCR0A = 0;
  sei();
}

// ADD PRIORITY -- LET MUSIC PLAY RATHER THAN SFX
// This is off now due to the higher clock, but it doesn't really matter
void beginSequence(const byte* seq) {
  if (audioState == 1) {
    static const byte ocr[] PROGMEM = {
      255, 208, 162, 132, 112, 97, 85, 76
    };

    currentSequence = (volatile const byte*)seq;
    currentLen = pgm_read_byte_near(seq + 2);
    currentMode = pgm_read_byte_near(seq);
    sequenceStep = 0;
    duration = 1;

    cli();
    OCR0A = pgm_read_byte_near(ocr + pgm_read_byte_near(seq + 1));
    TIMSK0 |= (1 << OCIE0A);
    sei();
  }
}

void noteon(byte notein) {
  static const byte prescaler[] PROGMEM = {
    6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
  };
  static const byte ocr[] PROGMEM = {
    148, 140, 132, 250, 236, 223, 210, 198, 187, 177, 167, 157,
    148, 140, 132, 250, 236, 223, 210, 198, 187, 177, 167, 157,
    148, 140, 132, 250, 236, 223, 210, 198, 187, 177, 167, 157,
    148, 140, 132, 125, 118, 111, 105, 99, 93, 88, 83, 78
  };

  cli();
  TCCR2B &= 0b11111000;
  TCCR2B |= pgm_read_byte_near(prescaler + notein);
  OCR2A = pgm_read_byte_near(ocr + notein);
  TIMSK2 |= (1 << OCIE2A);
  sei();

}

void noteoff() {
  cli();
  TIMSK2 &= ~(1 << OCIE2A);
  PORTD &= 0b11111011;
  sei();
}

void sequenceInterrupt() {
  duration--;
  if (duration == 0) {
    if (sequenceStep == currentLen) {
      // turn of this interrupt here!
      noteoff();
      sequenceStep = 0;
      duration = 1;
      TIMSK0 &= ~(1 << OCIE0A);
      return;
    }
    duration = pgm_read_byte_near(currentSequence + sequenceStep * 2 + 4) * 8;
    byte note = pgm_read_byte_near(currentSequence + sequenceStep * 2 + 3);
    if (note != 255) {
      noteon(note);
    } else {
      noteoff();
    }
    sequenceStep++;
  } else if (duration == 6) {
    if (currentMode == DETACHE) {
      noteoff();
    }
  }
}

ISR(TIMER2_COMPA_vect) {
  // Buzzer pin is D2
  PORTD ^= 0b100;
}

ISR(TIMER0_COMPA_vect) {
  sequenceInterrupt();
}
