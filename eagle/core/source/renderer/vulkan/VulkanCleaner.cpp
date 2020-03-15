//
// Created by Novak on 26/10/2019.
//

#include <eagle/core/renderer/vulkan/VulkanCleaner.h>
#include <algorithm>

EG_BEGIN

void VulkanCleaner::flush(uint32_t index){

    std::vector<VulkanCleanable*> dirtyObjects;
    for (auto cleanable : m_dirtyObjects){
        cleanable->flush(index);
        if (cleanable->is_dirty()){
            dirtyObjects.emplace_back(cleanable);
        }
    }
    std::swap(m_dirtyObjects, dirtyObjects);

}

void VulkanCleaner::push(VulkanCleanable* object){
    auto it = std::find(m_dirtyObjects.begin(), m_dirtyObjects.end(), object);
    if (it == m_dirtyObjects.end()){
        m_dirtyObjects.emplace_back(object);
    }
}


EG_END