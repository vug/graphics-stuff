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

  ctx.lineBresenham(150, 50, 180, 25, col);
  ctx.lineBresenham(150, 50, 180, 75, col);
  ctx.lineBresenham(150, 50, 120, 25, col);
  ctx.lineBresenham(150, 50, 120, 75, col);

  ctx.lineBresenham(150, 50, 175, 20, col);
  ctx.lineBresenham(150, 50, 175, 80, col);
  ctx.lineBresenham(150, 50, 125, 20, col);
  ctx.lineBresenham(150, 50, 125, 80, col);

  ctx.lineBresenham(150, 50, 190, 50, col);
  ctx.lineBresenham(150, 50, 110, 50, col);
  ctx.lineBresenham(150, 50, 150, 10, col);
  ctx.lineBresenham(150, 50, 150, 90, col);

  // Zingl functions
  ctx.zinglLine(250, 50, 300, 25, col);
  ctx.zinglCircle(300, 50, 25, col);
  ctx.zinglEllipseRect(350, 25, 450, 75, col);
  ctx.zinglQuadBezierSeg(50, 150, 65, 190, 100, 200, col);
  ctx.zinglQuadBezierSegAA(60, 150, 75, 190, 110, 200, col); // meh

  ctx.zinglLineAA(50, 250, 80, 225, col);
  ctx.zinglLineAA(50, 250, 80, 275, col);
  ctx.zinglLineAA(50, 250, 20, 225, col);
  ctx.zinglLineAA(50, 250, 20, 275, col);

  ctx.zinglLineAA(50, 250, 75, 220, col);
  ctx.zinglLineAA(50, 250, 75, 280, col);
  ctx.zinglLineAA(50, 250, 25, 220, col);
  ctx.zinglLineAA(50, 250, 25, 280, col);

  ctx.zinglLineAA(50, 250, 90, 250, col);
  ctx.zinglLineAA(50, 250, 10, 250, col);
  ctx.zinglLineAA(50, 250, 50, 210, col);
  ctx.zinglLineAA(50, 250, 50, 290, col);

  ctx.zinglLineWidthAA(150, 250, 180, 225, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 180, 275, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 120, 225, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 120, 275, 3.f, col);

  ctx.zinglLineWidthAA(150, 250, 175, 220, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 175, 280, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 125, 220, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 125, 280, 3.f, col);

  ctx.zinglLineWidthAA(150, 250, 190, 250, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 110, 250, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 150, 210, 3.f, col);
  ctx.zinglLineWidthAA(150, 250, 150, 290, 3.f, col);

  ctx.printPPM();

  return 0;
}