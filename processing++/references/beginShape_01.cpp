#include "../processing.h"

void setup()
{
  size(400, 400);

  beginShape();
  vertex(120, 80);
  vertex(340, 80);
  vertex(340, 300);
  vertex(120, 300);
  endShape(CLOSE);
}

void draw() {}
