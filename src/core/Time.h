/**
 * @file Time.h
 * @brief
 * @date 11/9/2025
 */
#pragma once

namespace RealmFortress
{
    /**
     * @class Time
     * @brief TODO: add brief description
     *
     * TODO: add detailed description
     */
    class Time
    {
    public:
        static void BeginFrame();
        static float Delta() { return s_Delta; }
        static double Now() { return s_CurrentTime; }

    private:
        static inline double s_LastTime = 0.0;
        static inline double s_CurrentTime = 0.0;
        static inline float s_Delta = 0.0f;
    };
} // namespace RealmFortress
