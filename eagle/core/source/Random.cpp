//
// Created by Novak on 05/04/2020.
//

#include <eagle/core/Random.h>

EG_BEGIN

Random Random::s_instance(0);

void Random::init() {
    std::random_device device;
    init(device());
}

void Random::init(uint32_t seed) {
    s_instance = Random(seed);
}

Random::Random(uint32_t seed) : m_generator(seed), m_distributor(0.0, 1.0) {

}

float Random::range(float min, float max) {
    float r = max - min;
    return (value() * r) + min;
}

int Random::range(int min, int max) {
    int r = max - min;
    return static_cast<int>(value() * r) + min;
}

float Random::value() {
    return s_instance.get_random_value();
}

float Random::get_random_value() {
    return m_distributor(m_generator);
}


EG_END
