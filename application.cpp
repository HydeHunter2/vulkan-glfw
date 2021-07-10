#include "application.h"

Application::Application() {
  initWindow();
  initVulkan();
}

Application::~Application() {
  glfwTerminate();
}

void Application::run() {
  while (!glfwWindowShouldClose(_window.get())) {
    glfwPollEvents();
  }
}

void Application::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = {glfwCreateWindow(_width, _height, "Vulkan", nullptr, nullptr),
             glfwDestroyWindow};
}

void Application::initVulkan() {
  initInstance();
  pickPhysicalDevice();
}

void Application::initInstance() {
  if (kEnableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Application";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  auto extensions = getRequiredExtensions();
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();
  if (kEnableValidationLayers) {
    createInfo.enabledLayerCount = kValidationLayers.size();
    createInfo.ppEnabledLayerNames = kValidationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&createInfo, nullptr, _instance.get()) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}

bool Application::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const auto& layerName : kValidationLayers) {
    if (std::find_if(availableLayers.begin(), availableLayers.end(),
                     [layerName](VkLayerProperties& properties) {
                       return strcmp(layerName, properties.layerName) == 0;
                     }) == availableLayers.end()) {
      return false;
    }
  }

  return true;
}

std::vector<const char*> Application::getRequiredExtensions() const {
  uint32_t glfwExtensionCount = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (kEnableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void Application::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(*_instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(*_instance, &deviceCount, devices.data());

  int maxScore = 0;
  for (const auto& device : devices) {
    int score = rateDeviceSuitability(device);

    if (score > maxScore) {
      _physicalDevice = device;
      maxScore = score;
    }
  }

  if (maxScore == 0) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

Application::QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  for (int i = 0; i < queueFamilyCount; ++i) {
    if (queueFamilies.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      break;
    }
  }

  return indices;
}

int Application::rateDeviceSuitability(VkPhysicalDevice device) {  // TODO: Refactor rating system
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  QueueFamilyIndices indices = findQueueFamilies(device);

  if (!indices.graphicsFamily.has_value()) {
    return 0;
  }

  if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    return INT_MAX;
  }

  return deviceProperties.limits.maxImageDimension2D;
}

