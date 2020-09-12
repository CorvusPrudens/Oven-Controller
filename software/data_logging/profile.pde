class Profile{
  PVector points[] = new PVector[16];
  int numPoints;
  
  Profile(){
    this.points[0] = new PVector(0, 25);
    this.points[1] = new PVector(75, 50);
    this.points[2] = new PVector(95, 100);
    this.points[3] = new PVector(165, 150);
    this.points[4] = new PVector(195, 183);
    this.points[5] = new PVector(255, 235);
    this.points[6] = new PVector(285, 183);
    this.numPoints = 7;
    //this.points[0] = new PVector(0, 25);
    //this.points[1] = new PVector(90, 50);
    //this.points[2] = new PVector(110, 100);
    //this.points[3] = new PVector(180, 150);
    //this.points[4] = new PVector(210, 183);
    //this.points[5] = new PVector(270, 235);
    //this.points[6] = new PVector(300, 183);
    //this.numPoints = 7;
    
    //this.points[0] = new PVector(0, 24);
    //this.points[1] = new PVector(30, 100);
    //this.points[2] = new PVector(120, 150);
    //this.points[3] = new PVector(150, 183);
    //this.points[4] = new PVector(210, 235);
    //this.points[5] = new PVector(240, 183);
    
    //this.points[0] = new PVector(0, 24);
    //this.points[1] = new PVector(70, 150);
    //this.points[2] = new PVector(190, 180);
    //this.points[3] = new PVector(240, 245);
    //this.points[4] = new PVector(260, 245);
    //this.points[5] = new PVector(360, 24);
    //this.numPoints = 6;
  }
  
  
  float tempAt(float time){
    float temp = 0;
    if (time > this.points[numPoints - 1].x){
      return -1;
    }
    int targindex = 0;
    for (int i = 1; i < numPoints; i++){
     if (time < this.points[i].x){
       targindex = i - 1;
       break;
     }else if (time == this.points[i].x){
      return (this.points[i].y); 
     }
    }
    
    float segAng = atan2((this.points[targindex + 1].y - this.points[targindex].y),
                         this.points[targindex + 1].x - this.points[targindex].x);
    
    float xdiff = time - this.points[targindex].x;
    temp = this.points[targindex].y + tan(segAng)*xdiff;
    
    return temp;
  }
  
  // Rate of change in degrees C per second
  float changeAt(float time, int lookahead){
    float change = 0;
    
    for (int i = 0; i < lookahead; i++){
      change += this.tempAt(i + time + 1) - this.tempAt(i + time);
    }

    return change/(float)lookahead;
  }
  
  void show(float maxt, float maxPossibleTemp){
    stroke(72, 139, 73);
    for (int i = 1; i < this.numPoints; i++){
      line(map(this.points[i - 1].x, 0, maxt, 0, width),
           map(this.points[i - 1].y, 0, maxPossibleTemp, height, 0),
           map(this.points[i].x, 0, maxt, 0, width),
           map(this.points[i].y, 0, maxPossibleTemp, height, 0));
    }
  }
}
