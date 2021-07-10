#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
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

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
  };

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
  void pickPhysicalDevice();
  static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  static int rateDeviceSuitability(VkPhysicalDevice device);

  std::unique_ptr<VkInstance, void (*)(VkInstance*)> _instance{
    new VkInstance,
    [](VkInstance* instance) {
      vkDestroyInstance(*instance, nullptr);
    }
  };
  VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
};