//
// Created by Novak on 25/06/2019.
//

#ifndef EAGLE_IMAGE_H
#define EAGLE_IMAGE_H

#include "eagle/core/Core.h"
#include "DescriptorItem.h"
#include <memory>


_EAGLE_BEGIN

//base interfaces for vulkan attachments and sampler
struct ImageAttachment{};
struct ImageSampler{};


class Image : public DescriptorItem {
public:

    Image(uint32_t width, uint32_t height) : DescriptorItem(EG_DESCRIPTOR_TYPE::IMAGE_2D), m_width(width), m_height(height) {}
    virtual ~Image() = default;
    uint32_t get_width() const {return m_width;}
    uint32_t get_height() const {return m_height;}
    virtual std::weak_ptr<ImageAttachment> get_attachment() = 0;
    virtual std::weak_ptr<ImageSampler> get_sampler() = 0;

protected:
    uint32_t m_width, m_height;

};

_EAGLE_END

#endif //EAGLE_IMAGE_H
