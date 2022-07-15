#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char *argv[])
{
  int width, height, n_channels;
  unsigned char *img_data;
  img_data = stbi_load(argv[1], &width, &height, &n_channels, 4);
  printf("%d %d %d", width, height, n_channels);
}