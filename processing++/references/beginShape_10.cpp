#include "../processing.h"

void setup()
{
  size(400, 400);

  beginShape(QUADS);
  vertex(120, 80);
  vertex(120, 300);
  vertex(200, 300);
  vertex(200, 80);
  vertex(260, 80);
  vertex(260, 300);
  vertex(340, 300);
  vertex(340, 80);
  endShape();
}

void draw() {}
