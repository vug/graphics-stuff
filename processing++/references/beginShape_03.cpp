#include "../processing.h"

void setup()
{
  size(400, 400);

  beginShape();
  vertex(80, 80);
  vertex(160, 80);
  vertex(160, 160);
  vertex(240, 160);
  vertex(240, 240);
  vertex(80, 240);
  endShape(CLOSE);
}

void draw() {}
