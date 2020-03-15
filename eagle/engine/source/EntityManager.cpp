//
// Created by Novak on 08/09/2019.
//

#include <eagle/engine/EntityManager.h>

EG_ENGINE_BEGIN

BaseComponent::Family BaseComponent::s_familyCounter = 0;

void Entity::destroy() {
    m_manager->destroy(*this);
}

Entity *EntityHandle::operator->() {
    return &m_manager->m_entities[m_index];
}

const Entity *EntityHandle::operator->() const {
    return &m_manager->m_entities[m_index];
}

Entity &EntityHandle::operator*() {
    return m_manager->m_entities[m_index];
}

const Entity &EntityHandle::operator*() const {
    return m_manager->m_entities[m_index];
}

Entity *EntityHandle::get() {
    return &m_manager->m_entities[m_index];
}

const Entity *EntityHandle::get() const {
    return &m_manager->m_entities[m_index];
}

EG_ENGINE_END