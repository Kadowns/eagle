//
// Created by Novak on 08/09/2019.
//

#ifndef EAGLE_ENTITYMANAGER_H
#define EAGLE_ENTITYMANAGER_H


#include <eagle/engine/GlobalDefs.h>
#include <eagle/engine/Object.h>
#include <bitset>

EG_ENGINE_BEGIN


class EntityManager;

template<typename C>
class ComponentHandle;

struct BaseComponent {
public:
    typedef size_t Family;

protected:

    static Family s_familyCounter;
};

template <typename Derived>
struct Component : public BaseComponent {
private:
    friend class Entity;
    friend class EntityManager;
    static Family family(){
        static Family family = s_familyCounter++;
        return family;
    }
};

class Entity : public Object {
public:

    template<typename C, typename... Args>
    ComponentHandle<C> assign(Args&&... args){
        static_assert(std::is_base_of<Component<C>, C>(), "C must inherit from Eagle::Engine::Component");
        BaseComponent::Family family = Component<C>::family();
        assert(!m_componentMask.test(family) && "Component is already present on this entity!");

        m_components[family] = std::make_shared<C>(std::forward<Args>(args)...);
        m_componentMask.set(family);

        return *static_cast<C*>(m_components[family].get());
    }

    template<typename C>
    void remove(){
        static_assert(std::is_base_of<Component<C>, C>(), "C must inherit from Eagle::Engine::Component");
        BaseComponent::Family family = Component<C>::family();
        assert(m_componentMask.test(family) && "Component is not present on this entity!");

        m_components.erase(family);
        m_componentMask.reset(family);
    }

    template<typename C>
    bool has_component(){
        static_assert(std::is_base_of<Component<C>, C>(), "C must inherit from Eagle::Engine::Component");
        BaseComponent::Family family = Component<C>::family();
        return m_componentMask.test(family);
    }


    template<typename C>
    ComponentHandle<C> component(){
        assert(has_component<C>());
        return *static_cast<C*>(m_components[Component<C>::family()].get());
    }

    uint32_t index() const { return m_index; }

    virtual std::string type_name() const override {
        static std::string name = boost::typeindex::type_id<Entity>().pretty_name();
        return name;
    }

    void destroy();

    explicit Entity(EntityManager* manager, uint32_t index) :
            m_manager(manager), m_index(index) {
    }
private:

    friend class EntityManager;
    using ComponentMask = std::bitset<32>;

    ComponentMask m_componentMask;
    std::unordered_map<BaseComponent::Family, std::shared_ptr<BaseComponent>> m_components;
    EntityManager* m_manager;
    uint32_t m_index;

};


//wrapper around a entity, so no new entity is created unnecessarily
class EntityHandle {
public:
    EntityHandle(EntityManager* manager, uint32_t index) : m_manager(manager), m_index(index) {}

    Entity* operator->();

    const Entity* operator->() const;

    Entity& operator*();

    const Entity& operator*() const;

    Entity* get();

    const Entity* get() const;

private:
    EntityManager* m_manager;
    uint32_t m_index;

};

template<typename C>
class ComponentHandle {
public:
    ComponentHandle(C& component) : m_component(component) {}

    C* operator->(){
        return &m_component;
    }

    const C* operator->() const {
        return &m_component;
    }

    C& operator*(){
        return m_component;
    }

    const C& operator*() const{
        return m_component;
    }

    C* get(){
        return &m_component;
    }

    const C* get() const{
        return &m_component;
    }

private:

    C& m_component;

};




class EntityManager {
public:

    EntityManager() = default;
    //iterator--------------------------------------------------------------
    template<bool All = false>
    class EntityIterator : public std::iterator<std::input_iterator_tag, EntityHandle> {
    public:
        explicit EntityIterator(EntityManager* manager, Entity::ComponentMask mask, uint32_t index) :
                m_manager(manager), m_mask(mask), m_i(index), m_capacity(manager->size()) {
            next();
        }

        void operator++(){
            m_i++;
            next();
        }

        bool operator==(const EntityIterator& rhs) const { return m_i == rhs.m_i; }
        bool operator!=(const EntityIterator& rhs) const { return m_i != rhs.m_i; }
        EntityHandle operator*() { return EntityHandle(m_manager, m_manager->m_entities[m_i].index()); }
        const EntityHandle operator*() const { return EntityHandle(m_manager, m_manager->m_entities[m_i].index()); }

    private:
        void next() {
            while (m_i < m_capacity && !predicate()) {
                ++m_i;
            }
        }

        inline bool predicate() {
            return All || (m_manager->m_entities[m_i].m_componentMask & m_mask) == m_mask;
        }
    private:

        EntityManager* m_manager;
        Entity::ComponentMask m_mask;
        uint32_t m_i;
        size_t m_capacity;
    };
    //----------------------------------------------------------------------

    //view------------------------------------------------------------------
    template<bool All = false>
    class BaseEntityView {
    public:
        using Iterator = EntityIterator<All>;

    public:
        Iterator begin() { return Iterator(m_manager, m_mask, 0); }
        Iterator end() { return Iterator(m_manager, m_mask, uint32_t(m_manager->size())); }
        const Iterator begin() const { return Iterator(m_manager, m_mask, 0); }
        const Iterator end() const { return Iterator(m_manager, m_mask, m_manager->size()); }

    private:
        friend class EntityManager;

        explicit BaseEntityView(EntityManager *manager) : m_manager(manager) { m_mask.set(); }
        explicit BaseEntityView(EntityManager *manager, Entity::ComponentMask mask) :
                m_manager(manager), m_mask(mask) {}

        EntityManager *m_manager;
        Entity::ComponentMask m_mask;
    };

    using SelectedEntityView = BaseEntityView<false>;
    using CompleteEntityView = BaseEntityView<true>;

    //----------------------------------------------------------------------

    template<typename... Components>
    SelectedEntityView entities_with_components(){
        Entity::ComponentMask mask = component_mask<Components...>();
        return SelectedEntityView(this, mask);
    }

    CompleteEntityView entities(){
        return CompleteEntityView(this);
    }

    //guarantees same family id for const and non const
    template <typename C>
    static BaseComponent::Family component_family() {
        return Component<typename std::remove_const<C>::type>::family();
    }

    template <typename C>
    Entity::ComponentMask component_mask() {
        Entity::ComponentMask mask;
        mask.set(component_family<C>());
        return mask;
    }

    template <typename C1, typename C2, typename ... Components>
    Entity::ComponentMask component_mask() {
        return component_mask<C1>() | component_mask<C2, Components ...>();
    }

    template <typename C>
    Entity::ComponentMask component_mask(const ComponentHandle<C> &c) {
        return component_mask<C>();
    }

    template <typename C1, typename ... Components>
    Entity::ComponentMask component_mask(const ComponentHandle<C1> &c1, const ComponentHandle<Components> &... args) {
        return component_mask<C1, Components ...>();
    }

    EntityHandle create(){
        uint32_t index = m_entities.size();
        m_entities.emplace_back(Entity(this, index));
        return EntityHandle(this, index);
    }

    void destroy(Entity& e){
        m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), e));
    }

    size_t size() { return m_entities.size(); }

private:
    friend class EntityHandle;
    std::vector<Entity> m_entities;

};

EG_ENGINE_END

#endif //EAGLE_ENTITYMANAGER_H
