#include "processing.h"

void setup()
{
  size(400, 300);
}

void draw()
{
  // double now = mfb_timer_now(timer);
  background(192, 64, 0);
  auto h = height() / 2;
  auto w = width() / 2;
  for (auto y = 0; y < h; ++y)
  {
    for (auto x = 0; x < w; ++x)
    {
      auto r = static_cast<uint8_t>(x * 255.0f / w);
      auto g = static_cast<uint8_t>(y * 255.0f / h);
      auto b = static_cast<uint8_t>(255.0f);
      // auto b = 255.0f * (std::sin(now) * 0.5f + 0.5f);
      stroke(r, g, b);
      point(x, y);
    }
  }

  fill(255, 255, 255);
  stroke(0, 0, 0);

  rect(200, 150, 40, 30);
  noFill();
  rect(210, 160, 40, 30);
  fill(200, 210, 80);
  noStroke();
  rect(220, 170, 40, 30);
}