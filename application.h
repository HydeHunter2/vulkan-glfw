#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <optional>
#include <fstream>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <set>

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

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

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

  void initVulkan();
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

  std::unique_ptr<VkInstance, void (*)(VkInstance*)> _instance{
    new VkInstance,
    [](VkInstance* instance) {
      vkDestroyInstance(*instance, nullptr);
    }
  };
  VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
  std::unique_ptr<VkDevice, void (*)(VkDevice*)> _device{
      new VkDevice,
      [](VkDevice* device) {
        vkDestroyDevice(*device, nullptr);
      }
  };
  VkQueue _graphicsQueue;
  VkQueue _presentQueue;
  std::unique_ptr<VkSurfaceKHR, std::function<void(VkSurfaceKHR*)>> _surface{
    new VkSurfaceKHR,
    [this](VkSurfaceKHR* surface) {
      vkDestroySurfaceKHR(*_instance, *surface, nullptr);
    }
  };
  std::unique_ptr<VkSwapchainKHR, std::function<void(VkSwapchainKHR*)>> _swapChain{
      new VkSwapchainKHR,
      [this](VkSwapchainKHR* swapChain) {
        vkDestroySwapchainKHR(*_device, *swapChain, nullptr);
      }
  };
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::unique_ptr<std::vector<VkImageView>, std::function<void(std::vector<VkImageView>*)>> _swapChainImageViews{
      new std::vector<VkImageView>,
      [this](std::vector<VkImageView>* imageViews) {
        for (auto& imageView : *imageViews) {
          vkDestroyImageView(*_device, imageView, nullptr);
        }
      }
  };
  std::unique_ptr<VkRenderPass, std::function<void(VkRenderPass*)>> _renderPass{
      new VkRenderPass,
      [this](VkRenderPass* renderPass) {
        vkDestroyRenderPass(*_device, *renderPass, nullptr);
      }
  };
  std::unique_ptr<VkPipelineLayout, std::function<void(VkPipelineLayout*)>> _pipelineLayout{
      new VkPipelineLayout,
      [this](VkPipelineLayout* pipelineLayout) {
        vkDestroyPipelineLayout(*_device, *pipelineLayout, nullptr);
      }
  };
  std::unique_ptr<VkPipeline, std::function<void(VkPipeline*)>> _graphicsPipeline{
      new VkPipeline,
      [this](VkPipeline* graphicsPipeline) {
        vkDestroyPipeline(*_device, *graphicsPipeline, nullptr);
      }
  };
  std::unique_ptr<std::vector<VkFramebuffer>, std::function<void(std::vector<VkFramebuffer>*)>> _swapChainFramebuffers{
      new std::vector<VkFramebuffer>,
      [this](std::vector<VkFramebuffer>* framebuffers) {
        for (auto& framebuffer : *framebuffers) {
          vkDestroyFramebuffer(*_device, framebuffer, nullptr);
        }
      }
  };
};
