#include "MenuSys.h"

#ifndef UTILITY_HEADER
#include "utility.h"
#endif


#define DIALOGUE_SAVE    22
#define DIALOGUE_DISCARD 24
#define DIALOGUE_DELETE  26
#define DIALOGUE_REFLOW  7

#define SETTINGS_CALIBRATE 0b1
#define SETTINGS_VIEW 0b10



byte serialC = 255;
// The dunamic
const byte* dialogueMessage;
byte dynamicDialogueMessage[4];
const byte* yesMessage;
const byte* noMessage;
byte dialogueResponse = 0;
byte yesResponse;
byte saveStep = 0;
uint16_t reflowTime = 0;

//debug
uint16_t serialTemp = 24;

// This will store the actual index of the current profile
uint16_t currentProfile = PROFILES_HEADER;
// This will store the number of the current profile
byte currentProfileVal = 0;

// CONST STRINGS
// This is 30 bytes in SRAM. If things get tough,
// start putting the pointers in PROGMEM too
const byte string_reflow[] PROGMEM = "Reflow";
const byte string_profiles[] PROGMEM = "Profiles";
const byte string_settings[] PROGMEM = "Settings";
const byte string_credits[] PROGMEM = "Credits";
const byte string_selectprofile[] PROGMEM = "Select profile...";
const byte string_saveas[] PROGMEM = "Save as profile...";
const byte string_add[] PROGMEM = "Add";
const byte string_del[] PROGMEM = "Del";
const byte string_adj[] PROGMEM = "Adj";
const byte string_save[] PROGMEM = "Save";
const byte string_deletepoint[] PROGMEM = "Delete this point?";
const byte string_delete[] PROGMEM = "Delete";
const byte string_cancel[] PROGMEM = "Cancel";
const byte string_discard[] PROGMEM = "Discard unsaved changes?";
const byte string_yes[] PROGMEM = "Yes";
const byte string_cancelreflow[] PROGMEM = "Cancel reflow process?";
const byte string_beginreflow[] PROGMEM = "Begin reflow";
const byte string_chooseprofile[] PROGMEM = "Choose Profile";
const byte string_exit[] PROGMEM = "Exit";
const byte string_overwriteprofile[] PROGMEM = "Overwrite profile";
const byte string_beginwithprof[] PROGMEM = "Begin reflow with profile";
const byte string_saving[] PROGMEM = "Saving...";
const byte string_calibrate[] PROGMEM = "Calibrate";
const byte string_viewresults[] PROGMEM = "View results";
const byte string_xscl[] PROGMEM = "xscl";
const byte string_yscl[] PROGMEM = "yscl";
const byte string_adjx[] PROGMEM = "Adjust time axis";
const byte string_adjy[] PROGMEM = "Adjust temperature axis";
const byte string_temp[] PROGMEM = "Temp";
const byte string_time[] PROGMEM = "Time";

void popupDyn(const byte* message, byte* dynamicMessage, const byte* yesbutt, const byte* nobutt, byte yesresp) {
  // MUST BE NULL TERMINATED
  retrieveStringDyn(dynamicMessage, dynamicDialogueMessage, 4);
  dialogueMessage = message;
  yesMessage = yesbutt;
  noMessage = nobutt;
  //  retrieveStringDyn(yesbutt, yesMessage, 16);
  //  retrieveStringDyn(nobutt, noMessage, 16);
  prevMenu = currentMenu;
  currentMenu = DIALOGUE_MENU;
  yesResponse = yesresp;
  buttonxy = 0;
}

void popupConst(const byte* message, const byte * yesbutt, const byte * nobutt, byte yesresp) {
  // MUST BE NULL TERMINATED
  //retrieveStringConst(message, dialogueMessage, 32);
  //  retrieveStringConst(yesbutt, yesMessage, 16);
  //  retrieveStringConst(nobutt, noMessage, 16);
  dialogueMessage = message;
  yesMessage = yesbutt;
  noMessage = nobutt;

  prevMenu = currentMenu;
  currentMenu = DIALOGUE_MENU;
  yesResponse = yesresp;
  buttonxy = 0;
}

void transientPopup(byte* message, byte duration) {
  // timed popup that doesn't take input
}

void mainMenu() {
  byte tempx = buttonxy >> 4;
  byte tempy = buttonxy & 15;
  updatePos(&tempx, &tempy, 0, 2, WRAP | INVERSE_Y);
  buttonxy = (tempx << 4) | tempy;

  //writeMAXbyte(0, 0b00010000);
  printVal(uint16_t(round(readMAXtemp())), 2, 2);
  //Serial.println(readMAXtemp());

  //  sprite(column, 10, 0, WHITE, 0, 0);
  sprite(torch, 32 - torch.width, 22, WHITE, 0, frameTick / 2 % torch.frames);
  //  sprite(column, 79, 0, WHITE, 0, 0);
  sprite(torch, 95, 22, WHITE, FLIPX, frameTick / 2 % torch.frames);
  for (byte i = 0; i < 10; i++) {
    //tile(ceilingCrumble, i* 16, 0, 0, 0);
    tile(block, i * 14, 64 - 14, 0, 0);
  }

  uiButtonConst(64 - BUTTON_W_XL / 2, 2, BUTTON_W_XL, BUTTON_H_M, string_reflow, anim, 0, buttonxy);
  uiButtonConst(64 - BUTTON_W_XL / 2, 18, BUTTON_W_XL, BUTTON_H_M, string_profiles, anim, 1, buttonxy);
  uiButtonConst(64 - BUTTON_W_XL / 2, 34, BUTTON_W_XL, BUTTON_H_M, string_settings, anim, 2, buttonxy);
  // uiButtonConst(64 - BUTTON_W_XL / 2, 50, BUTTON_W_XL, BUTTON_H_M, string_credits, anim, 3, buttonxy);


  if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
    if (buttonxy == 1) {
      beginSequence(forward);
      buttonxy = 0;
      currentMenu = PROFILES_MENU;
      //graphicsClear(BLACK);
    } else if (buttonxy == 0) {
      beginSequence(forward);
      buttonxy = 0;
      currentMenu = REFLOW_MENU;
    } else if (buttonxy == 2) {
      beginSequence(forward);
      buttonxy = 0;
      currentMenu = SETTINGS_MENU;
    }
  }
}

void settingsMenu() {
  //  static byte settingsState = 0;
  //  static byte settingsCalibrate = 0;
  //
  //  if (settingsState == 0) {
  //    byte tempx = buttonxy >> 4;
  //    byte tempy = buttonxy & 15;
  //    updatePos(&tempx, &tempy, 0, 1, WRAP | INVERSE_Y);
  //    buttonxy = (tempx << 4) | tempy;
  //
  //    uiButtonConst(48, 20, BUTTON_W_XL, BUTTON_H_L, string_calibrate, anim, 0, buttonxy);
  //    uiButtonConst(48, 20 + BUTTON_H_L + 2, BUTTON_W_XL, BUTTON_H_L, string_viewresults, anim, 1, buttonxy);
  //
  //    if ((buttons[ARR_CIRCLE] & JUST_PRESSED) > 0) {
  //      if (buttonxy == 0) {
  //        settingsState |= SETTINGS_CALIBRATE;
  //        //Serial.write(27);
  //      } else if (buttonxy == 1) {
  //        settingsState |= SETTINGS_VIEW;
  //      }
  //    }
  if ((buttons[ARR_CROSS] & JUST_PRESSED) > 0) {
    currentMenu = MAIN_MENU;
  }
  //  } else if ((settingsState & SETTINGS_CALIBRATE) > 0) {
  //    uint16_t currentTemp = (uint16_t)round(readMAXtemp());
  //
  //    printVal(currentTemp, 2, 2);
  //    printVal(settingsCalibrate, 2, 9);
  //
  //    if (frameTick == 0) {
  //      if ((reflowTime & 1) == 0) {
  //        ttbuffer[reflowTime / 2] = currentTemp;
  //        //ttbuffer[reflowTime / 2] = (uint16_t)serialTemp;
  //      }
  //      if (reflowTime < 256) {
  //        reflowTime++;
  //      }
  //    }
  //
  //    if (reflowTime / 2 > 1) {
  //      for (uint16_t i = 1; i < reflowTime / 2; i++) {
  //        line(i - 1, 64 - ttbuffer[i - 1] / 4, i, 64 - ttbuffer[i] / 4, WHITE);
  //      }
  //    }
  //
  //    //    if (Serial.available() > 0) {
  //    //      serialTemp = Serial.read();
  //    //    }
  //
  //    if (settingsCalibrate == 0) {
  //      if (maxTesting(&reflowTime, ttbuffer) == 1) {
  //        settingsCalibrate = 1;
  //        reflowTime = 0;
  //      }
  //    } else if (settingsCalibrate == 1) {
  //      if (currentTemp <= 40) {
  //        settingsCalibrate = 2;
  //        reflowTime = 0;
  //      }
  //    } else if (settingsCalibrate == 2) {
  //      if (minTesting(&reflowTime, ttbuffer) == 1) {
  //        settingsCalibrate = 3;
  //      }
  //    } else if (settingsCalibrate == 3) {
  //      saveCalibration();
  //      reflowTime = 0;
  //      settingsCalibrate = 0;
  //      settingsState &= ~SETTINGS_CALIBRATE;
  //      elementsOff();
  //    }
  //
  //    //printVal((uint16_t)liveTemp, 2, 2);
  //
  //    if ((buttons[ARR_CROSS] & JUST_PRESSED) > 0) {
  //      settingsState &= ~SETTINGS_CALIBRATE;
  //      elementsOff();
  //    }
  //  } else if ((settingsState & SETTINGS_VIEW) > 0) {
  //    float g = 0;
  //    for (byte i = 0; i < 5; i++) {
  //      EEPROM.get(CALIBRATION_OFFSET + i * 4, g);
  //      printVal(g * 100, 57, 12 + 7 * i);
  //      EEPROM.get(CALIBRATION_OFFSET + 20 + i * 4, g);
  //      printVal(g * 100, 71, 12 + 7 * i);
  //    }
  //    uint16_t ge = 0;
  //    EEPROM.get(CALIBRATION_OFFSET + 40, ge);
  //    printVal(ge, 57, 58);
  //    EEPROM.get(CALIBRATION_OFFSET + 42, ge);
  //    printVal(ge, 71, 58);
  //
  //    if ((buttons[ARR_CROSS] & JUST_PRESSED) > 0) {
  //      settingsState &= ~SETTINGS_VIEW;
  //    }
  //  }
}

void profilesMenu() {
  static byte temptemp;
  static byte temptime;
  static byte profileState0 = CHOOSE;
  static byte profileState1 = 0;
  static byte currentPoint = 0;
  static uint16_t prevProfile = currentProfile;
  static byte prevProfileVal = currentProfileVal;

  displayProfile(currentProfile, currentProfileVal, ttbuffer, (profileState1 & DISPLAY_BUFFER) > 0, 1);

  if (profileState0 == 0) {
    //    profileState1 |= UNLOADED;
    if ((profileState1 & DISPLAY_BUFFER) == 0) {
      loadTempProfile(currentProfile, currentProfileVal, ttbuffer);
      profileState1 |= DISPLAY_BUFFER;
    }

    byte tempx = buttonxy >> 4;
    byte tempy = buttonxy & 15;
    updatePos(&tempx, &tempy, 5, 0, WRAP | INVERSE_Y);
    buttonxy = (tempx << 4) | tempy;

    printLiteral("P", 30, 52);
    printVal(currentProfileVal + 1, 36, 52);
    uiButtonConst(5, 50, BUTTON_W_S, BUTTON_H_S, string_add, anim, 0, buttonxy);
    uiButtonConst(5 + (BUTTON_W_S + 1), 50, BUTTON_W_S, BUTTON_H_S, string_del, anim, 1 << 4, buttonxy);
    uiButtonConst(5 + (BUTTON_W_S + 1) * 2, 50, BUTTON_W_S, BUTTON_H_S, string_adj, anim, 2 << 4, buttonxy);
    uiButtonConst(5 + (BUTTON_W_S + 1) * 3, 50, BUTTON_W_S, BUTTON_H_S, string_save, anim, 3 << 4, buttonxy);

    uiButtonConst(5 + (BUTTON_W_S + 1) * 4, 50, BUTTON_W_S, BUTTON_H_S, string_xscl, anim, 4 << 4, buttonxy);
    uiButtonConst(5 + (BUTTON_W_S + 1) * 5, 50, BUTTON_W_S, BUTTON_H_S, string_yscl, anim, 5 << 4, buttonxy);

    if (dialogueResponse == DIALOGUE_DISCARD) {
      dialogueResponse = 0;
      profileState0 |= CHOOSE;
      profileState1 &= ~DISPLAY_BUFFER;
    }

    if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
      //graphicsClear(BLACK);
      //buttonxy = 1; // BUTTONXY SHOULD BE SAVED PER MENU! (?)
      if ((profileState1 & CHANGED) > 0) {
        popupConst(string_discard, string_yes, string_cancel, DIALOGUE_DISCARD);
      } else {
        profileState0 |= CHOOSE;
        profileState1 &= ~DISPLAY_BUFFER;
      }
    } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
      if (buttonxy == 0) {
        profileState0 |= ADD;
      } else if (buttonxy == (1 << 4)) {
        profileState0 |= DEL;
      } else if (buttonxy == (2 << 4)) {
        profileState0 |= ADJUST;
      } else if (buttonxy == (3 << 4)) {
        profileState0 |= SAVE;
        prevProfile = currentProfile;
        prevProfileVal = currentProfileVal;
      } else if (buttonxy == (4 << 4)) {
        profileState0 |= ADJ_AXIS;
        profileState1 |= ADJ_X_AXIS;
        for (byte i = 0; i < PROFILE_LENGTH; i++) {
          ttbuffer[i + PROFILE_LENGTH] = ttbuffer[i];
        }
      } else if (buttonxy == (5 << 4)) {
        profileState0 |= ADJ_AXIS;
        profileState1 |= ADJ_Y_AXIS;
        for (byte i = 0; i < PROFILE_LENGTH; i++) {
          ttbuffer[i + PROFILE_LENGTH] = ttbuffer[i];
        }
      }
    }
  } else if ((profileState0 & ADJUST) > 0) {
    if ((profileState0 & SELECT) == 0) {
      // This shite better be loaded by now
      displayStats(ttbuffer[3 + currentPoint * 2], ttbuffer[3 + currentPoint * 2 + 1]);

      updatePos(&currentPoint, &currentPoint, ttbuffer[0] - 1, ttbuffer[0] - 1, WRAP);
      // TODO: make this sprite more clearly indicating selection
      byte p1[2];
      bufferProfileToScreen(ttbuffer, currentPoint, p1);
      sprite(triBit, p1[0]  - triBit.width / 2 - 1, p1[1] - triBit.height - 1, WHITE, 0, 0);

      if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
        profileState0 &= ~ADJUST;
      } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
        profileState0 |= SELECT;
        for (byte i = 0; i < PROFILE_LENGTH; i++) {
          ttbuffer[i + PROFILE_LENGTH] = ttbuffer[i];
        }
      }
    } else {
      // POINT SELECTED

      displayStats(ttbuffer[3 + currentPoint * 2], ttbuffer[3 + currentPoint * 2 + 1]);
      // TODO -- make this actually work with 16 bit arguments!
      updatePos(&ttbuffer[3 + currentPoint * 2], &ttbuffer[3 + currentPoint * 2 + 1], ttbuffer[1], ttbuffer[2], 0);

      byte p1[2];
      bufferProfileToScreen(ttbuffer, currentPoint, p1);
      sprite(triBit, p1[0]  - triBit.width / 2 - 1, p1[1] - triBit.height - 1, WHITE, 0, 0);

      if (currentPoint > 0 && ttbuffer[3 + currentPoint * 2] < ttbuffer[3 + (currentPoint - 1) * 2]) {
        byte verytemptime = ttbuffer[3 + (currentPoint - 1) * 2];
        byte verytemptemp = ttbuffer[3 + (currentPoint - 1) * 2 + 1];

        ttbuffer[3 + (currentPoint - 1) * 2] = ttbuffer[3 + currentPoint * 2];
        ttbuffer[3 + (currentPoint - 1) * 2 + 1] = ttbuffer[3 + currentPoint * 2 + 1];

        ttbuffer[3 + currentPoint * 2] = verytemptime;
        ttbuffer[3 + currentPoint * 2 + 1] = verytemptemp;

        currentPoint--;
      } else if (currentPoint < ttbuffer[0] - 1 && ttbuffer[3 + currentPoint * 2] > ttbuffer[3 + (currentPoint + 1) * 2]) {
        byte verytemptime = ttbuffer[3 + (currentPoint + 1) * 2];
        byte verytemptemp = ttbuffer[3 + (currentPoint + 1) * 2 + 1];

        ttbuffer[3 + (currentPoint + 1) * 2] = ttbuffer[3 + currentPoint * 2];
        ttbuffer[3 + (currentPoint + 1) * 2 + 1] = ttbuffer[3 + currentPoint * 2 + 1];

        ttbuffer[3 + currentPoint * 2] = verytemptime;
        ttbuffer[3 + currentPoint * 2 + 1] = verytemptemp;

        currentPoint++;
      }

      if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
        profileState0 &= ~SELECT;
        for (int i = 0; i < PROFILE_LENGTH; i++) {
          ttbuffer[i] = ttbuffer[i + PROFILE_LENGTH];
        }
        if ((profileState0 & R2ZERO) > 0) {
          profileState0 = 0;
        }
      } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
        profileState0 &= ~SELECT;
        if ((profileState0 & R2ZERO) > 0) {
          profileState0 = 0;
        }
        byte changeCheck = 0;
        for (int i = 0; i < PROFILE_LENGTH; i++) {
          if (ttbuffer[i + PROFILE_LENGTH] != ttbuffer[i]) {
            profileState1 |= CHANGED;
            break;
          }
        }
      }
    }
  } else if ((profileState0 & ADD) > 0) {
    // NEEDS ERROR DIALOGUE!
    if (ttbuffer[0] < 16) {

      for (byte i = 0; i < PROFILE_LENGTH; i++) {
        ttbuffer[i + PROFILE_LENGTH] = ttbuffer[i];
      }
      uint16_t middlepair = (ttbuffer[0] / 2) - 1;
      uint16_t temp1 = ttbuffer[3 + middlepair * 2];
      uint16_t temp2 = ttbuffer[3 + middlepair * 2 + 2];
      temp1 = (temp1 + temp2) / 2;
      uint16_t middlex = temp1;

      temp1 = ttbuffer[3 + middlepair * 2 + 1];
      temp2 = ttbuffer[3 + middlepair * 2 + 3];
      uint16_t middley = (temp1 + temp2) / 2;

      for (byte i = ttbuffer[0] - 1; i > middlepair; i--) {
        ttbuffer[3 + i * 2 + 2] = ttbuffer[3 + i * 2];
        ttbuffer[3 + i * 2 + 3] = ttbuffer[3 + i * 2 + 1];
      }

      ttbuffer[3 + middlepair * 2 + 2] = middlex;
      ttbuffer[3 + middlepair * 2 + 3] = middley;
      ttbuffer[0]++;
      currentPoint = middlepair + 1;
    }

    profileState0 &= ~ADD;
    profileState0 |= ADJUST | SELECT | R2ZERO;
  } else if ((profileState0 & SAVE) > 0) {
    if (dialogueResponse == DIALOGUE_SAVE) {
      //      byte str[] = "Saving.  ";
      //      if (saveStep >= 11) {
      //        str[7] = '.';
      //      }
      //      if (saveStep >= 22) {
      //        str[8] = '.';
      //      }

      uiButtonConst(27, 32 - BUTTON_H_M / 2, 74, BUTTON_H_M, string_saving, 0, 0, 0);

      dialogueResponse = 0;
      profileState0 &= ~SAVE;
      profileState1 &= ~CHANGED;
      saveTempProfile(currentProfile, currentProfileVal, ttbuffer);
      beginSequence(ff7vict);

      // WE NEED A TRANSIENT "SAVED" DIALOGUE HERE!
    } else {

      chooseProfile(&profileState1, string_saveas, &currentProfile, &currentProfileVal);

      if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
        profileState0 &= ~SAVE;
        currentProfile = prevProfile;
        currentProfileVal = prevProfileVal;
      } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
        byte tempstr[4];
        toDec(tempstr, currentProfileVal + 1);
        tempstr[0] = tempstr[1];
        tempstr[1] = tempstr[2];
        tempstr[2] = '?';
        tempstr[3] = 0;
        popupDyn(string_overwriteprofile, tempstr, string_yes, string_cancel, DIALOGUE_SAVE);
      }
    }
  } else if ((profileState0 & CHOOSE) > 0) {
    profileState1 &= ~CHANGED;
    chooseProfile(&profileState1, string_selectprofile, &currentProfile, &currentProfileVal);

    if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
      buttonxy = 1;
      currentMenu = MAIN_MENU;
      beginSequence(back);
    } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
      profileState0 = 0;
    }

  } else if ((profileState0 & DEL) > 0) {
    displayStats(ttbuffer[3 + currentPoint * 2], ttbuffer[3 + currentPoint * 2 + 1]);

    updatePos(&currentPoint, &currentPoint, ttbuffer[0] - 1, ttbuffer[0] - 1, WRAP);
    byte p1[2];
    bufferProfileToScreen(ttbuffer, currentPoint, p1);
    sprite(triBit, p1[0] - triBit.width / 2 - 1, p1[1] - triBit.height - 1, WHITE, 0, 0);

    if (dialogueResponse == DIALOGUE_DELETE) {
      dialogueResponse = 0;
      profileState0 &= ~DEL;
      if (ttbuffer[0] > 6) {
        profileState1 |= CHANGED;
        for (byte i = currentPoint; i < ttbuffer[0] - 1; i++) {
          ttbuffer[3 + i * 2] = ttbuffer[3 + i * 2 + 2];
          ttbuffer[3 + i * 2 + 1] = ttbuffer[3 + i * 2 + 3];
        }
        ttbuffer[0]--;
      }
    }

    if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
      profileState0 &= ~DEL;
    } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
      popupConst(string_deletepoint, string_delete, string_cancel, DIALOGUE_DELETE);
    }
  } else if ((profileState0 & ADJ_AXIS) > 0) {
    byte yoffset = 14;
    byte xoff = 0;
    if ((anim & 32) > 0) {
      xoff = 1;
    }
    sprite(rotTriBit, 64 - BUTTON_W_S / 2 - 6 - xoff, (30 + yoffset), WHITE, FLIPX, 0);
    sprite(rotTriBit, 64 + BUTTON_W_S / 2 + 2 + xoff, (30 + yoffset), WHITE, 0, 0);
    if ((profileState1 & ADJ_X_AXIS) > 0) {
      updatePos(&ttbuffer[1], NULL, 1000, 0, 0);
      uiButtonConst(27, 32 - BUTTON_H_M / 2, 74, BUTTON_H_M, string_adjx, anim, 0, 0);
      uiButtonVal(64 - BUTTON_W_S / 2, (32 + yoffset) - BUTTON_H_S / 2, BUTTON_W_S, BUTTON_H_S, ttbuffer[1], 0, 0, 0);
    } else if ((profileState1 & ADJ_Y_AXIS) > 0) {
      updatePos(&ttbuffer[2], &ttbuffer[2], 1000, 0, 0);
      uiButtonConst(17, 32 - BUTTON_H_M / 2, 94, BUTTON_H_M, string_adjy, anim, 0, 0);
      uiButtonVal(64 - BUTTON_W_S / 2, (32 + yoffset) - BUTTON_H_S / 2, BUTTON_W_S, BUTTON_H_S, ttbuffer[2], 0, 0, 0);
    }

    if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
      profileState0 &= ~ADJ_AXIS;
      profileState1 &= ~(ADJ_Y_AXIS | ADJ_X_AXIS);
      for (int i = 0; i < PROFILE_LENGTH; i++) {
        ttbuffer[i] = ttbuffer[i + PROFILE_LENGTH];
      }
      if ((profileState0 & R2ZERO) > 0) {
        profileState0 = 0;
      }
    } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
      profileState0 &= ~ADJ_AXIS;
      profileState1 &= ~(ADJ_Y_AXIS | ADJ_X_AXIS);
      if ((profileState0 & R2ZERO) > 0) {
        profileState0 = 0;
      }
      byte changeCheck = 0;
      for (int i = 0; i < PROFILE_LENGTH; i++) {
        if (ttbuffer[i + PROFILE_LENGTH] != ttbuffer[i]) {
          profileState1 |= CHANGED;
          break;
        }
      }
    }
  }
  // END OF PROFILES STATES
}

void dialogueMenu() {
  byte tempx = buttonxy >> 4;
  updatePos(&tempx, NULL, 1, 0, WRAP);
  buttonxy = 0;
  buttonxy |= tempx << 4;
  //graphicsRect(16, 16, 98, 32, BLACK|FILL);
  //graphicsClear(BLACK);
  //rect(16, 16, 98, the screen will already be cleared anyway, might want to find a way to avoid this
  setTextColor(WHITE);
  printStringConst(dialogueMessage, 64 - (getLengthConst(dialogueMessage) / 2) * 4, 17);
  // need to include the dynamic part after the above string!
  uiButtonConst(64 - BUTTON_W_L - 2, 48 - BUTTON_H_M - 1, BUTTON_W_L, BUTTON_H_M,
                yesMessage, anim, 0, buttonxy);

  uiButtonConst(64 + 2, 48 - BUTTON_H_M - 1, BUTTON_W_L, BUTTON_H_M,
                noMessage, anim, 1 << 4, buttonxy);
  if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
    currentMenu = prevMenu;
  } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
    if ((buttonxy >> 4) == 1) {
      currentMenu = prevMenu;
    } else {
      currentMenu = prevMenu;
      dialogueResponse = yesResponse;
    }
  }
}

void screenSaver1() {
  static uint16_t box1 = 0;
  static uint16_t box2 = 0;
  static uint16_t box3 = 0;
  static uint16_t box4 = 0;
  static uint16_t wellboy = 230;
  static byte frameboy = 0;

  idleTime = 0;

  sprite(downwell, wellboy >> 8, 28, WHITE, 0, frameboy % downwell.frames);
  frameboy++;
  pixel(40 - (box1 >> 8), 14, 1);
  rect(27 - (box2 >> 8), 16, 2, 2, 1);
  rect(127 - (box3 >> 8), 32, 4, 4, 1);
  rect(100 - (box4 >> 8), 50, 6, 6, 1);
  box1 += 40;
  box2 += 60;
  box3 +=   250;
  box4 += 500;
  wellboy += 400;
  if (byte(wellboy >> 8) > 127 && byte(wellboy >> 8) < 244) {
    wellboy = (244 << 8);
  }
  for (byte i = 0; i < 6; i++) {
    if ((buttons[i] & JUST_PRESSED) > 0) {
      currentMenu = prevMenu;
      break;
    }
  }
}

void screenSaver2() {
  static byte saverState = 0;
  static byte screenTick = 0;
  static byte frameTick = 0;
  static byte boyPos = 240;
  static byte torch1 = 32 - torch.width;
  static byte torch2 = 95;
  static uint16_t treepos = (128) << 8;
#define SAVER_SPEED 2

  idleTime = 0;

  if (saverState == 0) {
    sprite(torch, 32 - torch.width, 16, WHITE, 0, frameTick / 2 % torch.frames);
    sprite(torch, 95, 16, WHITE, FLIPX, frameTick / 2 % torch.frames);
    for (byte i = 0; i < 10; i++) {
      //tile(ceilingCrumble, i* 16, 0, 0, 0);
      //tile(crumbling, i * 16, 64 - 16, 0, 0);
      tile(block, i * 14, 64 - 14, 0, 0);
    }
    sprite(downwell, boyPos, 32, WHITE, 0, frameTick++);
    if (frameTick == downwell.frames) {
      frameTick = 0;
    }
    boyPos += SAVER_SPEED;
    if (boyPos == 60) {
      saverState = 1;
    }
  } else {

    sprite(tree, treepos >> 8, 0, WHITE, 0, 0);
    treepos -= 96;

    if (torch1 != 248) {
      torch1 -= SAVER_SPEED / 2;
      sprite(torch, torch1, 16, WHITE, 0, frameTick / 2 % torch.frames);
    }
    torch2 -= SAVER_SPEED / 2;
    sprite(torch, torch2, 16, WHITE, FLIPX, frameTick / 2 % torch.frames);

    for (byte i = 0; i < 11; i++) {
      //tile(ceilingCrumble, i* 16, 0, 0, 0);
      //tile(crumbling, i * 16 + screenTick, 64 - 16, 0, 0);
      tile(block, i * 14 + screenTick, 64 - 14, 0, 0);
    }
    screenTick -= SAVER_SPEED;
    if (screenTick == 242) {
      screenTick = 0;
    }
    sprite(downwell, 60 + 16 * sin(radians((float)secondTick * (60.0 / 6.2832))), 32, WHITE, 0, frameTick++);
    if (frameTick == downwell.frames) {
      frameTick = 0;
    }
  }
  for (byte i = 0; i < 6; i++) {
    if ((buttons[i] & JUST_PRESSED) > 0) {
      currentMenu = prevMenu;
      break;
    }
  }

}

void reflowMenu() {
  // TODO: Let's have a little cursor that you can move around freely along the backlog
  // (or simply track the newest incoming temperature value);

  // The values that we'll calibrate should be empirically determined and
  // added before we're done as a default setting
  static byte reflowState = REFLOW_IDLE;
  static byte menuy = 0;
  static uint16_t prevProfile = 0;
  static byte prevProfileVal = 0;

  if ((reflowState & REFLOW_IDLE) > 0) {
    updatePos(NULL, &menuy, 0, 2, WRAP | INVERSE_Y);

    //printVal(short(tempAt(currentProfile, 20 + secondTick)), 2, 12);
    //Serial.println(changeAt(currentProfile, 20 + secondTick, 5));
    //    printVal(int(100*changeAt(currentProfile, 10 + secondTick, 3)), 2, 5);
    //    printVal(int(readMAXtemp()), 2, 12);

    printLiteral("P", 52, 56);
    printVal(currentProfileVal + 1, 58, 56);

    uiButtonConst(69, 44 - BUTTON_H_S - 1, BUTTON_W_XL + 11, BUTTON_H_S, string_beginreflow, anim, 0, menuy);
    uiButtonConst(69, 44, BUTTON_W_XL + 11, BUTTON_H_S, string_chooseprofile, anim, 1, menuy);
    uiButtonConst(69, 44 + BUTTON_H_S + 1, BUTTON_W_XL + 11, BUTTON_H_S, string_exit, anim, 2, menuy);

    if (dialogueResponse == DIALOGUE_REFLOW) {
      reflowTime = 0;
      updateTempLog(0, 0, 0, 1);
      dialogueResponse = 0;
      reflowState |= REFLOW_ACTIVE | LOG_TEMP;
      //digitalWrite(4, HIGH);
      reflowState &= ~REFLOW_IDLE;
      // DEBUG
      // Begin
      Serial.write('B');
    }

    // Need to have something like "cancel current logging and begin new reflow?"
    if ((buttons[ARR_CIRCLE] & JUST_PRESSED) == 1) {
      if (menuy == 0) {
        byte tempstr[4];
        toDec(tempstr, currentProfileVal + 1);
        tempstr[0] = tempstr[1];
        tempstr[1] = tempstr[2];
        tempstr[2] = '?';
        tempstr[3] = 0;
        popupDyn(string_beginwithprof, tempstr, string_yes, string_cancel, DIALOGUE_REFLOW);
      } else if (menuy == 1) {
        reflowState |= CHOOSE;
        prevProfile = currentProfile;
        prevProfileVal = currentProfileVal;
        reflowState &= ~REFLOW_IDLE;
      } else if (menuy == 2) {
        if ((reflowState & LOG_TEMP) > 0) {
          // do same thing as b if log temp
        } else {
          currentMenu = MAIN_MENU;
          buttonxy = 0;
        }
      }
    }
    //    if (dialogueResponse == DIALOGUE_REFLOW) {
    //      reflowState |= REFLOW_ACTIVE;
    //      digitalWrite(4, HIGH);
    //      reflowState &= ~REFLOW_IDLE;
    //      reflowState &= ~CHOOSE;
    //    }

    if ((reflowState & LOG_TEMP) > 0) {
      //This is for when someone cancels an active reflow but still want to see the temperature logged
      //displayProfilePoints(currentProfile, currentProfileVal, NULL, 0);

      displayProfile(currentProfile, currentProfileVal, NULL, 0, 0);
      float liveTemp = readMAXtemp();


      sendTemp(liveTemp);

      printStringConst(string_temp, 2, 2);
      printStringConst(string_time, 2, 9);
      printFloat(liveTemp, 21, 2);
      printVal(reflowTime, 21, 9);
      updateTempLog((uint16_t)liveTemp, reflowTime, currentProfile, 0);

      if (frameTick == 0) {
        reflowTime++;
      }

      //      if (frameTick == 0) {
      //        if ((reflowTime & 1) == 0) {
      //          ttbuffer[reflowTime / 2] = (uint16_t)liveTemp;
      //        }
      //        if (reflowTime < 256) {
      //          reflowTime++;
      //        }
      //      }
      //
      //      if (reflowTime / 2 > 1) {
      //        for (uint16_t i = 1; i < reflowTime / 2; i++) {
      //          line(i - 1, 64 - ttbuffer[i - 1] / 4, i, 64 - ttbuffer[i - 1] / 4, WHITE);
      //        }
      //      }

      //do thing

      //if user presses b, ask if they really want to leave with local popup
      if ((buttons[ARR_CROSS] & JUST_PRESSED) > 0) {
        currentMenu = MAIN_MENU;
        buttonxy = 0;
        elementsOff();
      }
    } else {
      // Not actively logging temp, just show the profile
      displayProfile(currentProfile, currentProfileVal, NULL, 0, 1);

      // if they press b, just leave
      if ((buttons[ARR_CROSS] & JUST_PRESSED) > 0) {
        currentMenu = MAIN_MENU;
        buttonxy = 0;
        elementsOff();
      }
    }
  } else if ((reflowState & CHOOSE) > 0) {
    displayProfile(currentProfile, currentProfileVal, NULL, 0, 1);
    chooseProfile(&reflowState, string_selectprofile, &currentProfile, &currentProfileVal);


    if ((buttons[ARR_CROSS] & JUST_PRESSED) > 0) {
      reflowState |= REFLOW_IDLE;
      reflowState &= ~CHOOSE;
      currentProfile = prevProfile;
      currentProfileVal = prevProfileVal;
    } else if ((buttons[ARR_CIRCLE] & JUST_PRESSED) > 0) {
      reflowState |= REFLOW_IDLE;
      reflowState &= ~CHOOSE;
    }
  } else if ((reflowState & REFLOW_ACTIVE) > 0) {
    static float pwm = 0.25;
    //static float serialTemp = 24.0;
    displayProfile(currentProfile, currentProfileVal, NULL, 0, 0);
    float liveTemp = readMAXtemp();
    //float liveTemp = emuReadMAXtemp();

    sendTemp(liveTemp);

    printStringConst(string_temp, 2, 4);
    printStringConst(string_time, 2, 11);
    printFloat(liveTemp, 21, 4);
    printVal(reflowTime, 21, 11);
    updateTempLog((uint16_t)liveTemp, reflowTime, currentProfile, 0);

    pwm = dutyTarget(currentProfile, liveTemp, reflowTime);
    //printFloat(pwm, 110, 16);
    elementsPWM(pwm);

    if (frameTick == 0) {
      reflowTime++;

      // Need a better method here I think,
      // active reflow should end when you need to open door
      uint16_t maxtime;
      EEPROM.get(currentProfile + 2, maxtime);
      if (reflowTime > maxtime) {
        elementsOff();
        reflowState &= ~REFLOW_ACTIVE;
        reflowState |= REFLOW_IDLE;
        Serial.write('E');
      }
    }

    if ((buttons[ARR_CROSS] & JUST_PRESSED) == 1) {
      // we'll need to think of a way to do this without actually
      // changing the menu -- reflow absolutely needs to continue unless
      // the user is sure
      //popupConst(string_cancelreflow, string_yes, string_cancel,
      elementsOff();
      reflowState &= ~REFLOW_ACTIVE;
      reflowState |= REFLOW_IDLE;
    }
  }
}

#define SAVER_WAIT_PERIOD 1200

void menuSys() {

  if (idleTime > SAVER_WAIT_PERIOD) {
    prevMenu = currentMenu;
    currentMenu = SCREEN_SAVER;
  }
  switch (currentMenu) {
    case MAIN_MENU:
      {
        //we could put screen clears here so that dialogues dont make the whole screen black!
        mainMenu();
        break;
      }
    case SETTINGS_MENU:
      {
        settingsMenu();
        break;
      }
    case PROFILES_MENU:
      {
        profilesMenu();
        break;
      }
    case DIALOGUE_MENU:
      {
        dialogueMenu();
        break;
      }
    case SCREEN_SAVER:
      {
        //screenSaver1();
        screenSaver2();
        break;
      }
    case REFLOW_MENU:
      {
        reflowMenu();
        idleTime = 0;
        break;
      }
    default:
      currentMenu = 0;
      break;
  }
}
