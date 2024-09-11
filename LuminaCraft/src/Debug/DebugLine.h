#pragma once

#include <glm/glm.hpp>

struct DebugLine
{
    DebugLine() = default;
    
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 color;
    float remainingTime; // Time left for the line to exist

    DebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, float duration)
        : start(start), end(end), color(color), remainingTime(duration) {}
};
