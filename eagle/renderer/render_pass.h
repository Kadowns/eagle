//
// Created by Novak on 07/09/2020.
//

#ifndef EAGLE_RENDERPASS_H
#define EAGLE_RENDERPASS_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <utility>
#include <optional>

namespace eagle {

struct RenderAttachmentDescription {
    Format format;
    AttachmentLoadOperator loadOp = AttachmentLoadOperator::DONT_CARE;
    AttachmentStoreOperator storeOp = AttachmentStoreOperator::STORE;
    AttachmentLoadOperator stencilLoadOp = AttachmentLoadOperator::DONT_CARE;
    AttachmentStoreOperator stencilStoreOp = AttachmentStoreOperator::DONT_CARE;
    ImageLayout initialLayout;
    ImageLayout finalLayout;
};

struct SubpassDependency {
    uint32_t srcSubpass;
    uint32_t dstSubpass;
    PipelineStageFlags srcStageMask;
    PipelineStageFlags dstStageMask;
    AccessFlags srcAccessMask;
    AccessFlags dstAccessMask;
    DependencyFlags dependencyFlags;
};

struct AttachmentReference {
    uint32_t attachmentIndex;
    ImageLayout layout;
};

struct SubpassDescription {
    std::vector<AttachmentReference> colorReferences;
    std::vector<AttachmentReference> inputReferences;
    std::optional<AttachmentReference> depthStencilReference;
};

struct RenderPassCreateInfo {
    std::vector<RenderAttachmentDescription> attachments;
    std::vector<SubpassDescription> subpassDescriptions;
    std::vector<SubpassDependency> subpassDependencies;

};

class RenderPass {
public:
    explicit RenderPass(RenderPassCreateInfo createInfo) : m_createInfo(std::move(createInfo)) {}
    virtual ~RenderPass() = default;
    inline const RenderPassCreateInfo& info() const { return m_createInfo; }
protected:
    RenderPassCreateInfo m_createInfo;
};

}


#endif //EAGLE_RENDERPASS_H
