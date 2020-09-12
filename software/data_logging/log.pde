// needs more work
class Log {

  PVector points[];
  int numPoints = 0;
  String lines[];

  Log() {
    points = new PVector[360*30];
  }

  void addPoint(float point, int time) {
    if (this.numPoints < points.length - 1 && !Float.isNaN(point)) {
      this.points[this.numPoints++] = new PVector(float(time)/1000.0, point);
    }
  }

  float change(float depth) {
    depth *= 30;
    if (numPoints - depth - 1 < 0) {
      return -1;
    }
    float change = 0;
    //for (int i = 0; i < depth; i++) {
    //  change += this.points[this.numPoints - i - 1].y - this.points[this.numPoints - i - 2].y;
    //}
    //return change/((this.points[this.numPoints - 1].x - this.points[this.numPoints - int(depth) - 1].x)/30.0);
    
    change = this.points[this.numPoints - 1].y - this.points[this.numPoints - int(depth) - 1].y;
    return change/(this.points[this.numPoints - 1].x - this.points[this.numPoints - int(depth) - 1].x);
  }

  void save() {
    String tim = "logs/log_" + str(month()) + "." + str(day()) + "." + str(year()) + "_" 
                 + str(hour()) + "."+ str(minute()) + "." + str(second()) + ".csv";
                 
    lines = new String[this.numPoints];
    for (int i = 0; i < this.numPoints; i++) {
      lines[i] = str(this.points[i].x) + ", " + str(this.points[i].y) + ", ";
    }
    
    
    saveStrings(tim, lines);
  }

  //x and y scale in seconds
  void display(int xscale, int yscale) {
    
    for (int i = 1; i < 10; i++) {
      float temp = height - i*(height/10);
      stroke(60);
      line (0, temp, width, temp);
      text(round(map(temp, height, 0, 0, yscale)), width - 50, temp - 10);

      float time = i*width/10;
      line(time, 0, time, height);
      text(round(map(time, 0, width, 0, xscale)), time + 2, height - 10);
    }

    if (numPoints > 1) {
      for (int i = 1; i < numPoints; i++) {
        strokeWeight(2);
        stroke(80, 125, 188);
        //line(map(float(i - 1)/dataRate, 0, xscale, 0, width), 
        //  map(points[i - 1], 0, yscale, height, 0), 
        //  map(float(i)/dataRate, 0, xscale, 0, width), 
        //  map(points[i], 0, yscale, height, 0));
        point(map(points[i - 1].x, 0, xscale, 0, width), 
          map(points[i - 1].y, 0, yscale, height, 0));
      }
      float convertedX = map(mouseX, 0, width, 0, xscale);
      if (convertedX < points[this.numPoints - 1].x) {

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
        circle(mouseX, map(points[index].y, 0, yscale, height, 0), 10);
        text(nf(points[index].y, 0, 2) + " C", mouseX, map(points[index].y, 0, yscale, height, 0) - 35);
        // gonna need to find a way to illustrate time
        text(nf(points[index].x, 0, 2) + " S", mouseX, map(points[index].y, 0, yscale, height, 0) - 20);
      }
    }
  }
}
