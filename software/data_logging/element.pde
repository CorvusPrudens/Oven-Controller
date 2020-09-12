float prevpwm = 0;
boolean pwm(float t, float hz, float duty) {
  float period = 1.0/hz;
  boolean high = false;
  if (t - prevpwm > period) {
    prevpwm = t;
  }
  float wavepos = t - prevpwm;
  if (wavepos/period < duty) {
    high = true;
  }
  return high;
}

class Element {
  float ambient = 24;
  float temperature = ambient + 1;
  float thermalAccel = 0;
  float accelInc = 0.1;
  float resistenceRatio = 0.1;
  float overAccel = 0;

  PVector points[] = new PVector[2048];
  int numPoints = 0;
  float prevTime = 0;
  float delta = 0;
  boolean done = false;

  Element() {
  }

  Element(float amb, float acc, float res) {
    this.ambient = amb;
    this.accelInc = acc;
    this.resistenceRatio = res;
  }

  void heatLinear(float timeinc, boolean on) {
    if (on) {
      this.temperature += this.accelInc*timeinc;
    }
    this.temperature -= (this.temperature - this.ambient)*this.resistenceRatio*timeinc;
    //this.temperature += thermalAcceleration;
  }
  
  void heatOvershoot(float timeinc, boolean on){
    
    if (on) {
      this.thermalAccel += ((this.accelInc - this.thermalAccel)/4)*timeinc;
    }else{
      this.thermalAccel -= (this.thermalAccel/16)*timeinc;
    }
    this.temperature += this.thermalAccel*timeinc;
    this.temperature -= (this.temperature - this.ambient)*this.resistenceRatio*timeinc;
    //this.temperature += thermalAcceleration;
  }

  void plot(float t, boolean on) {

    float d = t - this.delta;
    this.delta = t;
    //this.heatLinear(d, on);
    this.heatOvershoot(d, on);
    if (t - prevTime >= 1) {
      prevTime = t;
      points[numPoints++] = new PVector(t, this.temperature);
    }
  }
  
  float changeAt(float t, float depth){
    if (int(t) - depth < 0){
     return 0; 
    }
    float change = 0;
    for (int i = 0; i < depth; i++){
     change += this.points[int(t) - i].y - this.points[(int(t) - i) - 1].y; 
    }
    
    return change/depth;
  }

  void reset() {
    this.numPoints = 0;
    this.done = false;
    this.temperature = ambient + 1;
    this.thermalAccel = 0;
    this.prevTime = 0;
    this.delta = 0;
  }

  void show(float maxt, float maxPossibleTemp) {
    for (int i = 1; i < 10; i++) {
      float temp = height - i*(height/10);
      stroke(60);
      line (0, temp, width, temp);
      text(round(map(temp, height, 0, 0, maxPossibleTemp)), width - 50, temp - 10);

      float time = i*width/10;
      line(time, 0, time, height);
      text(round(map(time, 0, width, 0, maxt)), time + 2, height - 10);
    }


    if (numPoints > 1) {
      for (int i = 1; i < numPoints; i++) {
        strokeWeight(2);
        stroke(80, 125, 188);
        line(map(points[i - 1].x, 0, maxt, 0, width), 
          map(points[i - 1].y, 0, maxPossibleTemp, height, 0), 
          map(points[i].x, 0, maxt, 0, width), 
          map(points[i].y, 0, maxPossibleTemp, height, 0));
      }
      float convertedX = map(mouseX, 0, width, 0, maxt);
      if (convertedX < points[numPoints - 1].x) {

        float closestX = 1000;
        int index = 0;
        for (int i = 0; i < numPoints; i++) {
          if  (abs(convertedX - points[i].x) < closestX) {
            closestX = abs(convertedX - points[i].x);
            index = i;
          }
        }
        noStroke();
        fill(197, 186, 175);
        circle(mouseX, map(points[index].y, 0, maxPossibleTemp, height, 0), 10);
        text(str(round(points[index].y)) + " C", mouseX, map(points[index].y, 0, maxPossibleTemp, height, 0) - 35);
        text(str(round(points[index].x)) + " S", mouseX, map(points[index].y, 0, maxPossibleTemp, height, 0) - 20);
      }
    }
  }
}
