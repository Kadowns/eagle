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
    virtual bool is_dirty() const = 0;
    virtual void flush(uint32_t index) = 0;
};


class VulkanCleaner {
public:
    static void flush(uint32_t index);

    static void push(VulkanCleanable* object);

    static void clear();
private:
    static std::vector<VulkanCleanable*> m_dirtyObjects;
};

EG_END

#endif //EAGLE_VULKANCLEANER_H
