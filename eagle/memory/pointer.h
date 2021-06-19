//
// Created by Ricardo on 5/27/2021.
//

#ifndef EAGLE_POINTER_H
#define EAGLE_POINTER_H

#include <cstdint>
#include <cassert>

namespace eagle {

template<typename T>
class BasePointer {
public:
    //defined in base class so weak and strong pointers will use a SharedObject of the exact same type
    struct SharedObject {
        T* data = nullptr;
        size_t strongReferences = 0;
        size_t weakReferences = 0;
    };
};

template<typename T>
class WeakPointer;

template<typename T>
class StrongPointer : BasePointer<T> {
private:
    typedef typename BasePointer<T>::SharedObject SharedObject;
public:

    StrongPointer() = default;

    StrongPointer(T* ptr) {
        m_sharedObject = new SharedObject();
        m_sharedObject->data = ptr;
        increase_strong_reference();
    }

    ~StrongPointer(){
        reset();
    }

    StrongPointer(const StrongPointer<T>& other){
        m_sharedObject = other.m_sharedObject;
        if (m_sharedObject){
            increase_strong_reference();
        }
    }

    StrongPointer(StrongPointer<T>&& other){
        m_sharedObject = other.m_sharedObject;
        other.m_sharedObject = nullptr;
    }

    template<typename T2>
    StrongPointer(StrongPointer<T2>&& other){
        m_sharedObject = (SharedObject*)other.m_sharedObject;
        other.m_sharedObject = nullptr;
    }

    template<typename T2>
    StrongPointer(const StrongPointer<T2>& other){
        m_sharedObject = (SharedObject*)other.m_sharedObject;
        if (m_sharedObject){
            increase_strong_reference();
        }
    }

    explicit StrongPointer(const WeakPointer<T>& other);

    T* get() {
        T* result = nullptr;
        if (m_sharedObject){
            result = m_sharedObject->data;
        }
        return result;
    }

    T* get() const {
        T* result = nullptr;
        if (m_sharedObject){
            result = m_sharedObject->data;
        }
        return result;
    }

    template<typename T2>
    StrongPointer<T2> cast() const {
        return construct_from_shared_object((typename BasePointer<T2>::SharedObject*)m_sharedObject);
    }

    void reset(){
        if (m_sharedObject){
            decrease_strong_reference();
        }
        m_sharedObject = nullptr;
    }

    void reset_all(T* data = nullptr) {
        if (!m_sharedObject) {
            m_sharedObject = new SharedObject();
            increase_strong_reference();
        }
        delete m_sharedObject->data;
        m_sharedObject->data = data;
    }

    StrongPointer& operator=(const StrongPointer& rhs){
        if (this == &rhs) return *this;

        if (m_sharedObject){
            decrease_strong_reference();
        }

        m_sharedObject = rhs.m_sharedObject;
        if (m_sharedObject){
            increase_strong_reference();
        }
        return *this;
    }

    StrongPointer& operator=(StrongPointer&& rhs) noexcept {
        if (this == &rhs) return *this;

        if (m_sharedObject){
            decrease_strong_reference();
        }

        m_sharedObject = rhs.m_sharedObject;
        rhs.m_sharedObject = nullptr;
        return *this;
    }

    inline T& operator*() {
        assert(m_sharedObject != nullptr && m_sharedObject->data != nullptr && "dereferencing a null Pointer");
        return *m_sharedObject->data;
    }

    inline T& operator*() const {
        assert(m_sharedObject != nullptr && m_sharedObject->data != nullptr && "dereferencing a null Pointer");
        return *m_sharedObject->data;
    }

    inline T* operator->() {
        return &this->operator*();
    }

    inline T* operator->() const {
        return &this->operator*();
    }

    inline operator bool() const {
        return m_sharedObject != nullptr && m_sharedObject->data != nullptr;
    }

    inline bool operator==(const StrongPointer<T>& rhs) const {
        return m_sharedObject == rhs.m_sharedObject;
    }

    inline bool operator!=(const StrongPointer<T>& rhs) const {
        return !this->operator==(rhs);
    }

private:

    StrongPointer(typename BasePointer<T>::SharedObject* sharedObject){
        m_sharedObject = sharedObject;
        if (m_sharedObject){
            increase_strong_reference();
        }
    }

    inline void increase_strong_reference(){
        m_sharedObject->strongReferences++;
    }

    inline void decrease_strong_reference(){
        m_sharedObject->strongReferences--;
        if (m_sharedObject->strongReferences == 0){
            delete m_sharedObject->data;
            m_sharedObject->data = nullptr;
            if (m_sharedObject->weakReferences == 0){
                delete m_sharedObject;
                m_sharedObject = nullptr;
            }
        }
    }

private:
    template<typename T2>
    friend class StrongPointer;

    template<typename T2>
    friend class WeakPointer;

    SharedObject* m_sharedObject = nullptr;
};

template<typename T>
class WeakPointer : BasePointer<T> {
private:
    typedef typename BasePointer<T>::SharedObject SharedObject;

public:

    constexpr WeakPointer() = default;

    template<typename T2>
    WeakPointer(const StrongPointer<T2>& other){
        m_sharedObject = (SharedObject*)other.m_sharedObject;
        if (m_sharedObject){
            increase_weak_reference();
        }
    }

    WeakPointer(const WeakPointer<T>& other){
        m_sharedObject = other.m_sharedObject;
        if (m_sharedObject){
            increase_weak_reference();
        }
    }

    template<typename T2>
    WeakPointer(const WeakPointer<T2>& other){
        m_sharedObject = (SharedObject*)other.m_sharedObject;
        if (m_sharedObject){
            increase_weak_reference();
        }
    }

    WeakPointer(WeakPointer<T>&& other) noexcept {
        m_sharedObject = other.m_sharedObject;
        other.m_sharedObject = nullptr;
    }

    template<typename T2>
    WeakPointer(WeakPointer<T2>&& other) noexcept {
        m_sharedObject = (SharedObject*)other.m_sharedObject;
        other.m_sharedObject = nullptr;
    }

    T* get() {
        T* result = nullptr;
        if (m_sharedObject){
            result = m_sharedObject->data;
        }
        return result;
    }

    T* get() const {
        T* result = nullptr;
        if (m_sharedObject){
            result = m_sharedObject->data;
        }
        return result;
    }

    void reset(){
        if (m_sharedObject){
            decrease_weak_reference();
        }
        m_sharedObject = nullptr;
    }

    template<typename T2>
    WeakPointer<T2> cast() const {
        return WeakPointer<T2>((typename BasePointer<T2>::SharedObject*)m_sharedObject);
    }

    WeakPointer<T>& operator=(const WeakPointer<T>& rhs){
        if (this == &rhs) return *this;

        if (m_sharedObject){
            decrease_weak_reference();
        }

        m_sharedObject = rhs.m_sharedObject;
        if (m_sharedObject){
            increase_weak_reference();
        }
        return *this;
    }

    WeakPointer<T>& operator=(WeakPointer<T>&& rhs) noexcept {
        if (this == &rhs) return *this;

        if (m_sharedObject){
            decrease_weak_reference();
        }

        m_sharedObject = rhs.m_sharedObject;
        rhs.m_sharedObject = nullptr;
        return *this;
    }

    inline T& operator*() {
        assert(m_sharedObject != nullptr && m_sharedObject->data != nullptr && "dereferencing a null Pointer");
        return *m_sharedObject->data;
    }

    inline T& operator*() const {
        assert(m_sharedObject != nullptr && m_sharedObject->data != nullptr && "dereferencing a null Pointer");
        return *m_sharedObject->data;
    }

    inline T* operator->() {
        return &this->operator*();
    }

    inline T* operator->() const {
        return &this->operator*();
    }

    inline operator bool() const {
        return m_sharedObject != nullptr && m_sharedObject->data != nullptr;
    }

    inline bool operator==(const WeakPointer& rhs) const {
        return m_sharedObject == rhs.m_sharedObject;
    }

    inline bool operator!=(const WeakPointer& rhs) const {
        return !this->operator==(rhs);
    }

private:

    WeakPointer(typename BasePointer<T>::SharedObject* sharedObject){
        m_sharedObject = sharedObject;
        if (m_sharedObject){
            increase_weak_reference();
        }
    }

    inline void decrease_weak_reference(){
        assert(m_sharedObject->weakReferences > 0 && "Attempted to double destroy a pointer?");
        m_sharedObject->weakReferences--;
        if (m_sharedObject->weakReferences == 0 && m_sharedObject->strongReferences == 0){
            delete m_sharedObject;
            m_sharedObject = nullptr;
        }
    }

    inline void increase_weak_reference(){
        m_sharedObject->weakReferences++;
    }

private:
    template<typename T2>
    friend class StrongPointer;

    template<typename T2>
    friend class WeakPointer;

    SharedObject* m_sharedObject = nullptr;
};

template<typename T>
StrongPointer<T>::StrongPointer(const WeakPointer<T>& other) {
    m_sharedObject = other.m_sharedObject;
    if (m_sharedObject){
        increase_strong_reference();
    }
}

template<typename T, typename ...Args>
StrongPointer<T> make_strong(Args&&... args){
    return StrongPointer<T>(new T(std::forward<Args>(args)...));
}

}

#endif //EAGLE_POINTER_H
