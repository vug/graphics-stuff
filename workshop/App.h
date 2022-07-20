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

    App(const Specs &specs);
    ~App();

    void run();
    const int &getWinPosX() const { return winPosX; }
    const int &getWinPosY() const { return winPosY; }

    virtual void onInit() = 0;
    virtual void onRender(float time, float deltaTime) = 0;
    virtual void onDeinit() = 0;

    Specs specs;
    uint32_t width{};
    uint32_t height{};

  private:
    int winPosX{};
    int winPosY{};
  };
}