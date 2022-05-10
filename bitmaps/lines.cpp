#include "context.h"

#include <iostream>

int main()
{
  Context ctx{512, 512};
  const Color col{0, 20, 40};

  ctx.lineHorizontal(10, 10, 10, col);
  ctx.lineHorizontal(20, 30, 10, col);
  ctx.lineHorizontal(30, 20, 20, col);

  ctx.lineVertical(15, 15, 15, col);
  ctx.lineVertical(20, 30, 40, col);
  ctx.lineVertical(30, 20, 50, col);

  ctx.lineNaive(50, 50, 80, 25, col);
  ctx.lineNaive(50, 50, 80, 75, col);
  ctx.lineNaive(50, 50, 20, 25, col);
  ctx.lineNaive(50, 50, 20, 75, col);

  ctx.lineNaive(50, 50, 75, 20, col);
  ctx.lineNaive(50, 50, 75, 80, col);
  ctx.lineNaive(50, 50, 25, 20, col);
  ctx.lineNaive(50, 50, 25, 80, col);

  ctx.lineNaive(50, 50, 90, 50, col);
  ctx.lineNaive(50, 50, 10, 50, col);
  ctx.lineNaive(50, 50, 50, 10, col);
  ctx.lineNaive(50, 50, 50, 90, col);

  // ctx.lineBresenham(30, 50, 300, 300, {0, 20, 40});

  ctx.printPPM();

  return 0;
}