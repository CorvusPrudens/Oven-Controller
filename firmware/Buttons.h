// Interface buttons as wired on the oven board:
#define UP 0b00001000    // pin A3 of PINC
#define DOWN 0b00010000  // pin A4 of PINC
#define LEFT 0b00000100  // pin A2 of PINC
#define RIGHT 0b00100000 // pin A5 of PINC

#define CIRCLE 0b00000001// pin A0 of PINC
#define CROSS 0b00000010 // pin A1 of PINC

// Button states are packed in a byte to minimize RAM footprint
// I could pack two to a byte, but that would make things less clear.
#define JUST_PRESSED 0b0001
#define HELD 0b0010
#define JUST_RELEASED 0b0100

#define ARR_UP     3
#define ARR_DOWN   4
#define ARR_LEFT   2
#define ARR_RIGHT  5

#define ARR_CIRCLE 0
#define ARR_CROSS  1

//String buttonNames[] = {
//  "Circle ",
//  "Cross ",
//  "Left ",
//  "Up ",
//  "Down ",
//  "Right "
//};

// organized so that the index matches the physcial pin
byte buttons[] = {
  0, // circle
  0, // cross
  0, // left
  0, // up
  0, // down
  0  // right
};

// Compact elegant block
void getButtons(byte serialC) {
  for (byte i = 0; i < 6; i++) {
    // If the physical button is held down (normally high):
    if (((1 << i) & PINC) == 0 || ((1 << i) & serialC) == 0) {
      if ((buttons[i] & HELD) == 0) {
        buttons[i] = JUST_PRESSED | HELD;
        // This ensures that the JUST_PRESSED flag is only asserted for one frame:
      } else if ((buttons[i] & JUST_PRESSED) > 0) {
        buttons[i] = HELD;
      }
      // If the physical button is not held down:
    } else {
      if ((buttons[i] & HELD) > 0) {
        buttons[i] = JUST_RELEASED;
        // If HELD is not asserted and the variable is greater than zero, the JUST_RELEASED
        // flag is high
      } else if (buttons[i] > 0) {
        buttons[i] = 0;
      }
    }
  }
}

// How long each button is pressed in frames.
// Important for responsive UI design.
uint16_t buttonTimes[] = {
  0,
  0,
  0,
  0,
  0,
  0
};

void getButtonTimes() {
  for (byte i = 0; i < 6; i++) {
    if ((buttons[i] & HELD) > 0) {
      buttonTimes[i]++;
    } else {
      buttonTimes[i] = 0;
    }
  }
}
