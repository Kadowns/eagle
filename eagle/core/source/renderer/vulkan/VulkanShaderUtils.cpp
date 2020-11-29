//
// Created by Novak on 23/03/2020.
//

#include <eagle/core/renderer/vulkan/VulkanShaderUtils.h>
#include <eagle/core/renderer/vulkan/spirv_reflect.h>
#include <eagle/core/renderer/vulkan/VulkanConverter.h>


EG_BEGIN

void VulkanShaderUtils::add_bindings_from_shader_stage(const std::vector<uint32_t> &code, VkShaderStageFlags stage,
                                                            std::map<uint32_t, std::map<uint32_t, DescriptorBindingDescription>> &descriptorSetMap,
                                                            std::vector<VkPushConstantRange> &pushConstantsRanges) {

    SpvReflectShaderModule shaderReflection;
    SPV_REFLECT_ASSERT(spvReflectCreateShaderModule(code.size() * sizeof(uint32_t), code.data(), &shaderReflection));
    uint32_t descriptorBindingCount = 0;
    SPV_REFLECT_ASSERT(spvReflectEnumerateDescriptorBindings(&shaderReflection, &descriptorBindingCount, nullptr));

    if (descriptorBindingCount > 0) {

        std::vector<SpvReflectDescriptorBinding*> reflectedDescriptorBindings(descriptorBindingCount);

        SPV_REFLECT_ASSERT(spvReflectEnumerateDescriptorBindings(&shaderReflection, &descriptorBindingCount, reflectedDescriptorBindings.data()));

        //Sort by binding so we can easily check if the binding has already been added by a previous shader stage
        std::sort(std::begin(reflectedDescriptorBindings), std::end(reflectedDescriptorBindings),
                  [](const SpvReflectDescriptorBinding* a, const SpvReflectDescriptorBinding* b)
                  {
                      return a->set < b->set || a->binding < b->binding;
                  });

        for (auto& reflectedBinding : reflectedDescriptorBindings){

            DescriptorBindingDescription description = {};

            description.name = std::string(reflectedBinding->name);
            description.binding = reflectedBinding->binding;
            description.shaderStage = VulkanConverter::to_eg(stage);
            description.descriptorType = VulkanConverter::to_eg((VkDescriptorType)reflectedBinding->descriptor_type);
            if (description.descriptorType == DescriptorType::UNIFORM_BUFFER){
                description.size = reflectedBinding->block.size;

                auto get_member_type = [&](const SpvReflectTypeDescription& type){

                    SpvReflectTypeFlags flags = type.type_flags;

                    if (SPV_REFLECT_TYPE_FLAG_BOOL == flags){
                        return DataType::BOOL;
                    }
                    if (SPV_REFLECT_TYPE_FLAG_INT == flags){
                        return DataType::INT;
                    }
                    if (SPV_REFLECT_TYPE_FLAG_FLOAT & flags){
                        if ((SPV_REFLECT_TYPE_FLAG_VECTOR | SPV_REFLECT_TYPE_FLAG_MATRIX) & flags){
                            switch (type.traits.numeric.matrix.column_count) {
                                case 4:
                                    return DataType::MATRIX4X4;
                                case 3:
                                    return DataType::MATRIX3X3;
                                case 2:
                                    return DataType::MATRIX2X2;

                            }
                        }
                        if ((SPV_REFLECT_TYPE_FLAG_VECTOR) & flags) {
                            switch (type.traits.numeric.vector.component_count) {
                                case 2:
                                    return DataType::VECTOR2F;
                                case 3:
                                    return DataType::VECTOR3F;
                                case 4:
                                    return DataType::VECTOR4F;
                            }
                        }
                        return DataType::FLOAT;
                    }


                    if (SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE == flags){
                        return DataType::EXTERNAL_IMAGE;
                    }
                    if (SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER & flags){
                        return DataType::EXTERNAL_SAMPLER;
                    }
                    if (SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE & flags){
                        return DataType::EXTERNAL_SAMPLED_IMAGE;
                    }
                    if (SPV_REFLECT_TYPE_FLAG_EXTERNAL_BLOCK & flags){
                        return DataType::EXTERNAL_BLOCK;
                    }
                    if (SPV_REFLECT_TYPE_FLAG_EXTERNAL_MASK & flags){
                        return DataType::EXTERNAL_MASK;
                    }
                    if (SPV_REFLECT_TYPE_FLAG_STRUCT & flags){
                        return DataType::STRUCT;
                    }
                    if (SPV_REFLECT_TYPE_FLAG_ARRAY & flags){
                        return DataType::ARRAY;
                    }
                    return DataType::UNDEFINED;
                };

                for (int i = 0; i < reflectedBinding->block.member_count; i++) {

                    DescriptorBindingMemberDescription member = {};
                    member.type = get_member_type(*reflectedBinding->block.members[i].type_description);
                    member.name = reflectedBinding->block.members[i].name;
                    member.size = reflectedBinding->block.members[i].size;
                    member.offset = reflectedBinding->block.members[i].offset;

                    description.members.emplace(member.name, member);
                }
            }




            auto existingSet = descriptorSetMap.find(reflectedBinding->set);
            if (existingSet != descriptorSetMap.end()){
                //If this set already exists append this binding to it
                auto existingBinding = existingSet->second.find(reflectedBinding->binding);
                if (existingBinding != existingSet->second.end()){
                    //TODO existingBinding->second.stageFlags |= stage;
                }
                else{ //Otherwise a new binding needs to be added
                    existingSet->second.emplace(description.binding, description);
                }
            }
            else { //Otherwise a new set needs to be added

                descriptorSetMap.emplace(reflectedBinding->set, std::map<uint32_t, DescriptorBindingDescription>());
                descriptorSetMap[reflectedBinding->set].emplace(description.binding, description);
            }
        }
    }



    uint32_t pushConstantsCount = 0;
    SPV_REFLECT_ASSERT(spvReflectEnumeratePushConstantBlocks(&shaderReflection, &pushConstantsCount, nullptr));

    if (pushConstantsCount > 0){

        std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantsCount);

        SPV_REFLECT_ASSERT(spvReflectEnumeratePushConstantBlocks(&shaderReflection, &pushConstantsCount, pushConstants.data()));

        uint32_t pushConstantsOffset = 0;
        for (auto& pcr : pushConstantsRanges){
            pushConstantsOffset += pcr.size;
        }

        for (uint32_t i = 0; i < pushConstants.size(); i++){
            VkPushConstantRange pushConstantRange = {};
            pushConstantRange.stageFlags = stage;
            pushConstantRange.size = pushConstants[i]->size - pushConstantsOffset; //for some reason, spv reflect counts size as real size + offset
            pushConstantRange.offset = pushConstantsOffset;
            pushConstantsOffset += pushConstantRange.size;
            pushConstantsRanges.emplace_back(pushConstantRange);
        }
    }
}

void
VulkanShaderUtils::enumerate_output_variables(const std::vector<uint32_t> &code, uint32_t &outputVariableCount) {

    SpvReflectShaderModule shaderReflection;
    SPV_REFLECT_ASSERT(spvReflectCreateShaderModule(code.size() * sizeof(uint32_t), code.data(), &shaderReflection));

    SPV_REFLECT_ASSERT(spvReflectEnumerateOutputVariables(&shaderReflection, &outputVariableCount, nullptr));
}

VkShaderModule VulkanShaderUtils::create_shader_module(VkDevice device, const std::vector<uint32_t> &code) {

    EG_CORE_TRACE("Creating shader module!");

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size() * sizeof(uint32_t);
    createInfo.pCode = code.data();

    VkShaderModule shaderModule;
    VK_CALL_ASSERT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule)) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}


EG_END
