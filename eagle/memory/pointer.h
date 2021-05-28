//
// Created by Ricardo on 5/27/2021.
//

#ifndef EAGLE_POINTER_H
#define EAGLE_POINTER_H

#include <cstdint>
#include <cassert>

namespace eagle {

enum class PointerType {
    STRONG,
    WEAK
};

template<typename T>
class BasePointer {
protected:
    //defined in base class so weak and strong pointers will use a SharedObject of the exact same type
    struct SharedObject {
        T* data = nullptr;
        size_t strongReferences = 0;
        size_t weakReferences = 0;
    };
};

template<typename T, PointerType TYPE = PointerType::STRONG>
class Pointer : BasePointer<T> {
private:
    typedef typename BasePointer<T>::SharedObject SharedObject;
public:

    Pointer() = default;

    Pointer(T* ptr) {
        static_assert(TYPE == PointerType::STRONG, "Only strong pointers can be constructed with a raw pointer");
        m_sharedObject = new SharedObject();
        m_sharedObject->data = ptr;
        increase_strong_reference();
    }

    ~Pointer(){
        if (m_sharedObject){
            decrease_reference();
        }
    }

    Pointer(Pointer<T, PointerType::STRONG>& other){
        m_sharedObject = other.m_sharedObject;
        if (m_sharedObject){
            increase_reference();
        }
    }

    Pointer(Pointer<T, PointerType::WEAK>& other){
        m_sharedObject = other.m_sharedObject;
        if (m_sharedObject){
            increase_reference();
        }
    }

    Pointer(Pointer&& other){
        m_sharedObject = other.m_sharedObject;
        other.m_sharedObject = nullptr;
    }

    Pointer& operator=(const Pointer& rhs){
        if (this == &rhs) return *this;

        if (m_sharedObject){
            decrease_reference();
        }

        m_sharedObject = rhs.m_sharedObject;
        if (m_sharedObject){
            increase_reference();
        }
        return *this;
    }

    Pointer& operator=(Pointer&& rhs) noexcept {
        if (this == &rhs) return *this;

        if (m_sharedObject){
            decrease_reference();
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

    inline T& operator->() {
        return this->operator*();
    }

    inline T& operator->() const {
        return this->operator*();
    }

    inline operator bool() const {
        return m_sharedObject != nullptr && m_sharedObject->data != nullptr;
    }

    inline bool operator==(const Pointer<T>& rhs) const {
        return m_sharedObject == rhs.m_sharedObject;
    }

    inline bool operator!=(const Pointer<T>& rhs) const {
        return !this->operator==(rhs);
    }

private:

    inline void increase_reference(){
        if constexpr (TYPE == PointerType::STRONG){
            increase_strong_reference();
        }
        else {
            increase_weak_reference();
        }
    }

    inline void increase_strong_reference(){
        m_sharedObject->strongReferences++;
    }

    inline void increase_weak_reference(){
        m_sharedObject->weakReferences++;
    }

    inline void decrease_reference(){
        if constexpr (TYPE == PointerType::STRONG){
            decrease_strong_reference();
        }
        else {
            decrease_weak_reference();
        }
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

    inline void decrease_weak_reference(){
        assert(m_sharedObject->weakReferences > 0 && "Attempted to double destroy a pointer?");
        m_sharedObject->weakReferences--;
        if (m_sharedObject->weakReferences == 0 && m_sharedObject->strongReferences == 0){
            delete m_sharedObject;
            m_sharedObject = nullptr;
        }
    }
private:
    friend class Pointer<T, PointerType::STRONG>;
    friend class Pointer<T, PointerType::WEAK>;
    SharedObject* m_sharedObject = nullptr;
};

}

#endif //EAGLE_POINTER_H
