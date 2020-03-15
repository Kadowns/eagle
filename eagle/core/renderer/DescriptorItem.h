//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_DESCRIPTORITEM_H
#define EAGLE_DESCRIPTORITEM_H

#include "RenderingCore.h"

EG_BEGIN

class DescriptorItem {
public:
    explicit DescriptorItem(DescriptorType type): m_type(type){}
    virtual ~DescriptorItem() {}

    inline DescriptorType type() const {return m_type;}

private:
    DescriptorType m_type;
};


EG_END

#endif //EAGLE_DESCRIPTORITEM_H
