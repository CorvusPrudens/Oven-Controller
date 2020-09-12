#define THERMAL_INERTIA 22

const float defaultMaxChange[] PROGMEM = {
  0.8,
  0.8,
  0.8,
  0.7,
  0.6
};

const float defaultMinPWM[] PROGMEM = {
  0.4,
  0.4,
  0.4,
  0.55,
  0.65
};

const uint16_t defaultOvershoot[] PROGMEM = {
  25,
  25
}; 


// The SSR pin is D4
void elementsOn() {
  Serial.write(30);
  PORTD |= 0b00010000;
}

void elementsOff() {
  // debug
  Serial.write(0);
  PORTD &= 0b11101111;
}

void elementsPWM(float duty) {
  float translate = duty * 30;
  //debug mode
  Serial.write((byte)round(translate));
  //printVal(translate, 110, 50);
  if (frameTick < translate) {
    elementsOn();
    //debug
    //Serial.write(1);
  } else {
    elementsOff();
    //Serial.write(0);
  }
}

// The temperatures will be implicit:
// 0-50, 50-100, 100-150, 150-200, 200-250

// Default values that we'll use for now
float maxChange[5] = {
  0.8,
  0.8,
  0.8,
  0.7,
  0.6
};
float minPWM[5] = {
  0.4,
  0.4,
  0.4,
  0.55,
  0.65
};

// 100, 200
uint16_t overshoot[2];

byte openDoor = 0;

// I should really just store these values as uint16_t and cut the number of available profiles in half!!!!!!
// The x and y position on screen should be mapped from the maximum x and y positions (maybe include these in profile array?)

float tempAt(uint16_t profile, uint16_t tim) {
  // need to make exception for when a profile isn't saved to eeprom
  // oh yeah this caused an error and low key scared me
  float temp = 0;
  int16_t numPoints;
  int16_t tempval0;
  int16_t tempval1;
  int16_t tempval2;
  int16_t tempval3;
  EEPROM.get(profile, numPoints);

  EEPROM.get(profile + 6 + (numPoints - 1) * 4, tempval0);

  if (tim >= tempval0) {
    EEPROM.get(profile + 6 + (numPoints - 1) * 4 + 2, tempval0);
    return tempval0;
  }
//  else {
//    // this is a total hack, but should work for now
//    EEPROM.get(profile + 6 + (numPoints - 2) * 4, tempval0);
//    if (tim > tempval0){
//      return 50;
//    }
//  }
  
  uint16_t targindex = 0;
  // There seems to be a bug when we search between the last two points
  for (byte i = 1; i < numPoints; i++) {
    EEPROM.get(profile + 6 + i * 4, tempval0);
    if (tim < tempval0) {
      targindex = (i - 1) * 4 + 6; // this is a little confusing, but necessary with our limitations.
      // the target index is the previous point, so i - 1. The times four is because the x and y values are stored
      // in a 1d array and they are both 2 bes. The plus 6 is because the beginning of the array stores how many points follow and stuff.
      break;
    } else if (tim == tempval0) {
      EEPROM.get(profile + 6 + i * 4 + 2, tempval0);
      return (tempval0);
    }
  }

  //printVal((targindex - 6)/4 - 1, 64, 50);
  
  EEPROM.get(profile + targindex + 6, tempval0);
  EEPROM.get(profile + targindex + 2, tempval1);
  EEPROM.get(profile + targindex + 4, tempval2);
  EEPROM.get(profile + targindex, tempval3);
  float segAng = atan2((tempval0 - tempval1),     //y2 - y1
                       (tempval2 - tempval3));      //x2 - x1

  float xdiff = tim - tempval3;
  temp = float(tempval1) + tan(segAng) * xdiff;

  return temp;
}

// Rate of change in degrees C per second
float changeAt(uint16_t profile, float tim, byte lookahead) {
  float change = 0;

  for (byte i = 0; i < lookahead; i++) {
    change += tempAt(profile, i + tim + 1) - tempAt(profile, i + tim);
  }

  return change / (float)lookahead;
}

float changeTo(float tempNow, uint16_t profile, float tim, byte lookahead){
  float avgtemp = 0;
  float change = 0;

  // getting the average temperature of the next <lookahead> number of seconds
  for (byte i = 0; i < lookahead; i++){
    avgtemp += tempAt(profile, tim + 1 + i + THERMAL_INERTIA);
  }
  avgtemp /= (float)lookahead;
  float avgtim = (20 + lookahead + 1)/2.0;

//  printFloat(avgtemp, 110, 32);
//  printFloat(tempAt(profile, tim + 1 + THERMAL_INERTIA), 110, 39);
  //printFloat (avgtim, 110, 39);
  return (avgtemp - tempNow)/avgtim;
  // basically, just calculate the change necessary between the current temperature and the
  // temperature we're supposed to be at, and then look up what PWM that chagne corresponds to
  // at our current temperature
}

// Note, each index rerpresesnts a time change of 2 seconds!
float changeAtMeasurement(uint16_t* tempbuff, uint16_t tim, byte lookbehind) {
  float change = 0;

  for (byte i = 0; i < lookbehind; i++) {
    change += float(tempbuff[tim - i] - tempbuff[tim - i - 1])/2;
  }
  return change / (float)lookbehind;
}

float lookbehindBuffer[5];

float changeAtMeasurement(float* tempbuff, float temp){
  static byte depth = 0;
  float change = 0.5;
  for (byte i = 3; i < 255; i--){
    tempbuff[i + 1] = tempbuff[i];
  }
  tempbuff[0] = temp;

  if (depth < 5){
    depth++;
  } else{
    change = 0;
    for (byte i = 1; i < 5; i++){
      // these will be acquired every second instead of every two
      change += (tempbuff[i - 1] - tempbuff[i]);
    }
    change /= 4.0;
  }
  
  return change;
}

// MAX RATE OF CHANGE TO TARGET: Maximum oven capabilities
// MIN RATE OF CHANGE TO TARGET: 0.5 C/s



//    // The temperatures will be implicit:
//    // 0-50, 50-100, 100-150, 150-200, 200-250
//    float maxChange[5];
//    float minPWM[5];
//
//    // 100, 200
//    float overshoot[2];

//    boolean openDoor = false;

// Make sure to add the current temperature to ttbuffer before passing it to this function
byte maxTesting(uint16_t* calibrateTime, uint16_t* tempbuff) {

  static byte done = 0;
  static byte  calibrateState = 0;
  int overshoot100 = 0;
  int overshoot200 = 0;
  static byte prevcalibrateState = 0;

  // Max Temp Test
  if (calibrateState < 5) {
    prevcalibrateState = calibrateState;
    switch (calibrateState) {
      case 0:
        {
          elementsOn();
          if (*calibrateTime > 0) {
            if (tempbuff[*calibrateTime - 1] > 100) {
              elementsOff();
              calibrateState = 1;
              overshoot100 = *calibrateTime;
            }
          }
          break;
        }
      case 1:
        {
          if (tempbuff[*calibrateTime - 1] < 100) {
            calibrateState = 2;
            elementsOn();
          }
          break;
        }
      case 2:
        {
          if (tempbuff[*calibrateTime - 1] > 200) {
            elementsOff();
            calibrateState = 3;
            overshoot200 = int(*calibrateTime);
          }
          break;
        }
      case 3:
        {
          if (tempbuff[*calibrateTime - 1] < 200) {
            calibrateState = 4;
            elementsOn();
          }
          break;
        }
      case 4:
        {
          if (tempbuff[*calibrateTime - 1] >= 250) {
            calibrateState = 5;
            elementsOff();
          }
          break;
        }
    }
  } else if (calibrateState == 5) {
    //overshoots
    uint16_t overshootEnd100 = 0;
    uint16_t overshootEnd200 = 0;
    uint16_t highestShoot = 100;
    for (uint16_t i = overshoot100 + 1; i < *calibrateTime; i++) {
      if (tempbuff[i] > highestShoot) {
        highestShoot = tempbuff[i];
      } else if (tempbuff[i] < 100) {
        overshootEnd100 = i;
        break;
      }
    }
    overshoot[0] = highestShoot - 100;

    highestShoot = 200;
    for (uint16_t i = overshoot200 + 1; i < *calibrateTime; i++) {
      if (tempbuff[i] > highestShoot) {
        highestShoot = tempbuff[i];
      } else if (tempbuff[i] < 200) {
        overshootEnd200 = i;
        break;
      }
    }
    overshoot[1] = highestShoot - 200;


    //max change bands
    uint16_t topindex = 0;
    for (uint16_t i = 0; i < *calibrateTime; i++) {
      if (tempbuff[i] >= 50) {
        topindex = i;
        break;
      }
    }
    maxChange[0] = (float(tempbuff[topindex] - tempbuff[0]) / topindex)/2.0;

    int prevTop = topindex;
    for (int i = prevTop; i < *calibrateTime; i++) {
      if (tempbuff[i] >= 100) {
        topindex = i;
        break;
      }
    }
    maxChange[1] = (float(tempbuff[topindex] - tempbuff[prevTop]) / (topindex - prevTop))/2.0;

    prevTop = overshootEnd100;
    for (int i = prevTop; i < *calibrateTime; i++) {
      if (tempbuff[i] >= 150) {
        topindex = i;
        break;
      }
    }
    maxChange[2] = (float(tempbuff[topindex] - tempbuff[prevTop]) / (topindex - prevTop))/2.0;

    prevTop = topindex;
    for (int i = prevTop; i < *calibrateTime; i++) {
      if (tempbuff[i] >= 200) {
        topindex = i;
        break;
      }
    }
    maxChange[3] = (float(tempbuff[topindex] - tempbuff[prevTop]) / (topindex - prevTop))/2.0;

    prevTop = overshootEnd200;
    for (int i = prevTop; i < *calibrateTime; i++) {
      if (tempbuff[i] >= 250) {
        topindex = i;
        break;
      }
    }
    maxChange[4] = (float(250 - tempbuff[prevTop]) / (topindex - prevTop))/2.0;
    return 1;
  }
  return 0;
}

// make sure oven has cooled and
// set calibrate time to 0 before beginning this
byte minTesting(uint16_t* calibrateTime, uint16_t* tempbuff) {
  //minPWM testing
  static float tempDuty = 0.1;

  if (*calibrateTime > 10) {
    float tempChange = changeAtMeasurement(tempbuff, *calibrateTime/2 - 1, 4);
    printVal(tempChange*100, 2, 16);
    printVal(tempDuty*100, 2, 23);
    uint16_t temptemp = tempbuff[*calibrateTime/2 - 1];
    // bit of a hack, but whatever
    if (tempChange < 0.4 && tempDuty < 1) {
      // max rate of calibration adjustment is 1% per second
      tempDuty += 0.002 / 30.0;
    } else if (tempChange > 0.6 && tempDuty > 0) {
      tempDuty -= 0.002 / 30.0;
    }

    if (temptemp > 45 && temptemp < 50) {
      minPWM[0] = tempDuty;
    } else if (temptemp > 95 && temptemp < 100) {
      minPWM[1] = tempDuty;
    } else if (temptemp > 145 && temptemp < 150) {
      minPWM[2] = tempDuty;
    } else if (temptemp > 195 && temptemp < 200) {
      minPWM[3] = tempDuty;
    } else if (temptemp > 245 && temptemp < 250) {
      minPWM[4] = tempDuty;
      elementsPWM(0);
      return 1;
    }
  }
  elementsPWM(tempDuty);
  return 0;
}

// We should create 2 or 3 slots for this as well
#define CALIBRATION_OFFSET 750
void saveCalibration(){
  for (byte i = 0; i < 5; i++){
    EEPROM.put(CALIBRATION_OFFSET + i*4, maxChange[i]);
    EEPROM.put(CALIBRATION_OFFSET + 20 + i*4, minPWM[i]);
  }
  EEPROM.put(CALIBRATION_OFFSET + 40, overshoot[0]);
  EEPROM.put(CALIBRATION_OFFSET + 42, overshoot[1]);
}

// This is the culmination of all the data!
float dutyTarget(uint16_t profileNum, float floattemp, uint16_t calibrateTime) {
//  static float offset = 0;
  static float currentChange = 0.5;

  // Bit of a hack, but all on allows the oven to begin heating up
  uint16_t point1;
  EEPROM.get(profileNum + 6 + 4, point1);
  if (calibrateTime < point1){
    return 1.0;
  }
  float duty = 0.5;
  int changeArrIndex = 0;
  float targetChange = 0;

  if (frameTick == 0){
    currentChange = changeAtMeasurement(lookbehindBuffer, floattemp);
  }
  
  if (calibrateTime > 5) {

    targetChange = changeTo(floattemp, profileNum, calibrateTime, 4);
    byte currentTempBandIndex = (floattemp / 50);

    // If the requested rate of change is greater or nearly equal to the possible oven output...
    if (targetChange > maxChange[currentTempBandIndex]) {
      duty = 1;
    } else if (targetChange < 0.5 && targetChange > 0) {
      // If the requested rate of change is less than 0.5 degrees per second, return the
      // lowest PWM duty as given by the data
      duty =  minPWM[currentTempBandIndex];
    }
//    else if (targetChange < 0) {
//      openDoor = true;
//      duty =  0;
//    } 
    else {
      // Now for the hard stuff
      // We'll assume that the min calibration test gave good data and
      // successfully maintained a rate of change of 0.5 C/s
      // We can of course remove this assumption if we also measure
      // the amount of change during the min change test

      // We'll linearly interpolate between the max rate of change and 0.5 and
      // multiply the PWM by the resulting relationship (look im tired)

      float ratio = (targetChange - 0.5) / (maxChange[currentTempBandIndex] - 0.5);
      float PWMrange = 1 - minPWM[currentTempBandIndex];

      duty = minPWM[currentTempBandIndex] + ratio * PWMrange;

      // Hopefully our current setup will render this fudging uneccesary
      // Although I suspect some will have to occur since my calibration 
      // settings won't be perfect
//      if (currentTemp > targetTemp) {
//        if (offset > -0.2) {
//          offset -= 0.02 / 30;
//        }
//      } else if (currentTemp < targetTemp) {
//        if (offset < 0.2) {
//          offset += 0.02 / 30;
//        }
//      }
    }
//    if (currentTemp > targetTemp) {
//      if (offset > -0.2) {
//        offset -= 0.02 / 30;
//      }
//    } else if (currentTemp < targetTemp) {
//      if (offset < 0.2) {
//        offset += 0.02 / 30;
//      }
//    }
//    duty += offset;
  }
  //println(changeArrIndex, nf(duty, 0, 2), nf(offset, 0, 2), nf(*calibrateTime, 0, 2), nf(targetChange, 0, 2));
  if (openDoor) {
    duty = 0;
  }

  if (floattemp + THERMAL_INERTIA*currentChange > tempAt(profileNum, calibrateTime + THERMAL_INERTIA) + 5){
    duty = 0;
  }

//  printFloat(currentChange, 110, 0);
//  printFloat(targetChange, 110, 7);
  
  return duty;
}
