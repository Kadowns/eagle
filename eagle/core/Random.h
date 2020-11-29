//
// Created by Novak on 05/04/2020.
//

#ifndef EAGLE_RANDOM_H
#define EAGLE_RANDOM_H

#include <eagle/core/CoreGlobalDefinitions.h>

EG_BEGIN

class Random {
public:
    static void init(uint32_t seed);
    static void init();
    static float range(float min, float max);
    static int range(int min, int max);
    static float value();
private:
    Random(uint32_t seed);
    float get_random_value();
private:
    static Random s_instance;
    std::mt19937 m_generator;
    std::uniform_real_distribution<float> m_distributor;
};

EG_END

#endif //EAGLE_RANDOM_H
