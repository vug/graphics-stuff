#include "../processing.h"

void setup()
{
  size(400, 400);

  beginShape(QUAD_STRIP);
  vertex(120, 80);
  vertex(120, 300);
  vertex(200, 80);
  vertex(200, 300);
  vertex(260, 80);
  vertex(260, 300);
  vertex(340, 80);
  vertex(340, 300);
  endShape();
}

void draw() {}
