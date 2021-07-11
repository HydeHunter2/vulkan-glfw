#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <functional>
#include <algorithm>
#include <string>

template<class VkThing, class DeleterType>
class VkWrapperInterface {
 public:
  explicit VkWrapperInterface(DeleterType deleter) : _deleter(std::move(deleter)) {}
  DeleterType getDeleter() {
    return _deleter;
  }
  VkThing* get() {
    return &_thing;
  }
  VkThing& operator->() {
    return _thing;
  }
  VkThing operator*() {
    return _thing;
  }

 private:
  DeleterType _deleter;
  VkThing _thing;
};

template<class VkThing>
using VkWrapperDeleter = std::function<void(VkThing, const VkAllocationCallbacks*)>;

template<class VkThing>
class VkWrapper : public VkWrapperInterface<VkThing, VkWrapperDeleter<VkThing>> {
  using Interface = VkWrapperInterface<VkThing, VkWrapperDeleter<VkThing>>;
 public:
  explicit VkWrapper(VkWrapperDeleter<VkThing> deleter) : Interface(deleter) {}
  virtual ~VkWrapper() {
    Interface::getDeleter()(Interface::operator*(), nullptr);
  }
};

template<class VkThing, class VkParent>
using VkWrapperWithParentDeleter = std::function<void(VkParent, VkThing, const VkAllocationCallbacks*)>;

template<class VkThing, class VkParent>
class VkWrapperWithParent: public VkWrapperInterface<VkThing, VkWrapperWithParentDeleter<VkThing, VkParent>> {
  using Interface = VkWrapperInterface<VkThing, VkWrapperWithParentDeleter<VkThing, VkParent>>;
 public:
  VkWrapperWithParent(VkParent* parent, VkWrapperWithParentDeleter<VkThing, VkParent> deleter) : Interface(deleter), _parent(parent) {}
  virtual ~VkWrapperWithParent() {
    Interface::getDeleter()(*_parent, Interface::operator*(), nullptr);
  }
 private:
  VkParent* _parent;
};

template<class VkThing, class VkParent>
class VkWrapperVectorWithParent: public VkWrapperInterface<std::vector<VkThing>, VkWrapperWithParentDeleter<VkThing, VkParent>> {
  using Interface = VkWrapperInterface<std::vector<VkThing>, VkWrapperWithParentDeleter<VkThing, VkParent>>;
 public:
  VkWrapperVectorWithParent(VkParent* parent, VkWrapperWithParentDeleter<VkThing, VkParent> deleter) : Interface(deleter), _parent(parent) {}
  virtual ~VkWrapperVectorWithParent() {
    for (auto& thing : Interface::operator*()) {
      Interface::getDeleter()(*_parent, thing, nullptr);
    }
  }

 private:
  VkParent* _parent;
};
