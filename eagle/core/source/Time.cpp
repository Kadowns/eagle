//
// Created by Novak on 05/04/2020.
//

#include <eagle/core/Time.h>

EG_BEGIN

Time Time::s_instance;

void Time::init() {
    s_instance = Time();
}

void Time::update() {
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    s_instance.m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastFrameTime).count();
    s_instance.m_time += s_instance.m_deltaTime;
    lastFrameTime = currentTime;
}

EG_END