//
// Created by Novak on 05/04/2020.
//

#ifndef EAGLE_TIME_H
#define EAGLE_TIME_H

#include <eagle/CoreGlobalDefinitions.h>
#include <chrono>

namespace eagle {

class Time {
public:

    void start();
    void stop();
    void update();

    inline float time()          { return m_time; }
    inline float delta_time()    { return unscaled_delta_time() * m_timeScale; }
    inline float unscaled_delta_time() { return m_deltaTime; }
    inline float time_scale() const { return m_timeScale; }
    inline void set_time_scale(float timeScale) { m_timeScale = timeScale; }

private:
    float m_timeScale = 1, m_deltaTime = 0, m_time = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start, m_lastUpdate;
    bool m_started = false;
};

}

#endif //EAGLE_TIME_H
