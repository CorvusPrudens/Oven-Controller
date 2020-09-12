int wordLen = 8;
PImage img;
PImage testimg;
int chars[][];

int idx(int x, int y, int w) {
  return  x + y*w;
}
// need to make this accept any word width (not just 8 bit words)
int[] extractChar(PImage image, int x, int y, int w, int h, boolean inverted) {

  int tempval = 0;
  if (h > 0 && (h & (wordLen - 1)) == 0) {
    tempval = 1 + ((h - 1)/wordLen);
  } else {
    tempval = 1 + h/wordLen;
  }
  int bits[] = new int[w*tempval];
  image.loadPixels();

  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      color pixel = image.pixels[idx(x + i, y + j, image.width)];
      int val;
      if (!inverted) {
        val = int(round(brightness(pixel)/255)) << (j & (wordLen - 1));
      } else {
        val = 1 - int(round(brightness(pixel)/255)) << (j & (wordLen - 1));
      }
      bits[idx(i, floor(j/wordLen), w)] |= val;
    }
  }

  return bits;
}

void process(String path, int xnum, int ynum, int xgrid, int ygrid, int w, int h, boolean inverted) {
  img = loadImage(path);
  chars = new int[xnum*ynum][w*floor((h + 1)/wordLen)];
  img.loadPixels();

  for (int i = 0; i < ynum; i++) {
    for (int j = 0; j < xnum; j++) {
      chars[idx(j, i, xnum)] = extractChar(img, j*xgrid, i*ygrid, w, h, inverted);
    }
  }
}

void process(String path, int xoff, int yoff, int xnum, int ynum, int xgrid, int ygrid, int w, int h, boolean inverted) {
  img = loadImage(path);
  chars = new int[xnum*ynum][w*floor((h + 1)/wordLen)];
  img.loadPixels();

  for (int i = 0; i < ynum; i++) {
    for (int j = 0; j < xnum; j++) {
      chars[idx(j, i, xnum)] = extractChar(img, xoff + j*xgrid, yoff + i*ygrid, w, h, inverted);
    }
  }
}

void writeToFile(String path, int w, int h, int gridx) {
  // NEEDS FIXING!
  int tempval = 0;
  if (h > 0 && (h & (wordLen - 1)) == 0) {
    tempval = 1 + ((h - 1)/wordLen);
  } else {
    tempval = 1 + h/wordLen;
  }

  String lines[] = new String[chars.length*(1 + tempval)];
  for (int i = 0; i < chars.length; i++) {
    lines[i*(1 + tempval)] = "  // Bitmap " + str(i) + " (" + str(i % gridx) + ", " + str(i/gridx) + ")";
    for (int k = 0; k < tempval; k++) {
      String line = "  ";
      for (int j = 0; j < w; j++) {
        //println(idx(j + 1, k, w));
        line += "0x" + hex(chars[i][idx(j, k, w)], round(2*(wordLen/8))) + ", ";
      }
      lines[i*(1 + tempval) + 1 + k] = line;
    }
  }
  int i = 0;
  boolean done = false;
  while (!done) {
    int index = 0;
    if (lines[i].length() > 82) {
      for (int j = 81; j > -1; j--){
        if (lines[i].charAt(j) == ','){
          index = j;
          break;
        }
      }
      String newShorterLine = lines[i].substring(0, index + 1);
      String newLine = " " + lines[i].substring(index + 1, lines[i].length());
      lines[i] = newShorterLine;
      lines = splice(lines, newLine, i + 1);
    }
    i++;
    if (i == lines.length){
     done = true; 
    }
  }
  saveStrings(path, lines);
}

PImage displayBitmap(int bm[], int w, int h, int bg) {
  //background(bg*255);
  PImage canv = createImage(w, h, RGB);
  canv.loadPixels();
  stroke(255);
  strokeWeight(1);
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      if ((bm[idx(i, int(j/wordLen), w)] & (1 << (j & (wordLen - 1)))) > 0) {
        canv.pixels[idx(i, j, w)] = color((1 - bg)*255);
      } else {
        canv.pixels[idx(i, j, w)] = color((bg)*255);
      }
    }
  }
  canv.updatePixels();
  return canv;
}
