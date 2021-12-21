//
// Created by Ricardo on 10/24/2020.
//

#include <eagle/renderer/vulkan/vulkan_render_pass.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>

namespace eagle {

//so we can keep array memory alive when returning from other functions
struct VkSubpassDescriptionHelper {
    std::vector<VkAttachmentReference> colorReferences;
    std::vector<VkAttachmentReference> inputReferences;
    std::optional<VkAttachmentReference> depthStencilReference;
};

static VkAttachmentReference create_attachment_reference(const AttachmentReference& reference){
    VkAttachmentReference vkReference = {};
    vkReference.attachment = reference.attachmentIndex;
    vkReference.layout = VulkanConverter::to_vk(reference.layout);
    return vkReference;
}

static VkSubpassDescriptionHelper create_subpass_description(const SubpassDescription& description){
    VkSubpassDescriptionHelper descriptionHelper = {};
    descriptionHelper.colorReferences.reserve(description.colorReferences.size());
    for (auto& colorReference : description.colorReferences){
        descriptionHelper.colorReferences.emplace_back(create_attachment_reference(colorReference));
    }

    descriptionHelper.inputReferences.reserve(description.inputReferences.size());
    for (auto& inputReference : description.inputReferences){
        descriptionHelper.inputReferences.emplace_back(create_attachment_reference(inputReference));
    }

    if (description.depthStencilReference.has_value()){
        descriptionHelper.depthStencilReference = create_attachment_reference(description.depthStencilReference.value());
    }

    return descriptionHelper;
}


VulkanRenderPass::VulkanRenderPass(
        RenderPassCreateInfo createInfo,
        const VulkanRenderPassCreateInfo& vulkanCreateInfo) :
        RenderPass(std::move(createInfo)),
        m_vulkanCreateInfo(vulkanCreateInfo) {
    EG_TRACE("eagle","Creating a VulkanRenderPass!");
    m_vkAttachments = std::move(VulkanConverter::to_vk_vector<VkAttachmentDescription>(m_createInfo.attachments));

    m_clearValues.resize(m_vkAttachments.size());
    for (int i = 0; i < m_clearValues.size(); i++){
        if (i == m_clearValues.size() - 1){
            m_clearValues[i].depthStencil = {1.0f, 0};
        }
        else {
            m_clearValues[i].color = {0.0f, 0.0f, 0.0f, 1.0f};
        }
    }
//    std::for_each(m_clearValues.begin(), m_clearValues.end(), [](VkClearValue& value){
//        value.color = {0.0f, 0.0f, 0.0f, 1.0f};
//    });
//    VkClearValue depthClearValues = {};
//    depthClearValues.depthStencil = {1.0f, 0};
//    m_clearValues.emplace_back(depthClearValues);

    create_native_render_pass();
    EG_TRACE("eagle","VulkanRenderPass created!");
}

void VulkanRenderPass::create_native_render_pass() {
    EG_TRACE("eagle","Creating a native render pass!");

    //we need to keep our helper vector alive, so all its internal attachment reference vectors are also kept alive
    std::vector<VkSubpassDescriptionHelper> subpassDescriptionHelper;
    std::vector<VkSubpassDescription> subpassDescriptions;
    subpassDescriptionHelper.reserve(m_createInfo.subpassDescriptions.size());
    for (auto& description : m_createInfo.subpassDescriptions){
        subpassDescriptionHelper.push_back(create_subpass_description(description));
        auto& descriptionHelper = subpassDescriptionHelper.back();

        VkSubpassDescription subpassDescription = {};
        if (!descriptionHelper.colorReferences.empty()){
            subpassDescription.pColorAttachments = descriptionHelper.colorReferences.data();
            subpassDescription.colorAttachmentCount = descriptionHelper.colorReferences.size();
        }

        if (!descriptionHelper.inputReferences.empty()){
            subpassDescription.pInputAttachments = descriptionHelper.inputReferences.data();
            subpassDescription.inputAttachmentCount = descriptionHelper.inputReferences.size();
        }

        if (descriptionHelper.depthStencilReference.has_value()){
            subpassDescription.pDepthStencilAttachment = &descriptionHelper.depthStencilReference.value();
        }

        subpassDescriptions.emplace_back(subpassDescription);
    }

    std::vector<VkSubpassDependency> dependencies(m_createInfo.subpassDependencies.size());
    for (size_t i = 0; i < dependencies.size(); i++){
        dependencies[i].srcSubpass = m_createInfo.subpassDependencies[i].srcSubpass;
        dependencies[i].dstSubpass = m_createInfo.subpassDependencies[i].dstSubpass;
        dependencies[i].srcStageMask = VulkanConverter::eg_flags_to_vk_flags<PipelineStageFlagsBits>(m_createInfo.subpassDependencies[i].srcStageMask);
        dependencies[i].dstStageMask = VulkanConverter::eg_flags_to_vk_flags<PipelineStageFlagsBits>(m_createInfo.subpassDependencies[i].dstStageMask);
        dependencies[i].srcAccessMask = VulkanConverter::eg_flags_to_vk_flags<AccessFlagBits>(m_createInfo.subpassDependencies[i].srcAccessMask);
        dependencies[i].dstAccessMask = VulkanConverter::eg_flags_to_vk_flags<AccessFlagBits>(m_createInfo.subpassDependencies[i].dstAccessMask);
        dependencies[i].dependencyFlags = VulkanConverter::eg_flags_to_vk_flags<DependencyFlagBits>(m_createInfo.subpassDependencies[i].dependencyFlags);
    }

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = m_vkAttachments.size();
    renderPassInfo.pAttachments = m_vkAttachments.data();
    renderPassInfo.subpassCount = subpassDescriptions.size();
    renderPassInfo.pSubpasses = subpassDescriptions.data();
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

    VK_CALL_ASSERT(vkCreateRenderPass(m_vulkanCreateInfo.device, &renderPassInfo, nullptr, &m_vkRenderPass)) {
        throw std::runtime_error("failed to create render pass!");
    }

    EG_TRACE("eagle","Native render pass created!");
}

VulkanRenderPass::~VulkanRenderPass() {
    EG_TRACE("eagle","Destroying a VulkanRenderPass!");
    cleanup();
    EG_TRACE("eagle","VulkanRenderPass destroyed!");
}

void VulkanRenderPass::cleanup() {
    EG_TRACE("eagle","Cleaning up a VulkanRenderPass!");
    VK_CALL vkDestroyRenderPass(m_vulkanCreateInfo.device, m_vkRenderPass, nullptr);
    EG_TRACE("eagle","VulkanRenderPass cleaned!");
}

}
