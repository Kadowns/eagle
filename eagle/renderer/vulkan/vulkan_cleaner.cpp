//
// Created by Novak on 26/10/2019.
//

#include <eagle/renderer/vulkan/vulkan_cleaner.h>
#include <algorithm>

namespace eagle {

std::vector<VulkanCleanable*> VulkanCleaner::m_dirtyObjects;
std::mutex VulkanCleaner::m_mutex;

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
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_dirtyObjects.begin(), m_dirtyObjects.end(), object);
    if (it == m_dirtyObjects.end()){
        m_dirtyObjects.emplace_back(object);
    }
}

void VulkanCleaner::clear() {
    m_dirtyObjects.clear();
}


}