//
// Created by Ricardo on 4/10/2022.
//

#ifndef EAGLE_VULKAN_DELETER_H
#define EAGLE_VULKAN_DELETER_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_fence.h>
#include <eagle/renderer/vulkan/vulkan_semaphore.h>
#include <eagle/renderer/vulkan/vulkan_image.h>
#include <eagle/renderer/vulkan/vulkan_texture.h>
#include <eagle/renderer/vulkan/vulkan_render_pass.h>
#include <eagle/renderer/vulkan/vulkan_shader.h>
#include <eagle/renderer/vulkan/vulkan_framebuffer.h>
#include <eagle/renderer/vulkan/vulkan_vertex_buffer.h>
#include <eagle/renderer/vulkan/vulkan_index_buffer.h>
#include <eagle/renderer/vulkan/vulkan_uniform_buffer.h>
#include <eagle/renderer/vulkan/vulkan_storage_buffer.h>
#include <eagle/renderer/vulkan/vulkan_command_buffer.h>
#include <eagle/renderer/vulkan/vulkan_descriptor_set.h>
#include <eagle/renderer/vulkan/vulkan_descriptor_set_layout.h>

#include <memory>
#include <vector>

namespace eagle {

struct VulkanDeleterCreateInfo {
    VkDevice device;
    size_t frameCount;
};

class VulkanDeleter {
public:

    VulkanDeleter(const VulkanDeleterCreateInfo& info);
    ~VulkanDeleter();

    void destroy(VulkanDescriptorSetLayout* setLayout);
    void destroy(VulkanDescriptorSet* set);
    void destroy(VulkanFramebuffer* buffer);
    void destroy(VulkanFence* fence);
    void destroy(VulkanSemaphore* semaphore);
    void destroy(VulkanRenderPass* renderPass);
    void destroy(VulkanShader* renderPass);
    void destroy(VulkanImage* image);
    void destroy(VulkanTexture* texture);
    void destroy(VulkanVertexBuffer* buffer);
    void destroy(VulkanIndexBuffer* buffer);
    void destroy(VulkanUniformBuffer* buffer);
    void destroy(VulkanStorageBuffer* buffer);
    void destroy(VulkanCommandBuffer* buffer);

    void clear_frame();
    void clear_all();

private:
    template<typename T>
    struct Resource {
        using type = T;
        std::unique_ptr<T> ptr;
        size_t framesRemaining;
    };

    VulkanDeleterCreateInfo m_info;
    std::vector<Resource<VulkanImage>> m_images;
    std::vector<Resource<VulkanTexture>> m_textures;
    std::vector<Resource<VulkanVertexBuffer>> m_vertexBuffers;
    std::vector<Resource<VulkanIndexBuffer>> m_indexBuffers;
    std::vector<Resource<VulkanUniformBuffer>> m_uniformBuffers;
    std::vector<Resource<VulkanStorageBuffer>> m_storageBuffers;
    std::vector<Resource<VulkanCommandBuffer>> m_commandBuffers;
    std::vector<Resource<VulkanDescriptorSetLayout>> m_descriptorSetLayouts;
    std::vector<Resource<VulkanDescriptorSet>> m_descriptorSets;
    std::vector<Resource<VulkanFramebuffer>> m_framebuffers;
    std::vector<Resource<VulkanFence>> m_fences;
    std::vector<Resource<VulkanSemaphore>> m_semaphores;
    std::vector<Resource<VulkanRenderPass>> m_renderPasses;
    std::vector<Resource<VulkanShader>> m_shaders;

};

}

#endif //EAGLE_VULKAN_DELETER_H
