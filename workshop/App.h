#pragma once

#include <string>

namespace ws
{
  class App
  {
  public:
    struct Specs
    {
      std::string name;
      uint32_t width;
      uint32_t height;
    };

    ~App();

    void run();

    virtual Specs getSpecs() = 0;
    virtual void onInit() = 0;
    virtual void onRender() = 0;
    virtual void onDeinit() = 0;

    Specs specs;
  };
}