int test = 0;
float tick = 0;

void setup(){
  process("corvusprudensborder.png", 1, 1, 128, 32, 128, 32, false);
  print(chars[0].length);
  // print(chars[0]);
  writeToHex("output.hex", 128, 32, 128);
  size(512, 128);
  noSmooth();
  testimg = displayBitmap(chars[0], 128, 32, 0);
  frameRate(60);
}

void draw(){
  tick += 0.2;
  image(testimg, 0, 0, width, height);
  // testimg = displayBitmap(chars[int(tick) % (chars.length)], 16, 16, 0);
}
