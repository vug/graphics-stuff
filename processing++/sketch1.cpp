#include "processing.h"

void setup()
{
    size(800, 600);
}

void draw()
{
    int width = 800;
    int height = 600;
    // double now = mfb_timer_now(timer);
    for (auto y = 0; y < height; ++y)
    {
        for (auto x = 0; x < width; ++x)
        {
            auto r = static_cast<uint8_t>(x * 255.0f / width);
            auto g = static_cast<uint8_t>(y * 255.0f / height);
            auto b = static_cast<uint8_t>(255.0f);
            // auto b = 255.0f * (std::sin(now) * 0.5f + 0.5f);
            point(x, y, {r, g, b});
        }
    }
}