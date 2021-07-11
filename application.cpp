#include "application.h"

Application::Application() {
  initWindow();
  _vulkan = std::make_unique<Vulkan>(_window);
}

Application::~Application() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

void Application::run() {
  _vulkan->drawFrame();

  while (!glfwWindowShouldClose(_window)) {
    glfwPollEvents();
  }

  vkDeviceWaitIdle(*_vulkan->getDevice());
}

void Application::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(_width, _height, "Vulkan", nullptr, nullptr);
}
