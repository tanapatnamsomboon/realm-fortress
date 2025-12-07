/**
 * @file faction.h
 * @brief
 * @date 12/2/2025
 */

#pragma once

#include "core/base.h"
#include <glm/glm.hpp>
#include <string>

namespace RealmFortress
{
    enum class Faction
    {
        Player = 0, // blue
        Enemy1,     // red
        Enemy2,     // green
        Enemy3,     // yellow
        Neutral
    };

    inline const char* FactionToString(Faction faction)
    {
        switch (faction)
        {
        case Faction::Player: return "Player";
        case Faction::Enemy1: return "Enemy 1";
        case Faction::Enemy2: return "Enemy 2";
        case Faction::Enemy3: return "Enemy 3";
        case Faction::Neutral: return "Neutral";
        default: return "Unknown";
        }
    }

    inline const char* FactionToColorName(Faction faction)
    {
        switch (faction)
        {
        case Faction::Player: return "blue";
        case Faction::Enemy1: return "red";
        case Faction::Enemy2: return "green";
        case Faction::Enemy3: return "yellow";
        default: return "neutral";
        }
    }

    inline glm::vec3 FactionToColor(Faction faction)
    {
        switch (faction)
        {
        case Faction::Player: return glm::vec3(1.0f, 0.0f, 0.0f);
        case Faction::Enemy1: return glm::vec3(1.0f, 0.0f, 0.0f);
        case Faction::Enemy2: return glm::vec3(0.2f, 1.0f, 0.2f);
        case Faction::Enemy3: return glm::vec3(1.0f, 1.0f, 0.2f);
        default: return glm::vec3(0.7f, 0.7f, 0.7f);
        }
    }
}
