#pragma once
#include <memory>

#include "../Physics/Collision/Box.h"

class Camera;

class Player
{
public:

    Player();
    ~Player();


    void Update(double DeltaTime);
    
    [[nodiscard]] _forceinline std::shared_ptr<Camera> GetCamera() { return m_Camera; }

    [[nodiscard]] _forceinline Collision::Box GetBoundingBox() const { return m_BoxCollision; }


private:

    Collision::Box m_BoxCollision;
    
    std::shared_ptr<Camera> m_Camera;
    
};
