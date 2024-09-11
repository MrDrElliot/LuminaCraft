#pragma once

#include <glm/glm.hpp>

struct Plane
{
    glm::vec3 normal;
    float distance;

    // Normalize the plane
    void normalize()
    {
        float length = glm::length(normal);
        normal /= length;
        distance /= length;
    }
};

class Frustum
{
public:
    Plane planes[6];

    // Extract the planes from the view-projection matrix
    void extractPlanes(const glm::mat4& viewProjectionMatrix)
    {
        // Left
        planes[0].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0];
        planes[0].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0];
        planes[0].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0];
        planes[0].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0];
        planes[0].normalize();

        // Right
        planes[1].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0];
        planes[1].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0];
        planes[1].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0];
        planes[1].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0];
        planes[1].normalize();

        // Bottom
        planes[2].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1];
        planes[2].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1];
        planes[2].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1];
        planes[2].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1];
        planes[2].normalize();

        // Top
        planes[3].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1];
        planes[3].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1];
        planes[3].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1];
        planes[3].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1];
        planes[3].normalize();

        // Near
        planes[4].normal.x = viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2];
        planes[4].normal.y = viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2];
        planes[4].normal.z = viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2];
        planes[4].distance = viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2];
        planes[4].normalize();

        // Far
        planes[5].normal.x = viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2];
        planes[5].normal.y = viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2];
        planes[5].normal.z = viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2];
        planes[5].distance = viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2];
        planes[5].normalize();
    }

    // Check if a point is inside the frustum
    bool pointInFrustum(const glm::vec3& point)
    {
        for (int i = 0; i < 6; i++)
        {
            if (glm::dot(planes[i].normal, point) + planes[i].distance < 0)
            {
                return false;  // Outside
            }
        }
        return true;  // Inside
    }

    // Check if a sphere is inside the frustum
    bool sphereInFrustum(const glm::vec3& center, float radius)
    {
        for (int i = 0; i < 6; i++)
        {
            if (glm::dot(planes[i].normal, center) + planes[i].distance < -radius)
            {
                return false;  // Outside
            }
        }
        return true;  // Inside
    }

    // Check if a box (AABB) is inside the frustum
    bool boxInFrustum(const glm::vec3& min, const glm::vec3& max)
    {
        for (int i = 0; i < 6; i++)
            {
            glm::vec3 positiveVertex = min;
            glm::vec3 negativeVertex = max;

            if (planes[i].normal.x >= 0)
            {
                positiveVertex.x = max.x;
                negativeVertex.x = min.x;
            }
            if (planes[i].normal.y >= 0)
            {
                positiveVertex.y = max.y;
                negativeVertex.y = min.y;
            }
            if (planes[i].normal.z >= 0)
            {
                positiveVertex.z = max.z;
                negativeVertex.z = min.z;
            }

            if (glm::dot(planes[i].normal, positiveVertex) + planes[i].distance < 0)
            {
                return false;  // Outside
            }
        }
        return true;  // Inside or intersecting
    }
};
