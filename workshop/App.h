#pragma once

#include <string>

namespace ws
{
  class App
  {
  public:
    struct Specs
    {
      std::string name = "My Workshop App";
      uint32_t width = 800u;
      uint32_t height = 600u;
      bool shouldDebugOpenGL = true;
    };

    ~App();

    void run();

    virtual Specs getSpecs() = 0;
    virtual void onInit() = 0;
    virtual void onRender(float time, float deltaTime) = 0;
    virtual void onDeinit() = 0;

    Specs specs;
    uint32_t width{};
    uint32_t height{};
  };
}