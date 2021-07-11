#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <optional>
#include <fstream>
#include <vector>
#include <set>

#include "vk_wrapper.h"

struct Camera {
  glm::vec3 origin;
  float yaw;
  float pitch;
};

class Vulkan {
 public:
  Vulkan(GLFWwindow* window);

  void drawFrame();
  VkDevice* getDevice();

  void pushConstants(const Camera& camera);

 private:
  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete();
  };

  const int kMaxFramesInFlight = 2;

  const std::vector<const char*> kDeviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      "VK_KHR_portability_subset"
  };

  const std::vector<const char*> kValidationLayers = {
      "VK_LAYER_KHRONOS_validation"
  };

#ifdef NDEBUG
  const bool kEnableValidationLayers = false;
#else
  const bool kEnableValidationLayers = true;
#endif

  void initInstance();
  bool checkValidationLayerSupport();
  std::vector<const char*> getRequiredExtensions() const;
  void initPhysicalDevice();
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  int rateDeviceSuitability(VkPhysicalDevice device);
  void initLogicalDevice();
  void initSurface();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
  static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  void initSwapChain();
  void initImageViews();
  void initGraphicsPipeline();
  static std::vector<char> readFile(const std::string& filename);
  VkShaderModule createShaderModule(const std::vector<char>& code);
  void initRenderPass();
  void initFramebuffers();
  void initCommandPool();
  void initCommandBuffers();
  void initSyncObjects();

  GLFWwindow* _window;

  VkWrapper<VkInstance> _instance{vkDestroyInstance};
  VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
  VkWrapper<VkDevice> _device{vkDestroyDevice};
  VkQueue _graphicsQueue;
  VkQueue _presentQueue;
  VkWrapperWithParent<VkSurfaceKHR, VkInstance> _surface{_instance.get(), vkDestroySurfaceKHR};
  VkWrapperWithParent<VkSwapchainKHR, VkDevice> _swapChain{_device.get(), vkDestroySwapchainKHR};
  std::vector<VkImage> _swapChainImages;
  VkFormat _swapChainImageFormat;
  VkExtent2D _swapChainExtent;
  VkWrapperVectorWithParent<VkImageView, VkDevice> _swapChainImageViews{_device.get(), vkDestroyImageView};
  VkWrapperWithParent<VkRenderPass, VkDevice> _renderPass{_device.get(), vkDestroyRenderPass};
  VkWrapperWithParent<VkPipelineLayout, VkDevice> _pipelineLayout{_device.get(), vkDestroyPipelineLayout};
  VkWrapperWithParent<VkPipeline, VkDevice> _graphicsPipeline{_device.get(), vkDestroyPipeline};
  VkWrapperVectorWithParent<VkFramebuffer, VkDevice> _swapChainFramebuffers{_device.get(), vkDestroyFramebuffer};
  VkWrapperWithParent<VkCommandPool, VkDevice> _commandPool{_device.get(), vkDestroyCommandPool};
  std::vector<VkCommandBuffer> _commandBuffers;
  VkWrapperVectorWithParent<VkSemaphore, VkDevice> _imageAvailableSemaphores{_device.get(), vkDestroySemaphore};
  VkWrapperVectorWithParent<VkSemaphore, VkDevice> _renderFinishedSemaphores{_device.get(), vkDestroySemaphore};
  VkWrapperVectorWithParent<VkFence, VkDevice> _inFlightFences{_device.get(), vkDestroyFence};
  std::vector<VkFence> _imagesInFlight;
  size_t _currentFrame = 0;
  Camera _pushConstant;
};
