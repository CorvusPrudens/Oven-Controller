
#define MAX31856_CONFIG_REG 0x00
#define MAX31856_CONFIG_BIAS 0x80
#define MAX31856_CONFIG_MODEAUTO 0x40
#define MAX31856_CONFIG_MODEOFF 0x00
#define MAX31856_CONFIG_1SHOT 0x20
#define MAX31856_CONFIG_3WIRE 0x10
#define MAX31856_CONFIG_24WIRE 0x00
#define MAX31856_CONFIG_FAULTSTAT 0x02
#define MAX31856_CONFIG_FILT50HZ 0x01
#define MAX31856_CONFIG_FILT60HZ 0x00

#define MAX31856_RTDMSB_REG 0x01
#define MAX31856_RTDLSB_REG 0x02
#define MAX31856_HFAULTMSB_REG 0x03
#define MAX31856_HFAULTLSB_REG 0x04
#define MAX31856_LFAULTMSB_REG 0x05
#define MAX31856_LFAULTLSB_REG 0x06
#define MAX31856_FAULTSTAT_REG 0x07

#define MAX31865_FAULT_HIGHTHRESH 0x80
#define MAX31865_FAULT_LOWTHRESH 0x40
#define MAX31865_FAULT_REFINLOW 0x20
#define MAX31865_FAULT_REFINHIGH 0x10
#define MAX31865_FAULT_RTDINLOW 0x08
#define MAX31865_FAULT_OVUV 0x04

#define REF_RESISTOR 400.0

#define maxCS 0b10 // PORTB (pin D9)
SPISettings pt100SPI = SPISettings(1000000, MSBFIRST, SPI_MODE1);

byte readMAXbyte(byte addr) {
  byte ret;
  addr &= 0x7F;
  PORTB &= ~maxCS;
  SPI.beginTransaction(pt100SPI);
  SPI.transfer(addr);
  ret = SPI.transfer(0xFF);
  PORTB |= maxCS;
  SPI.endTransaction();
  return ret;
}

byte writeMAXbyte(byte addr, byte val) {
  addr |= 0x80;
  //uint16_t buff = val | (addr << 8);
  SPI.beginTransaction(pt100SPI);
  PORTB &= ~maxCS;

  SPI.transfer(addr);
  SPI.transfer(val);
  //SPI.transfer16(buff);
  PORTB |= maxCS;
  SPI.endTransaction();
}

uint16_t readMAXword(byte addr) {
  uint16_t ret;
  addr &= 0x7F;
  SPI.beginTransaction(pt100SPI);
  PORTB &= ~maxCS;

  SPI.transfer(addr);
  ret = SPI.transfer(0xFF) << 8;
  //SPI.transfer(addr + 1);
  ret |= SPI.transfer(0xFF);
  PORTB |= maxCS;
  SPI.endTransaction();
  return ret;
}


float celsius(float R_ohms)
{
  return -247.29 + R_ohms
    * ( 2.3992 + R_ohms * (0.00063962 + 1.0241E-6 * R_ohms));
}

byte readMAXfault() {
  return readMAXbyte(MAX31856_FAULTSTAT_REG);
}

void clearMAXfault() {
  byte t = readMAXbyte(MAX31856_CONFIG_REG);
  t &= ~0x2C;
  t |= MAX31856_CONFIG_FAULTSTAT;
  writeMAXbyte(MAX31856_CONFIG_REG, t);
}

void enableMAXbias(byte b) {
  byte t = readMAXbyte(MAX31856_CONFIG_REG);
  if (b == 1) {
    t |= MAX31856_CONFIG_BIAS;
  } else {
    t &= ~MAX31856_CONFIG_BIAS;
  }
  writeMAXbyte(MAX31856_CONFIG_REG, t);
}

void autoConvert(byte b) {
  byte t = readMAXbyte(MAX31856_CONFIG_REG);
  if (b == 1) {
    t |= MAX31856_CONFIG_MODEAUTO; // enable autoconvert
  } else {
    t &= ~MAX31856_CONFIG_MODEAUTO; // disable autoconvert
  }
  writeMAXbyte(MAX31856_CONFIG_REG, t);
}

void set3Wires() {
  byte t = readMAXbyte(MAX31856_CONFIG_REG);

  t |= MAX31856_CONFIG_3WIRE;

  writeMAXbyte(MAX31856_CONFIG_REG, t);
}

float readMAXtemp() {
  static byte readState = 0;
  static float ret = 24.0;
  switch(readState++){
    case 0:
    {
      clearMAXfault();
      enableMAXbias(true);
      break;
    }
    case 1:
    {
      byte t = readMAXbyte(0);
      t |= MAX31856_CONFIG_1SHOT;
      writeMAXbyte(0, t);
      break;
    }
    case 2:
    {
      readState = 0;
      ret = celsius((float(readMAXword(MAX31856_RTDMSB_REG) >> 1) / 32768.0)
        * REF_RESISTOR);
      break;
    }
    default:
      break;
  }
  return ret;
}

void beginMAX() {
  writeMAXbyte(MAX31856_HFAULTMSB_REG, 0b10010000);
  writeMAXbyte(MAX31856_LFAULTMSB_REG, 0b00100000);

  set3Wires();
  clearMAXfault();
  //autoConvert(true);
}
