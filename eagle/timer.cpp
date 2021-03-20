//
// Created by Novak on 05/04/2020.
//

#include <eagle/timer.h>

namespace eagle {

void Timer::update() {
    if (!m_started){
        return;
    }
    auto now = std::chrono::high_resolution_clock::now();
    m_deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(now - m_lastUpdate).count();
    m_time = std::chrono::duration<float, std::chrono::seconds::period>(now - m_start).count();
    m_lastUpdate = now;
}

void Timer::start() {
    m_start = m_lastUpdate = std::chrono::high_resolution_clock::now();
    m_time = m_deltaTime = 0;
    m_started = true;
}

void Timer::stop() {
    m_started = false;
}

}