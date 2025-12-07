/**
 * @file hex_constants.h
 * @brief
 * @date 12/2/2025
 */

#pragma once

#include "core/base.h"

namespace RealmFortress::Hex
{
    constexpr f32 MODEL_SIZE = 2.31f / 2.0f;
    constexpr f32 MODEL_DEPTH = 1.0f;

    constexpr f32 SQRT3 = 1.732050807568877f;

    constexpr f32 ELEVATION_STEP = 0.5f;
    constexpr f32 BASE_HEIGHT = 0.0f;

    constexpr i32 NUM_DIRECTIONS = 6;

    constexpr f32 CORNER_ANGLE_OFFSET = 30.0f;
    constexpr f32 CORNER_ANGLE_STEP = 60.0f;
}
