/**
 * @file timestep.h
 * @brief
 * @date 11/19/2025
 */

#pragma once

#include "core/base.h"

namespace RF
{
    class Timestep
    {
    public:
        Timestep(float time = 0.0f)
            : mTime(time)
        {
        }

        operator float() const { return mTime; }

        f32 GetSeconds() const { return mTime; }
        f32 GetMilliseconds() const { return mTime * 1000.0f; }

    private:
        float mTime;
    };
} // namespace RF
