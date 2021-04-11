//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_VERTEXLAYOUT_H
#define EAGLE_VERTEXLAYOUT_H

#include <initializer_list>
#include <vector>

#include "renderer_global_definitions.h"

namespace eagle {

struct VertexInputBindingDescription {
    std::vector<Format> attributes;
    VertexInputRate inputRate;
    size_t stride() const;
};

class VertexLayout {
public:
    VertexLayout();
    ~VertexLayout();

    void add(uint32_t binding, Format attribute);
    void add(const VertexInputBindingDescription& binding);

    size_t stride() const;

    inline size_t attribute_count() const {
        size_t count = 0;
        for (auto& binding : m_bindings){
            count += binding.attributes.size();
        }
        return count;
    }

    inline size_t binding_count() const {
        return m_bindings.size();
    }

    inline const std::vector<VertexInputBindingDescription>& bindings() const {
        return m_bindings;
    }

    inline VertexInputBindingDescription binding(uint32_t index){
        return m_bindings[index];
    }

    inline VertexInputBindingDescription operator[](uint32_t index){
        return binding(index);
    }

    inline std::vector<VertexInputBindingDescription>::iterator begin() { return m_bindings.begin(); }
    inline std::vector<VertexInputBindingDescription>::iterator end() { return m_bindings.end(); }

    inline std::vector<VertexInputBindingDescription>::const_iterator begin() const { return m_bindings.cbegin(); }
    inline std::vector<VertexInputBindingDescription>::const_iterator end() const { return m_bindings.cend(); }

private:
    std::vector<VertexInputBindingDescription> m_bindings;
};

}

#endif //EAGLE_VERTEXLAYOUT_H
