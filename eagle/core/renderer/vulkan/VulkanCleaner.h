//
// Created by Novak on 26/10/2019.
//

#ifndef EAGLE_VULKANCLEANER_H
#define EAGLE_VULKANCLEANER_H

#include "VulkanCore.h"

EG_BEGIN

class VulkanCleaner;

class VulkanCleanable {
public:
    VulkanCleanable(VulkanCleaner& cleaner) : m_cleaner(cleaner){}

    virtual bool is_dirty() const = 0;
    virtual void flush(uint32_t index) = 0;
protected:
    VulkanCleaner& m_cleaner;
};


class VulkanCleaner {
public:
    virtual void flush(uint32_t index);

    virtual void push(VulkanCleanable* object);
private:
    std::vector<VulkanCleanable*> m_dirtyObjects;
};

EG_END

#endif //EAGLE_VULKANCLEANER_H
