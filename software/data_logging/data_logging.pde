import processing.serial.*;

Serial arduino;
Element element;
Profile profile;
Log log;
float tick = 0;
boolean override = false;
int reflowTime = 0;
int currentTime;
int startTime;

void setup() {
  frameRate(30);
  size(1000, 500);
  element = new Element(24, 3, 0.01);
  profile = new Profile();
  prevpwm = 0;
  arduino = new Serial(this, Serial.list()[0], 115200);

  log = new Log();
}

boolean heating = false;
boolean begin = false;
int arduinoPWM = 0;
int frameTick = 0;

void draw() {
  background(49, 47, 47);

  if (arduino.available() > 0) {
    byte buff[] = new byte[32];
    if (!begin && arduino.read() == 'B') {
      begin = true;
      startTime = millis();
    } else if (arduino.read() == '<') {
      int len = arduino.readBytesUntil('>', buff);
      String fl = "";
      for (int i = 0; i < len - 1; i++) {
        fl += str(char(buff[i]));
      }
      log.addPoint(float(fl), millis() - startTime);
      arduino.clear();
    }
    begin = true;
  }
  if (frameTick < arduinoPWM) {
    heating = true;
  } else {
    heating = false;
  }

  if (element.numPoints > 0) {
    arduino.write(nf(element.temperature, 0, 2));
    text(nf(element.temperature, 0, 2), 60, 10);
    arduino.write('f');
  }

  log.display(360, 300);
  profile.show(360, 300);

  if (frameTick++ == 29) {
    frameTick = 0;
    if (begin) {
      reflowTime++;
    }
  }

  fill(255);
  if (log.numPoints > 0) {
    text("Time: " + nf(float(millis() - startTime)/1000.0, 0, 2), 10, 15);
    text("Temp: " + str(log.points[log.numPoints - 1].y), 10, 30);
    text("Change: " + nf(log.change(5), 0, 3), 10, 45);
    text("tempAt: " + nf(profile.tempAt(int(float(millis() - startTime)/1000.0)), 0, 2), 10, 60);
  }
}

void keyPressed() {
  if (key == ' ') {
    override = true;
  }
  if (key == 'r') {
    element.reset();
    tick = 0;
  }
  if (key == 's') {
    log.save();
  }
}
