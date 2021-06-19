//
// Created by Novak on 07/09/2020.
//

#ifndef EAGLE_RENDERPASS_H
#define EAGLE_RENDERPASS_H

#include <eagle/renderer/renderer_global_definitions.h>

#include <utility>

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

struct RenderPassCreateInfo {
    std::vector<RenderAttachmentDescription> colorAttachments;
    RenderAttachmentDescription depthAttachment;
};

class RenderPass {
public:

    RenderPass(RenderPassCreateInfo  renderPassCreateInfo) :
            m_createInfo(std::move(renderPassCreateInfo)) {}

    virtual ~RenderPass() = default;

    inline const std::vector<RenderAttachmentDescription>& color_attachments() const { return m_createInfo.colorAttachments; }
    inline const RenderAttachmentDescription& depth_attachment() const { return m_createInfo.depthAttachment; }

protected:
    RenderPassCreateInfo m_createInfo;
};

}


#endif //EAGLE_RENDERPASS_H
