//
// Created by Novak on 05/04/2020.
//

#ifndef EAGLE_TIME_H
#define EAGLE_TIME_H

#include <eagle/core/CoreGlobalDefinitions.h>
#include <chrono>

EG_BEGIN

class Time {
public:

    inline static float time()          { return s_instance.m_time; }
    inline static float delta_time()    { return s_instance.m_deltaTime * s_instance.m_timeScale; }
    inline static float unscaled_delta_time() { return s_instance.m_deltaTime; }
    inline static float time_scale()    { return s_instance.m_timeScale; }

    inline static void set_time_scale(float timeScale) { s_instance.m_timeScale = timeScale; }
private:
    friend class Application;
    static void init();
    static void update();
private:
    static Time s_instance;
    float m_time = 0, m_deltaTime = 0, m_timeScale = 1;
};

EG_END

#endif //EAGLE_TIME_H
