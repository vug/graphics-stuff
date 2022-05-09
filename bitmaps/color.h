#pragma once

#include <cassert>

struct Color
{
    int r = 255;
    int g = 255;
    int b = 255;

    static Color interpolate(Color c1, Color c2, float mix)
    {
        assert(0.f <= mix && mix <= 1.f);
        Color c;
        c.r = static_cast<int>(c1.r * (1.f - mix) + c2.r * mix);
        c.b = static_cast<int>(c1.b * (1.f - mix) + c2.b * mix);
        c.g = static_cast<int>(c1.g * (1.f - mix) + c2.g * mix);
        return c;
    }
};