#include <MiniFB.h>

struct Color
{
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
};

class Processing
{
public:
    struct mfb_window *window;
    int width = 200;
    int height = 200;
    uint32_t *buffer;

public:
    void size(int w, int h);
    bool waitSync();
    mfb_update_state updateWindow();
    void point(int x, int y, Color c);
};

extern Processing processing;

void size(int w, int h);
void point(int x, int y, Color c);