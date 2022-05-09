#include "context.h"

#include <iostream>

int main()
{
  Context ctx{512, 512};

  ctx.lineBresenham(30, 50, 300, 300, {240, 220, 170});

  ctx.printPPM();

  return 0;
}