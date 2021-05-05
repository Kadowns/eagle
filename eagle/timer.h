//
// Created by Novak on 05/04/2020.
//

#ifndef EAGLE_TIME_H
#define EAGLE_TIME_H

#include <eagle/core_global_definitions.h>
#include <chrono>

namespace eagle {

class Timer {
public:

    void start();
    void stop();
    void update();

    inline float time() const{ return m_time; }
    inline int64_t time_nano() const { return m_timeNano; }
    inline float delta_time() const { return unscaled_delta_time() * m_timeScale; }
    inline float unscaled_delta_time() const { return m_deltaTime; }
    inline float time_scale() const { return m_timeScale; }
    inline void set_time_scale(float timeScale) { m_timeScale = timeScale; }

private:
    float m_timeScale = 1, m_deltaTime = 0, m_time = 0;
    int64_t m_timeNano = 0;
    std::chrono::time_point<std::chrono::steady_clock> m_start, m_lastUpdate;
    bool m_started = false;
};

}

#endif //EAGLE_TIME_H
