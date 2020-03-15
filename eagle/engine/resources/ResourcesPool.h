//
// Created by Novak on 10/08/2019.
//

#ifndef EAGLE_RESOURCEMANAGER_H
#define EAGLE_RESOURCEMANAGER_H

#include <eagle/engine/GlobalDefs.h>
#include <eagle/engine/Object.h>
#include <unordered_map>
#include <utility>

#define EG_CREATE_HANDLE(type, obj) Eagle::Engine::ResourceHandle<type>(*dynamic_cast<Resource<type>*>(obj))

EG_ENGINE_BEGIN

using ResourceID = size_t;

class BaseResource : public Object{
public:
    typedef size_t Family;

    explicit BaseResource(const std::string& name) : Object(name) {}

    virtual ~BaseResource() {}

    virtual ResourceID resource_id() const = 0;

protected:

    static Family s_familyCounter;
private:

};

template<typename T>
class Resource : public BaseResource {
public:
    virtual ~Resource() {}

    explicit Resource(const std::string& name) : BaseResource(name) {}

    static std::string static_type_name() {
        return boost::typeindex::type_id<T>().pretty_name();
    }

    static BaseResource::Family family(){
        static BaseResource::Family family = s_familyCounter++;
        return family;
    }

    virtual std::string type_name() const override {
        static std::string name = static_type_name();
        return name;
    }

    virtual ResourceID resource_id() const override final {
        return m_resourceId;
    }

private:
    friend class ResourcesPool;

    //unique id for this resource (should remain the same after serializing)
    ResourceID m_resourceId;
};

template <typename R>
class ResourceHandle {
public:

    ResourceHandle(ResourceID id) : m_resourceId(id) {}
    ResourceHandle() : ResourceHandle(0) {}
    ResourceHandle(const Resource<R>& resource) : m_resourceId(resource.resource_id()) {}
    ResourceHandle(const BaseResource& resource) : m_resourceId(resource.resource_id()){}
    ResourceHandle(const Object& object) : m_resourceId(dynamic_cast<const BaseResource&>(object).resource_id()){}

    bool valid() const;
    operator bool() const;

    R *operator -> ();
    const R *operator -> () const;

    R &operator * ();
    const R &operator * () const;

    R *get();
    const R *get() const;


    bool operator == (const ResourceHandle<R> &other) const {
        return m_resourceId == other.m_resourceId;
    }

    bool operator != (const ResourceHandle<R> &other) const {
        return !(*this == other);
    }

    ResourceID& id() { return m_resourceId; }
    const ResourceID& id() const { return m_resourceId; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ar & m_resourceId;
    }

    ResourceID m_resourceId;
};

class ResourcesPool {
public:

    ~ResourcesPool();

    static ResourcesPool& instance() {
        static ResourcesPool* s_instance = new ResourcesPool();
        return *s_instance;
    }

    const std::vector<std::unordered_map<ResourceID, BaseResource*>>& resources() { return m_resources; }

    template<typename R>
    void store(R* resource, ResourceID id){
        static_assert(std::is_base_of<Resource<R>, R>());
        BaseResource::Family family = Resource<R>::family();

        //if its a new resource type, expand resources pools
        if (family >= m_resources.size()){
            m_resources.resize(family + 1);
        }else if (m_resources[family].find(id) != m_resources[family].end()){
            throw std::runtime_error("Id already present in map!");
        }

        resource->m_resourceId = id;

        //invokes loading method with arguments, and stores the result in the correct map
        m_resources[family][id] = resource;
    }

    template<typename R>
    size_t count(){
        static_assert(std::is_base_of<Resource<R>, R>());
        BaseResource::Family family = Resource<R>::family();
        return m_resources[family].size();
    }

private:

    template<typename R>
    friend class ResourceHandle;

    template<typename R>
    R* get_resource_ptr(ResourceID id) {
        static_assert(std::is_base_of<Resource<R>, R>());
        return static_cast<R*>(m_resources[Resource<R>::family()][id]);
    }

    template<typename R>
    const R* get_resource_ptr(ResourceID id) const {
        static_assert(std::is_base_of<Resource<R>, R>());
        return static_cast<const R*>(m_resources[Resource<R>::family()][id]);
    }
    
    template<typename R>
    bool has_resource(ResourceID id) const {
        BaseResource::Family family = Resource<R>::family();
        return family < m_resources.size() && m_resources[family].find(id) != m_resources[family].end();
    }

private:

    std::vector<std::unordered_map<ResourceID, BaseResource*>> m_resources;

};


template<typename R>
inline bool ResourceHandle<R>::valid() const {
    return ResourcesPool::instance().template has_resource<R>(m_resourceId);
}

template<typename R>
inline ResourceHandle<R>::operator bool() const {
    return valid();
}

template<typename R>
inline R *ResourceHandle<R>::operator->() {
    return ResourcesPool::instance().template get_resource_ptr<R>(m_resourceId);
}

template<typename R>
inline const R *ResourceHandle<R>::operator->() const {
    return ResourcesPool::instance().template get_resource_ptr<R>(m_resourceId);
}

template<typename R>
inline R& ResourceHandle<R>::operator*() {
    return *ResourcesPool::instance().template get_resource_ptr<R>(m_resourceId);
}

template<typename R>
inline const R& ResourceHandle<R>::operator*() const {
    return *ResourcesPool::instance().template get_resource_ptr<R>(m_resourceId);
}

template<typename R>
inline R *ResourceHandle<R>::get() {
    return ResourcesPool::instance().template get_resource_ptr<R>(m_resourceId);
}

template<typename R>
inline const R *ResourceHandle<R>::get() const {
    return ResourcesPool::instance().template get_resource_ptr<R>(m_resourceId);
}

EG_ENGINE_END

#endif //EAGLE_RESOURCEMANAGER_H
