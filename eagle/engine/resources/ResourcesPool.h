//
// Created by Novak on 10/08/2019.
//

#ifndef EAGLE_RESOURCEMANAGER_H
#define EAGLE_RESOURCEMANAGER_H

#include <eagle/engine/EngineCore.h>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/type_index.hpp>
#include <unordered_map>
#include <utility>

EG_ENGINE_BEGIN

using ResourceID = size_t;

class BaseResource {
public:
    typedef size_t Family;

    explicit BaseResource(const std::string& name) : m_name(name) {}

    virtual ~BaseResource() {}

    virtual std::string type_name() const = 0;

    const std::string& name() const { return m_name; }

protected:

    static Family s_familyCounter;
private:
    std::string m_name;
};

template<typename T>
class Resource : public BaseResource {
public:
    virtual ~Resource() {}

    explicit Resource(const std::string name) : BaseResource(name) {}

    static std::string static_type_name() {
        return boost::typeindex::type_id<T>().pretty_name();
    }

    virtual std::string type_name() const override {
        static std::string name = static_type_name();
        return name;
    }

    ResourceID id() const { return m_id; }

private:
    friend class ResourcesPool;

    ResourceID m_id;

    static BaseResource::Family family(){
        static BaseResource::Family family = s_familyCounter++;
        return family;
    }
};

template <typename R>
class ResourceHandle {
public:

    ResourceHandle(ResourceID id) : m_id(id) {}
    ResourceHandle(const Resource<R>& resource) : ResourceHandle(resource.id()) {}

    bool valid() const;
    operator bool() const;

    R *operator -> ();
    const R *operator -> () const;

    R &operator * ();
    const R &operator * () const;

    R *get();
    const R *get() const;


    bool operator == (const ResourceHandle<R> &other) const {
        return m_id == other.m_id;
    }

    bool operator != (const ResourceHandle<R> &other) const {
        return !(*this == other);
    }

    ResourceID& id() { return m_id; }
    const ResourceID& id() const { return m_id; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version){
        ar & m_id;
    }

    ResourceID m_id;
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

        resource->m_id = id;

        //invokes loading method with arguments, and stores the result in the correct map
        m_resources[family][id] = resource;
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

    static ResourcesPool* s_instance;

    std::string m_root;
    std::vector<boost::filesystem::path> m_paths;

    std::vector<std::unordered_map<ResourceID, BaseResource*>> m_resources;


};


template<typename R>
inline bool ResourceHandle<R>::valid() const {
    return ResourcesPool::instance().template has_resource<R>(m_id);
}

template<typename R>
inline ResourceHandle<R>::operator bool() const {
    return valid();
}

template<typename R>
inline R *ResourceHandle<R>::operator->() {
    return ResourcesPool::instance().template get_resource_ptr<R>(m_id);
}

template<typename R>
inline const R *ResourceHandle<R>::operator->() const {
    return ResourcesPool::instance().template get_resource_ptr<R>(m_id);
}

template<typename R>
inline R& ResourceHandle<R>::operator*() {
    return *ResourcesPool::instance().template get_resource_ptr<R>(m_id);
}

template<typename R>
inline const R& ResourceHandle<R>::operator*() const {
    return *ResourcesPool::instance().template get_resource_ptr<R>(m_id);
}

template<typename R>
inline R *ResourceHandle<R>::get() {
    return ResourcesPool::instance().template get_resource_ptr<R>(m_id);
}

template<typename R>
inline const R *ResourceHandle<R>::get() const {
    return ResourcesPool::instance().template get_resource_ptr<R>(m_id);
}

EG_ENGINE_END

#endif //EAGLE_RESOURCEMANAGER_H
