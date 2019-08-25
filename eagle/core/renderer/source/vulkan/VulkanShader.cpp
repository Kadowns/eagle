#include "eagle/core/renderer/vulkan/spirv_reflect.h"
#include "eagle/core/renderer/vulkan/VulkanShader.h"
#include "eagle/core/renderer/vulkan/VulkanShaderCompiler.h"
#include "eagle/core/renderer/vulkan/VulkanHelper.h"
#include "eagle/core/Log.h"

#include <fstream>
#include <eagle/core/renderer/Shader.h>


EG_BEGIN

VulkanShader::VulkanShader(const std::string &vertFilePath, const std::string &fragFilePath,
                           const std::vector<Reference<VulkanDescriptorSetLayout>> &descriptorSetLayouts,
                           const ShaderPipelineInfo &pipelineInfo, const VulkanShaderCreateInfo &createInfo) :
    m_pipelineInfo(pipelineInfo),
    m_vertexLayout(pipelineInfo.vertexLayout),
    m_cleared(true),
    m_info(createInfo){

    m_vertShaderCode = VulkanShaderCompiler::compile_glsl(vertFilePath, EG_SHADER_STAGE::VERTEX);
    m_fragShaderCode = VulkanShaderCompiler::compile_glsl(fragFilePath, EG_SHADER_STAGE::FRAGMENT);

    create_pipeline_layout(descriptorSetLayouts);
    create_pipeline();
}

VulkanShader::~VulkanShader() {
    cleanup_pipeline();
    VK_CALL vkDestroyPipelineLayout(m_info.device, m_pipelineLayout, nullptr);
}

void VulkanShader::create_pipeline_layout(
        const std::vector<Reference<VulkanDescriptorSetLayout>> &descriptorSetLayouts) {

    SpvReflectShaderModule vertexShaderReflection, fragmentShaderReflection;
    SPV_REFLECT_ASSERT(spvReflectCreateShaderModule(m_vertShaderCode.size() * sizeof(unsigned int), m_vertShaderCode.data(), &vertexShaderReflection));
    SPV_REFLECT_ASSERT(spvReflectCreateShaderModule(m_fragShaderCode.size() * sizeof(unsigned int), m_fragShaderCode.data(), &fragmentShaderReflection));

    //vertex input-----------------------------
    m_inputAttributes.clear();
    m_inputAttributes.resize(m_vertexLayout.get_component_count());
    uint32_t offset = 0;
    for (uint32_t i = 0; i < m_inputAttributes.size(); i++){
        m_inputAttributes[i].format = VulkanHelper::get_vk_format(m_vertexLayout[i]);
        m_inputAttributes[i].binding = 0;
        m_inputAttributes[i].location = i;
        m_inputAttributes[i].offset = offset;
        offset += format_size(m_vertexLayout[i]);
    }

    m_inputBinding.binding = 0;
    m_inputBinding.stride = offset;
    m_inputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    //map for each descriptor set with respective descriptor bindings
    std::map<uint32_t, std::map<uint32_t, VkDescriptorSetLayoutBinding>> descriptorSetMap;
    std::vector<VkPushConstantRange> pushConstantsRanges;
    uint32_t pushConstantsOffset = 0;

    auto add_push_constants_from_shader_stage = [&](const SpvReflectShaderModule* shaderReflectionModule, const VkShaderStageFlags shaderStage) {

        uint32_t pushConstantsCount = 0;
        SPV_REFLECT_ASSERT(spvReflectEnumeratePushConstantBlocks(shaderReflectionModule, &pushConstantsCount, nullptr));

        if (pushConstantsCount > 0){

            std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantsCount);

            SPV_REFLECT_ASSERT(spvReflectEnumeratePushConstantBlocks(shaderReflectionModule, &pushConstantsCount, pushConstants.data()));

            for (uint32_t i = 0; i < pushConstants.size(); i++){
                VkPushConstantRange pushConstantRange = {};
                pushConstantRange.stageFlags = shaderStage;
                pushConstantRange.size = pushConstants[i]->size - pushConstantsOffset; //for some reason, spv reflect counts size as real size + offset
                pushConstantRange.offset = pushConstantsOffset;
                pushConstantsOffset += pushConstantRange.size;
                pushConstantsRanges.emplace_back(pushConstantRange);
            }
        }
    };

    add_push_constants_from_shader_stage(&vertexShaderReflection, VK_SHADER_STAGE_VERTEX_BIT);
    add_push_constants_from_shader_stage(&fragmentShaderReflection, VK_SHADER_STAGE_FRAGMENT_BIT);

    //fragment shader output count
    SPV_REFLECT_ASSERT(spvReflectEnumerateOutputVariables(&fragmentShaderReflection, &m_outputAttachmentCount, nullptr));

    std::vector<VkDescriptorSetLayout> layouts(descriptorSetLayouts.size());
    for (uint32_t i = 0; i < layouts.size(); i++){
        layouts[i] = descriptorSetLayouts[i]->get_native_layout();
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantsRanges.size());
    pipelineLayoutInfo.pPushConstantRanges = pushConstantsRanges.data();
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
    pipelineLayoutInfo.pSetLayouts = layouts.data();

    VK_CALL_ASSERT(vkCreatePipelineLayout(m_info.device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout)) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void VulkanShader::create_pipeline() {
    EG_CORE_TRACE("Creating shader pipeline!");

    VkShaderModule vertShaderModule = create_shader_module(m_vertShaderCode);
    VkShaderModule fragShaderModule = create_shader_module(m_fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};


    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &m_inputBinding;
    vertexInputInfo.vertexAttributeDescriptionCount = m_inputAttributes.size();
    vertexInputInfo.pVertexAttributeDescriptions = m_inputAttributes.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;


    VkExtent2D extent = {};
    extent.width = m_info.pExtent->width * m_pipelineInfo.viewport.widthPercent;
    extent.height = m_info.pExtent->height * m_pipelineInfo.viewport.heightPercent;
    VkViewport viewport = {};
    viewport.x = m_pipelineInfo.viewport.x;
    viewport.y = m_pipelineInfo.viewport.y;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {static_cast<int32_t>(m_pipelineInfo.viewport.x), static_cast<int32_t>(m_pipelineInfo.viewport.y)};
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
    rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachment(m_outputAttachmentCount);
    for (size_t i = 0; i < colorBlendAttachment.size(); i++){
        if (m_pipelineInfo.blendEnable){
            colorBlendAttachment[i].blendEnable = VK_TRUE;
            colorBlendAttachment[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment[i].colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment[i].alphaBlendOp = VK_BLEND_OP_ADD;
        }else{
            colorBlendAttachment[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment[i].blendEnable = VK_FALSE;
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
    if (m_pipelineInfo.depthTesting){
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
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
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    if (m_pipelineInfo.dynamicStates) {
        pipelineInfo.pDynamicState = &dynamicState;
    }
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = *m_info.pRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CALL_ASSERT(vkCreateGraphicsPipelines(m_info.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline)) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    VK_CALL vkDestroyShaderModule(m_info.device, fragShaderModule, nullptr);
    VK_CALL vkDestroyShaderModule(m_info.device, vertShaderModule, nullptr);

    m_cleared = false;

    EG_CORE_TRACE("Shader pipeline created!");
}

void VulkanShader::cleanup_pipeline(){
    if (m_cleared){ return; }
    VK_CALL vkDestroyPipeline(m_info.device, m_graphicsPipeline, nullptr);
    m_cleared = true;
}

VkShaderModule VulkanShader::create_shader_module(const std::vector<uint32_t> &code) {

    EG_CORE_TRACE("Creating shader module!");

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size() * sizeof(uint32_t);
    createInfo.pCode = code.data();

    VkShaderModule shaderModule;
    VK_CALL_ASSERT(vkCreateShaderModule(m_info.device, &createInfo, nullptr, &shaderModule)) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

VkPipeline& VulkanShader::get_pipeline() {
    return m_graphicsPipeline;
}

VkPipelineLayout& VulkanShader::get_layout() {
    return m_pipelineLayout;
}


EG_END
