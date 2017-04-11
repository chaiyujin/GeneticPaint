/* -----------------------------
* File   : random.h
* Author : Yuki Chai
* Created: 2016.11.28
* Project: Yuki
*/
#pragma once
#ifndef __YUKI_UTILS_RANDOM_H__
#define __YUKI_UTILS_RANDOM_H__

#include <random>
using std::random_device;
using std::seed_seq;
using std::mt19937;
using std::uniform_real_distribution;

namespace Yuki {
    class Random {
    private:
        random_device   r;
        seed_seq        seed;
        mt19937         engine;
        uniform_real_distribution<> dist;
    public:
        Random() : r(), 
            seed({r(), r(), r(), r(), r()}),
            engine(seed),
            dist(0, 1) {}
        ~Random() {}

        double random() {
            return dist(engine);
        }

		// return a number in [0, scale)
        template <class T>
        T random(T scale) {
            return (T)(random() * scale);
        }
    };
}

#endif // !__YUKI_UTILS_RANDOM_H__
