#include "../processing.h"

void setup()
{
  size(400, 400);

  beginShape(TRIANGLE_FAN);
  vertex(230, 200);
  vertex(230, 60);
  vertex(368, 200);
  vertex(230, 340);
  vertex(88, 200);
  vertex(230, 60);
  endShape();
}

void draw() {}
