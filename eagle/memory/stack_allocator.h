//
// Created by Ricardo on 2/25/2021.
//

#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include <cstdint>
#include <cassert>

namespace eagle {

class StackAllocator {
public:
    explicit StackAllocator(size_t size) : m_size(size), m_block((uint8_t*) malloc(m_size)), m_marker(0) {}

    virtual ~StackAllocator() { free(m_block); }


    void* alloc(size_t size) {
        assert(m_marker + size < m_size && "Stack overflow");
        uintptr_t address = reinterpret_cast<uintptr_t>(m_block) + m_marker;
        m_marker += size;
        return reinterpret_cast<void*>(address);
    }

    void* alloc_aligned(size_t size, size_t alignment) {
        assert((alignment & (alignment - 1)) == 0 && "Requested alignment was not a power of 2");
        auto rawAddress = reinterpret_cast<uintptr_t>(alloc(size + alignment));
        return reinterpret_cast<void*>(next_aligned_address(rawAddress, alignment));
    }

    size_t marker() const { return m_marker; }

    void free_to_marker(size_t marker) {
        assert(marker >= 0);
        m_marker = marker;
    }

protected:

    inline static uintptr_t next_aligned_address(uintptr_t address, size_t alignment) {
        size_t mask = alignment - 1;
        uintptr_t misalignment = address & mask;
        ptrdiff_t adjustment = alignment - misalignment;
        return address + adjustment;
    }

protected:
    size_t m_size;
    uint8_t* m_block;
    size_t m_marker;
};


class TypedStackAllocator : private StackAllocator {
private:
    typedef void (* DestructorFunc)(void*, size_t);

public:

    class Scope {
    public:
        Scope(TypedStackAllocator& allocator) : m_allocator(allocator) {}

        ~Scope() {
            for (int i = 0; i < m_allocationCount; ++i) {
                m_allocator.pop();
            }
        }

        template<typename T, typename ...Args>
        T* construct(size_t quantity = 1, Args&& ...args) {
            m_allocationCount++;
            return m_allocator.construct<T, Args...>(quantity, std::forward<Args>(args)...);
        }

    private:
        TypedStackAllocator& m_allocator;
        size_t m_allocationCount = 0;
    };

    explicit TypedStackAllocator(size_t size) : StackAllocator(size) {}

    ~TypedStackAllocator() override = default;

    Scope scope() { return Scope(*this); }

    template<typename T, typename ...Args>
    T* construct(size_t quantity = 1, Args&& ...args) {
        assert(quantity >= 1);

        size_t currentMarker = marker();
        auto destructorAddress = (DestructorFunc*) alloc(sizeof(DestructorFunc));
        *destructorAddress = destruct<T>;

        auto quantityAddress = (size_t*) alloc(sizeof(size_t));
        *quantityAddress = quantity;

        auto alignmentAddress = (size_t*) alloc(sizeof(size_t));
        *alignmentAddress = alignof(T);

        T* obj = (T*) alloc_aligned(sizeof(T) * quantity, alignof(T));
        T* objIterator = obj;
        for (int i = 0; i < quantity; i++) {
            ::new(objIterator) T(std::forward(args)...);
            objIterator++;
        }
        auto lastMarkerAddress = (size_t*) alloc(sizeof(size_t));
        *lastMarkerAddress = currentMarker;
        return obj;
    }

    void pop() {
        auto lastMarkerAddress = reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(m_block) + m_marker - sizeof(size_t));
        auto destructorAddress = reinterpret_cast<DestructorFunc*>(reinterpret_cast<uintptr_t>(m_block) + *lastMarkerAddress);
        auto quantityAddress = reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(destructorAddress) + sizeof(DestructorFunc*));
        auto alignmentAddress = reinterpret_cast<size_t*>(reinterpret_cast<uintptr_t>(quantityAddress) + sizeof(size_t));
        auto objAddress = reinterpret_cast<void*>(next_aligned_address(reinterpret_cast<uintptr_t>(alignmentAddress) + sizeof(size_t), *alignmentAddress));
        (*destructorAddress)(objAddress, *quantityAddress);
        free_to_marker(*lastMarkerAddress);
    }

    size_t marker() const {
        return m_marker;
    }

private:

    template<typename T>
    static void destruct(void* obj, size_t quantity) {
        T* tobj = static_cast<T*>(obj);
        for (int i = 0; i < quantity; i++) {
            tobj->~T();
            tobj++;
        }
    }

};

}

#endif //STACK_ALLOCATOR_H
