int test = 0;
float tick = 0;

void setup(){
  process("running.png", 4, 4, 16, 16, 16, 16, false);
  writeToFile("output.txt", 16, 16, 4);
  size(600, 600);
  noSmooth();
  testimg = displayBitmap(chars[4], 16, 16, 0);
  frameRate(60);
}

void draw(){
  tick += 0.2;
  image(testimg, 0, 0, width, height);
  testimg = displayBitmap(chars[int(tick) % (chars.length)], 16, 16, 0);
}
