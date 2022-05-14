#include "processing.h"

#include <cmath>

int width = 800;
int height = 600;
uint32_t *buffer;

void setup();
void draw();

// void resize(struct mfb_window *window, int w, int h)
// {
//   (void)window;
//   width = w;
//   height = h;
//   buffer = static_cast<uint32_t *>(realloc(buffer, width * height * sizeof(uint32_t)));
// }

int main()
{
  setup();
  // mfb_set_resize_callback(window, resize);
  // struct mfb_timer *timer = mfb_timer_create();

  do
  {
    draw();

    if (processing.updateWindow() != STATE_OK)
    {
      processing.window = 0x0;
      break;
    }
  } while (processing.waitSync());

  return 0;
}