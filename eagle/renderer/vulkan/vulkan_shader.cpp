#include <eagle/renderer/vulkan/vulkan_shader.h>
#include <eagle/renderer/vulkan/vulkan_helper.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_shader_utils.h>
#include <eagle/renderer/vulkan/vulkan_render_pass.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>
#include <eagle/file_system.h>

namespace eagle {

VulkanShader::VulkanShader(const ShaderCreateInfo &createInfo, const VulkanShaderCreateInfo &nativeCreateInfo) :
        Shader(createInfo),
        m_nativeCreateInfo(nativeCreateInfo),
        m_cleared(true) {

    m_listener.subscribe(*m_nativeCreateInfo.on_window_resized, [this](VkExtent2D extent){
        m_nativeCreateInfo.extent = extent;
        cleanup_pipeline();
        create_pipeline();
    });

    for(auto& stageDescription : m_createInfo.shaderStages){
        if (stageDescription.stage == ShaderStage::COMPUTE) {
            throw std::logic_error("compute shaders are not allowed on a graphics shader");
        }
    }

    create_pipeline_layout();
    create_pipeline();
}

VulkanShader::~VulkanShader() {
    cleanup_pipeline();
    vkDestroyPipelineLayout(m_nativeCreateInfo.device, m_pipelineLayout, nullptr);

    m_descriptorSetLayouts.clear();
}

void VulkanShader::create_pipeline_layout() {

    //vertex input-----------------------------
    m_inputAttributes.resize(m_createInfo.vertexLayout.attribute_count());
    m_inputBindings.resize(m_createInfo.vertexLayout.binding_count());

    uint32_t attributeIndex = 0;
    uint32_t bindingIndex = 0;
    for (auto& binding : m_createInfo.vertexLayout){
        uint32_t offset = 0;
        for (auto& attribute : binding.attributes){
            m_inputAttributes[attributeIndex].format = VulkanConverter::to_vk(attribute);
            m_inputAttributes[attributeIndex].binding = bindingIndex;
            m_inputAttributes[attributeIndex].location = attributeIndex;
            m_inputAttributes[attributeIndex].offset = offset;
            offset += format_size(attribute);
            attributeIndex++;
        }


        m_inputBindings[bindingIndex].binding = bindingIndex;
        m_inputBindings[bindingIndex].stride = binding.stride();
        m_inputBindings[bindingIndex].inputRate = VulkanConverter::to_vk(binding.inputRate);
        bindingIndex++;
    }

    //map for each descriptor set with respective descriptor bindings
    std::map<uint32_t, std::map<uint32_t, DescriptorBindingDescription>> descriptorSetMap;
    std::vector<VkPushConstantRange> pushConstantsRanges;

    for (auto& stageDescription : m_createInfo.shaderStages){
        VkShaderStageFlags stage = VulkanConverter::to_vk(stageDescription.stage);

        VulkanShaderUtils::add_bindings_from_shader_stage(stageDescription.spirVCode, stage, descriptorSetMap, pushConstantsRanges);
        //fragment shader output count
        if (stage == VK_SHADER_STAGE_FRAGMENT_BIT){
           VulkanShaderUtils::enumerate_output_variables(stageDescription.spirVCode, m_outputAttachmentCount);
        }
    }

    for (auto& descriptorSetLayout : descriptorSetMap){

        std::vector<DescriptorBindingDescription> descriptions;
        for(auto& binding : descriptorSetLayout.second){
            descriptions.emplace_back(binding.second);
        }

        m_descriptorSetLayouts.emplace_back(std::make_unique<VulkanDescriptorSetLayout>(
                DescriptorSetLayoutCreateInfo{descriptions},
                VulkanDescriptorSetLayoutCreateInfo{m_nativeCreateInfo.device}
                ));
    }

    std::vector<VkDescriptorSetLayout> layouts(m_descriptorSetLayouts.size());
    for (uint32_t i = 0; i < layouts.size(); i++){
        layouts[i] = m_descriptorSetLayouts[i]->native_layout();
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantsRanges.size());
    pipelineLayoutInfo.pPushConstantRanges = pushConstantsRanges.data();
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
    pipelineLayoutInfo.pSetLayouts = layouts.data();

    auto result = vkCreatePipelineLayout(m_nativeCreateInfo.device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);

    if (result != VK_SUCCESS) {
        throw VulkanException("failed to create pipeline layout!", result);
    }
}

void VulkanShader::create_pipeline() {
    EG_TRACE("eagle","Creating shader pipeline!");


    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(m_createInfo.shaderStages.size());

    for (auto& stageDescription : m_createInfo.shaderStages){

        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.stage = VulkanConverter::to_vk(stageDescription.stage);
        shaderStageCreateInfo.module = VulkanShaderUtils::create_shader_module(m_nativeCreateInfo.device, stageDescription.spirVCode);
        shaderStageCreateInfo.pName = stageDescription.entryPointName.c_str();

        shaderStages.push_back(shaderStageCreateInfo);
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (m_inputAttributes.empty()){
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    }
    else {
        vertexInputInfo.vertexBindingDescriptionCount = m_inputBindings.size();
        vertexInputInfo.pVertexBindingDescriptions = m_inputBindings.data();
        vertexInputInfo.vertexAttributeDescriptionCount = m_inputAttributes.size();
        vertexInputInfo.pVertexAttributeDescriptions = m_inputAttributes.data();
    }


    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VulkanConverter::to_vk(m_createInfo.primitiveTopology);
    inputAssembly.primitiveRestartEnable = VK_FALSE;


    VkExtent2D extent = {};
    extent.width = (uint32_t)((float)m_nativeCreateInfo.extent.width * m_createInfo.viewport.widthPercent);
    extent.height = (uint32_t)((float)m_nativeCreateInfo.extent.height * m_createInfo.viewport.heightPercent);
    VkViewport viewport = {};
    viewport.x = m_createInfo.viewport.x;
    viewport.y = m_createInfo.viewport.y;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {static_cast<int32_t>(m_createInfo.viewport.x), static_cast<int32_t>(m_createInfo.viewport.y)};
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VulkanConverter::to_vk(m_createInfo.cullMode);
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment(m_outputAttachmentCount);
    for (size_t i = 0; i < colorBlendAttachment.size(); i++){
        if (m_createInfo.blendEnable){
            colorBlendAttachment[i].blendEnable = VK_TRUE;
            colorBlendAttachment[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment[i].colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment[i].alphaBlendOp = VK_BLEND_OP_ADD;
        }
        else {
            colorBlendAttachment[i].blendEnable = VK_FALSE;
            colorBlendAttachment[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment[i].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment[i].colorBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment[i].alphaBlendOp = VK_BLEND_OP_ADD; // Optional
        }
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    if (m_createInfo.depthTesting){
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    }
    else{
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    }
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = colorBlendAttachment.size();
    colorBlending.pAttachments = colorBlendAttachment.data();
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStateEnables = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicState.pDynamicStates = dynamicStateEnables.data();

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    if (m_createInfo.dynamicStates) {
        pipelineInfo.pDynamicState = &dynamicState;
    }
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = ((VulkanRenderPass*)m_createInfo.renderPass)->native_render_pass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


    auto result = vkCreateGraphicsPipelines(m_nativeCreateInfo.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to create graphics pipeline", result);
    }

    for (auto& shaderStage : shaderStages){
        vkDestroyShaderModule(m_nativeCreateInfo.device, shaderStage.module, nullptr);
    }

    m_cleared = false;

    EG_TRACE("eagle","Shader pipeline created!");
}

void VulkanShader::cleanup_pipeline(){
    if (m_cleared){
        return;
    }
    vkDestroyPipeline(m_nativeCreateInfo.device, m_graphicsPipeline, nullptr);
    m_cleared = true;
}

VkPipeline VulkanShader::native_pipeline() {
    return m_graphicsPipeline;
}

VkPipelineLayout VulkanShader::native_pipeline_layout() {
    return m_pipelineLayout;
}

DescriptorSetLayout* VulkanShader::descriptor_set_layout(uint32_t index) const {
    return m_descriptorSetLayouts[index].get();
}

}
