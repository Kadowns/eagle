//
// Created by Ricardo on 4/10/2022.
//

#include "vulkan_deleter.h"

namespace eagle {

namespace detail {

template<typename T>
void clear_frame_resources(std::vector<T>& resources) {
    std::vector<T> remainingResources;
    remainingResources.reserve(resources.size());
    for (auto& resource : resources){
        resource.framesRemaining--;
        if (resource.framesRemaining > 0){
            remainingResources.emplace_back(std::move(resource));
        }
    }
    std::swap(resources, remainingResources);
}

}

VulkanDeleter::VulkanDeleter(const VulkanDeleterCreateInfo &info) : m_info(info) {

}

VulkanDeleter::~VulkanDeleter() {
    clear_all();
}

void VulkanDeleter::destroy(VulkanDescriptorSetLayout* setLayout) {
    m_descriptorSetLayouts.emplace_back(Resource<VulkanDescriptorSetLayout>{std::unique_ptr<VulkanDescriptorSetLayout>(setLayout), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanDescriptorSet* set) {
    m_descriptorSets.emplace_back(Resource<VulkanDescriptorSet>{std::unique_ptr<VulkanDescriptorSet>(set), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanFramebuffer* framebuffer) {
    m_framebuffers.emplace_back(Resource<VulkanFramebuffer>{std::unique_ptr<VulkanFramebuffer>(framebuffer), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanFence* fence) {
    m_fences.emplace_back(Resource<VulkanFence>{std::unique_ptr<VulkanFence>(fence), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanSemaphore* semaphore) {
    m_semaphores.emplace_back(Resource<VulkanSemaphore>{std::unique_ptr<VulkanSemaphore>(semaphore), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanRenderPass* renderPass) {
    m_renderPasses.emplace_back(Resource<VulkanRenderPass>{std::unique_ptr<VulkanRenderPass>(renderPass), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanShader* shader) {
    m_shaders.emplace_back(Resource<VulkanShader>{std::unique_ptr<VulkanShader>(shader), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanImage *image) {
    m_images.emplace_back(Resource<VulkanImage>{std::unique_ptr<VulkanImage>(image), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanTexture *texture) {
    m_textures.emplace_back(Resource<VulkanTexture>{std::unique_ptr<VulkanTexture>(texture), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanVertexBuffer *buffer) {
    m_vertexBuffers.emplace_back(Resource<VulkanVertexBuffer>{std::unique_ptr<VulkanVertexBuffer>(buffer), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanIndexBuffer *buffer) {
    m_indexBuffers.emplace_back(Resource<VulkanIndexBuffer>{std::unique_ptr<VulkanIndexBuffer>(buffer), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanUniformBuffer *buffer) {
    m_uniformBuffers.emplace_back(Resource<VulkanUniformBuffer>{std::unique_ptr<VulkanUniformBuffer>(buffer), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanStorageBuffer *buffer) {
    m_storageBuffers.emplace_back(Resource<VulkanStorageBuffer>{std::unique_ptr<VulkanStorageBuffer>(buffer), m_info.frameCount});
}

void VulkanDeleter::destroy(VulkanCommandBuffer *buffer) {
    m_commandBuffers.emplace_back(Resource<VulkanCommandBuffer>{std::unique_ptr<VulkanCommandBuffer>(buffer), m_info.frameCount});
}

void VulkanDeleter::clear_frame() {
    //command buffers may reference lots of objects, so they are destroyed first
    detail::clear_frame_resources(m_commandBuffers);
    detail::clear_frame_resources(m_renderPasses);

    //a descriptor set must be destroyed prior to its descriptors
    detail::clear_frame_resources(m_descriptorSets);

    //descriptors
    detail::clear_frame_resources(m_framebuffers);
    detail::clear_frame_resources(m_textures);
    detail::clear_frame_resources(m_images);
    detail::clear_frame_resources(m_uniformBuffers);
    detail::clear_frame_resources(m_storageBuffers);

    //graphic pipelines
    detail::clear_frame_resources(m_descriptorSetLayouts);
    detail::clear_frame_resources(m_shaders);

    //these can probably go in any order
    detail::clear_frame_resources(m_indexBuffers);
    detail::clear_frame_resources(m_vertexBuffers);
    detail::clear_frame_resources(m_fences);
    detail::clear_frame_resources(m_semaphores);
}

void VulkanDeleter::clear_all() {
    //command buffers may reference lots of objects, so they are destroyed first
    m_commandBuffers.clear();
    m_renderPasses.clear();

    //a descriptor set must be destroyed prior to its descriptors
    m_descriptorSets.clear();

    //descriptors
    m_framebuffers.clear();
    m_textures.clear();
    m_images.clear();
    m_uniformBuffers.clear();
    m_storageBuffers.clear();

    //graphic pipelines
    m_descriptorSetLayouts.clear();
    m_shaders.clear();

    //these can probably go in any order
    m_indexBuffers.clear();
    m_vertexBuffers.clear();
    m_fences.clear();
    m_semaphores.clear();
}

}