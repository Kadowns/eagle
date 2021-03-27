//
// Created by Ricardo on 3/21/2021.
//

#ifndef EG_POOL_ALLOCATOR_H
#define EG_POOL_ALLOCATOR_H

#include <cassert>
#include <cstdint>
#include <vector>
#include <set>
#include <algorithm>

namespace eagle {

template<typename T>
class PoolAllocator;

//behaves like a std::unique_ptr
template<typename T>
class PoolPtr {
public:

    PoolPtr(PoolPtr<T>&& other)  noexcept : m_allocator(other.m_allocator), m_index(other.m_index), m_ptr(other.m_ptr) {
        other.m_ptr = nullptr;
    }

    ~PoolPtr();

    inline PoolPtr<T>& operator=(PoolPtr<T>&& other) noexcept {
        m_ptr = other.m_ptr;
        m_index = other.m_index;
        m_allocator = other.m_allocator;
        other.m_ptr = nullptr;
    }

    inline bool valid() const {
        return m_ptr != nullptr;
    }

    inline T* operator->(){
        assert(m_ptr && "Invalid PoolPtr");
        return m_ptr;
    }

    inline const T* operator->() const {
        assert(m_ptr && "Invalid PoolPtr");
        return m_ptr;
    }

    inline T* get() {
        assert(m_ptr && "Invalid PoolPtr");
        return m_ptr;
    }

    inline const T* get() const {
        assert(m_ptr && "Invalid PoolPtr");
        return m_ptr;
    }

private:
    friend PoolAllocator<T>;

    PoolPtr(PoolAllocator<T>& allocator, size_t index, T* ptr) : m_allocator(allocator), m_index(index), m_ptr(ptr) {}

    PoolAllocator<T>& m_allocator;
    size_t m_index;
    T* m_ptr;
};

template<typename T>
class PoolAllocator {
public:

    explicit PoolAllocator(size_t blockElementCount) :
            m_elementSize(sizeof(T)),
            m_chunkElementCount(blockElementCount) {
        m_chunks.emplace_back(new uint8_t[m_elementSize * blockElementCount]);
        m_freedElements.reserve(blockElementCount);
    }

    ~PoolAllocator(){
        for (auto& block : m_chunks) {
            delete[] block;
        }
    }

    inline size_t size() const {
        return m_marker - m_freedElements.size();
    }

    inline size_t capacity() const {
        return m_chunkElementCount * m_chunks.size();
    }

    inline size_t chunk_count() const {
        return m_chunks.size();
    }

    inline size_t chunk_size() const {
        return m_chunkElementCount * sizeof(T);
    }

    inline size_t chunk_element_count() const {
        return m_chunkElementCount;
    }

    inline size_t element_size() const {
        return sizeof(T);
    }

    template<typename ...Args>
    PoolPtr<T> construct(Args&& ...args) {
        if (!m_freedElements.empty()){
            size_t index = m_freedElements[0];
            m_freedElements.erase(m_freedElements.begin());
            return std::move(construct(index, std::forward<Args>(args)...));
        }

        size_t index = m_marker++;
        size_t blockIndex = block_index(index);
        if (blockIndex >= m_chunks.size()){
            m_chunks.emplace_back(new uint8_t[m_elementSize * m_chunkElementCount]);
            m_freedElements.reserve(m_freedElements.size() + m_chunkElementCount);
        }
        return std::move(construct(index, std::forward<Args>(args)...));
    }

    void destroy(PoolPtr<T>& ptr) {
        if (!ptr.m_ptr){
            return;
        }
        assert(std::find(m_freedElements.begin(), m_freedElements.end(), ptr.m_index) == m_freedElements.end() && "Called free with a PoolPtr that was already freed");
        ptr->~T();
        ptr.m_ptr = nullptr;
        m_freedElements.emplace_back(ptr.m_index);
    }

protected:

    template<typename ...Args>
    inline PoolPtr<T> construct(size_t index, Args&& ...args){
        void* ptr = ptr_for_index(index);
        ::new(ptr) T(std::forward<Args>(args)...);
        return std::move(PoolPtr<T>(*this, index, reinterpret_cast<T*>(ptr)));
    }

    inline size_t block_index(size_t index) const {
        return index / m_chunkElementCount;
    }

    inline size_t byte_index(size_t index) const {
        return (index % m_chunkElementCount) * m_elementSize;
    }

    inline void* ptr_for_index(size_t index){
        return ptr_for_index(block_index(index), byte_index(index));
    }

    inline void* ptr_for_index(size_t blockIndex, size_t byteIndex){
        return reinterpret_cast<void*>(m_chunks[blockIndex] + byteIndex);
    }

private:
    size_t m_elementSize;
    size_t m_chunkElementCount;
    size_t m_marker = 0;
    std::vector<uint8_t*> m_chunks;
    std::vector<size_t> m_freedElements;

};

template<typename T>
PoolPtr<T>::~PoolPtr() {
    m_allocator.destroy(*this);
}

}

#endif //TRIANGLE_POOL_ALLOCATOR_H
