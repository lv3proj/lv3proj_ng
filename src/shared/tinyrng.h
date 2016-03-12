#pragma once

#include "pcg_basic.h"

class TinyRNG
{
public:
    TinyRNG() {}
    TinyRNG(uint64_t seed, uint64_t seq) { init(seed, seq); }
    inline void init(uint64_t seed, uint64_t seq) { pcg32_srandom_r(&_state, seed, seq); }
    inline unsigned operator() () { return pcg32_random_r(&_state); }
    // in [0 .. upper)
    inline unsigned operator()(unsigned upper) { return pcg32_boundedrand_r(&_state, upper); }
    // in [lower .. upper)
    inline unsigned operator()(unsigned lower, unsigned upper) { return lower + pcg32_boundedrand_r(&_state, upper - lower); }

private:
    pcg32_random_t _state;
};
