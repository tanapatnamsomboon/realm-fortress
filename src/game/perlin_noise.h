/**
 * @file perlin_noise.h
 * @brief
 * @date 12/18/2025
 */

#pragma once

#include "core/base.h"
#include <cmath>
#include <random>
#include <algorithm>

namespace RealmFortress
{
    class PerlinNoise
    {
    public:
        PerlinNoise(u32 seed = 0)
        {
            std::mt19937 gen(seed);
            for (i32 i = 0; i < 256; ++i)
            {
                mPermutation[i] = i;
            }
            std::shuffle(mPermutation, mPermutation + 256, gen);
            for (i32 i = 0; i < 256; ++i)
            {
                mPermutation[256 + i] = mPermutation[i];
            }
        }

        f32 Noise(f32 x, f32 y) const
        {
            i32 X = static_cast<i32>(std::floor(x)) & 255;
            i32 Y = static_cast<i32>(std::floor(y)) & 255;

            x -= std::floor(x);
            y -= std::floor(y);

            f32 u = Fade(x);
            f32 v = Fade(y);

            i32 a = mPermutation[X] + Y;
            i32 aa = mPermutation[a];
            i32 ab = mPermutation[a + 1];
            i32 b = mPermutation[X + 1] + Y;
            i32 ba = mPermutation[b];
            i32 bb = mPermutation[b + 1];

            f32 res = Lerp(v,
                Lerp(u, Grad(mPermutation[aa], x, y), Grad(mPermutation[ba], x - 1, y)),
                Lerp(u, Grad(mPermutation[ab], x, y - 1), Grad(mPermutation[bb], x - 1, y - 1))
            );

            return (res + 1.0f) / 2.0f; // Normalize to [0, 1]
        }

        f32 OctaveNoise(f32 x, f32 y, i32 octaves, f32 persistence = 0.5f) const
        {
            f32 total = 0.0f;
            f32 frequency = 1.0f;
            f32 amplitude = 1.0f;
            f32 max_value = 0.0f;

            for (i32 i = 0; i < octaves; ++i)
            {
                total += Noise(x * frequency, y * frequency) * amplitude;
                max_value += amplitude;
                amplitude *= persistence;
                frequency *= 2.0f;
            }

            return total / max_value;
        }

    private:
        static f32 Fade(f32 t)
        {
            return t * t * t * (t * (t * 6 - 15) + 10);
        }

        static f32 Lerp(f32 t, f32 a, f32 b)
        {
            return a + t * (b - a);
        }

        static f32 Grad(i32 hash, f32 x, f32 y)
        {
            i32 h = hash & 15;
            f32 u = h < 8 ? x : y;
            f32 v = h < 4 ? y : (h == 12 || h == 14 ? x : 0);
            return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
        }

    private:
        i32 mPermutation[512];
    };
} // namespace RealmFortress
