#include "application.h"

Application::Application() {
  initWindow();
  _vulkan = std::make_unique<Vulkan>(_window);
}

Application::~Application() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

using timer = std::chrono::high_resolution_clock;
uint64_t timeFrom(const timer::time_point& timePoint) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(timer::now() - timePoint).count();
}

void Application::run() {
  float speed = 0.1;
  float rotationSpeed = 1. / 250;
  int fps = 20;

  double mouseX, mouseY;
  glfwGetCursorPos(_window, &mouseX, &mouseY);

  auto lastRender = timer::now();
  while (!glfwWindowShouldClose(_window)) {
    glfwPollEvents();

    if (timeFrom(lastRender) > 1000. / fps) {
      int w = glfwGetKey(_window, GLFW_KEY_W);
      int a = glfwGetKey(_window, GLFW_KEY_A);
      int s = glfwGetKey(_window, GLFW_KEY_S);
      int d = glfwGetKey(_window, GLFW_KEY_D);

      // TODO: Refactor moving system
      float yaw = _camera.yaw;
      if (w == GLFW_PRESS) {
        _camera.origin += glm::vec3(sin(yaw), 0, cos(yaw)) * speed;
      }
      if (a == GLFW_PRESS) {
        _camera.origin += glm::vec3(sin(yaw - M_PI / 2), 0, cos(yaw - M_PI / 2)) * speed;
      }
      if (s == GLFW_PRESS) {
        _camera.origin += glm::vec3(sin(yaw + M_PI), 0, cos(yaw + M_PI)) * speed;
      }
      if (d == GLFW_PRESS) {
        _camera.origin += glm::vec3(sin(yaw + M_PI  / 2), 0, cos(yaw + M_PI / 2)) * speed;
      }

      // TODO: Refactor rotation system
      double xPos, yPos;
      glfwGetCursorPos(_window, &xPos, &yPos);
      _camera.yaw += std::clamp((xPos - mouseX) * rotationSpeed, -0.5, 0.5);
      _camera.pitch -= std::clamp((yPos - mouseY) * rotationSpeed, -0.5, 0.5);
      _camera.pitch = std::clamp(_camera.pitch, -0.6f, 0.6f);
      mouseX = xPos;
      mouseY = yPos;

      _vulkan->pushConstants(_camera);  // TODO: Refactor updating camera in shader
      _vulkan->drawFrame();
      lastRender = timer::now();
    }
  }

  vkDeviceWaitIdle(*_vulkan->getDevice());
}

void Application::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(_width, _height, "Vulkan", nullptr, nullptr);

  glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
