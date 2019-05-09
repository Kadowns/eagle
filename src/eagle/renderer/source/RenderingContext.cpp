//
// Created by Novak on 08/05/2019.
//

#include "../RenderingContext.h"

_EAGLE_BEGIN

RenderingContext::RenderingContext(uint32_t width, uint32_t height) :
    m_width(width),
    m_height(height){

}

uint32_t RenderingContext::width() const {
    return m_width;
}

uint32_t RenderingContext::height() const {
    return m_height;
}

_EAGLE_END
