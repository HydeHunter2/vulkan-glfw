#pragma once

#include <cstdint>
#include <memory>

#include "vulkan.h"

class Application {
 public:
  Application();
  ~Application();
  void run();

 private:
  void initWindow();

  uint32_t _width = 800;
  uint32_t _height = 600;
  GLFWwindow* _window;

  std::unique_ptr<Vulkan> _vulkan;
};
