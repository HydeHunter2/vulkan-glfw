#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

class Application {
 public:
  Application();
  ~Application();
  void run();

 private:

  // GLFW

  void initWindow();

  std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> _window{nullptr, nullptr};
  uint32_t _width = 800;
  uint32_t _height = 600;

  // Vulkan

  const std::vector<const char*> kValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };

#ifdef NDEBUG
  const bool kEnableValidationLayers = false;
#else
  const bool kEnableValidationLayers = true;
#endif

  void initVulkan();
  void initInstance();
  bool checkValidationLayerSupport();
  std::vector<const char*> getRequiredExtensions() const;

  std::unique_ptr<VkInstance, void (*)(VkInstance*)> _instance{
    new VkInstance,
    [](VkInstance* instance) {
      vkDestroyInstance(*instance, nullptr);
    }
  };
};