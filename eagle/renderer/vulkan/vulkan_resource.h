//
// Created by Ricardo on 24/08/2022.
//

#ifndef EAGLE_VULKAN_RESOURCE_H
#define EAGLE_VULKAN_RESOURCE_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>

#include <cstdint>

namespace eagle {

class VulkanFoo {
public:



};

class VulkanDeviceAllocator;

template<typename T>
class VulkanResource {
public:
    using Iterator = typename std::vector<T>::iterator;
    using ConstIterator = typename std::vector<T>::const_iterator;
public:
    VulkanResource(VulkanDeviceAllocator *allocator, std::vector<T>&& resources);

    VulkanResource(VulkanResource&& other) noexcept;
    VulkanResource& operator=(VulkanResource&& other) noexcept;

    //non copyable
    VulkanResource(const VulkanResource& other) = delete;
    VulkanResource& operator=(const VulkanResource& other) = delete;

    ~VulkanResource();

    void swap(VulkanResource& other);
    void reset();

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    T& handle(size_t index);
    const T& handle(size_t index) const;

    Iterator begin();
    Iterator end();

    ConstIterator cbegin() const;
    ConstIterator cend() const;

private:
    VulkanDeviceAllocator* m_allocator = nullptr;
    std::vector<T> m_resources;
};

class VulkanDeviceAllocator {
public:

    VulkanDeviceAllocator(VkDevice device, size_t frameCount);
    ~VulkanDeviceAllocator();

    void clean_frame(size_t frameIndex);

    VulkanResource<VkBuffer> create_buffer(const VkBufferCreateInfo &createInfo);

private:
    //only resources can access destroy functions
    template<typename T>
    friend class VulkanResource;

    void destroy(VkBuffer buffer, size_t frameIndex);

private:

    struct FrameResources {
        std::vector<VkBuffer> buffers;
    };

    VkDevice m_device;
    std::vector<FrameResources> m_frames;
};

template<typename T>
VulkanResource<T>::VulkanResource(VulkanDeviceAllocator *allocator, std::vector<T>&& resources) :
    m_allocator(allocator),
    m_resources(std::move(resources)) {

}

template<typename T>
VulkanResource<T>::VulkanResource(VulkanResource&& other) noexcept {
    swap(other);
    other.reset();
}

template<typename T>
VulkanResource<T>::~VulkanResource() {
    reset();
}

template<typename T>
VulkanResource<T> &VulkanResource<T>::operator=(VulkanResource&& other) noexcept {
    swap(other);
    other.reset();
    return *this;
}

template<typename T>
void VulkanResource<T>::swap(VulkanResource<T>& other) {
    std::swap(m_allocator, other.m_allocator);
    std::swap(m_resources, other.m_resources);
}

template<typename T>
void VulkanResource<T>::reset() {
    if (m_allocator) {
        for (auto i = 0; i < m_resources.size(); i++) {
            m_allocator->destroy(m_resources[i], i);
        }
    }
    m_allocator = nullptr;
    m_resources.clear();
}

template<typename T>
T& VulkanResource<T>::operator[](size_t index) {
    return m_resources[index];
}

template<typename T>
const T& VulkanResource<T>::operator[](size_t index) const {
    return m_resources[index];
}

template<typename T>
T& VulkanResource<T>::handle(size_t index) {
    return m_resources[index];
}

template<typename T>
const T& VulkanResource<T>::handle(size_t index) const {
    return m_resources[index];
}

template<typename T>
typename VulkanResource<T>::Iterator VulkanResource<T>::begin() {
    return m_resources.begin();
}

template<typename T>
typename VulkanResource<T>::Iterator VulkanResource<T>::end() {
    return m_resources.end();
}

template<typename T>
typename VulkanResource<T>::ConstIterator VulkanResource<T>::cbegin() const {
    return m_resources.cbegin();
}

template<typename T>
typename VulkanResource<T>::ConstIterator VulkanResource<T>::cend() const {
    return m_resources.cend();
}

}

#endif //EAGLE_VULKAN_RESOURCE_H
