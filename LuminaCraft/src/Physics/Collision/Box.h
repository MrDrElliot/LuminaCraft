#pragma once

namespace Collision
{

    
    // describes an axis-aligned rectangle with a velocity 
    struct Box 
    { 
        // position of top-left corner 
        float x, y; 

        // dimensions 
        float w, h; 

        // velocity 
        float vx, vy;

        
        bool IsOverlapping(const Box& other) const
        {
            return (x < other.x + other.w &&
               x + w > other.x &&
               y < other.y + other.h &&
               y + h > other.y);
        }
    };
    
    
    float SweptAABB(Box b1, Box b2, float& normalx, float& normaly);

    bool AABBCheck(Box b1, Box b2);
    Box GetSweptBroadphaseBox(Box b);
    
}
