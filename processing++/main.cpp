#include <MiniFB.h>

#include <cmath>

int width = 800;
int height = 600;
uint32_t *buffer;

void resize(struct mfb_window *window, int w, int h)
{
  (void)window;
  width = w;
  height = h;
  buffer = static_cast<uint32_t *>(realloc(buffer, width * height * sizeof(uint32_t)));
}

int main()
{
  struct mfb_window *window = mfb_open_ex("MiniFB Test", width, height, WF_BORDERLESS || WF_RESIZABLE);
  mfb_set_resize_callback(window, resize);
  struct mfb_timer *timer = mfb_timer_create();
  buffer = new uint32_t[height * width * sizeof(uint32_t)];

  mfb_set_viewport(window, 0, 0, width, height);

  mfb_update_state state;
  do
  {
    double now = mfb_timer_now(timer);
    for (auto y = 0; y < height; ++y)
    {
      for (auto x = 0; x < width; ++x)
      {
        auto ix = y * width + x;
        auto r = x * 255.0f / width;
        auto g = y * 255.0f / height;
        auto b = 255.0f * (std::sin(now) * 0.5f + 0.5f);
        buffer[ix] = MFB_RGB(r, g, b);
      }
    }

    state = mfb_update_ex(window, buffer, width, height);
    if (state != STATE_OK)
    {
      window = 0x0;
      break;
    }
  } while (mfb_wait_sync(window));

  return 0;
}