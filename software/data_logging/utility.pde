float buffer[] = new float[60];

float avg(float val, float buff[]){
  float cum = 0;
  for (int i = 1; i < buff.length; i++){
   buff[i] = buff[i - 1];
   cum += buff[i - 1];
  }
  buff[0] = val;
  cum += val;
  return cum/buff.length;
}
