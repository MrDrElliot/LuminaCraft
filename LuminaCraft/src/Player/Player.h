#pragma once
#include <memory>
#include <glm/glm.hpp>

class World;
class Camera;

class Player
{
public:


    Player(World* InWorld);
    ~Player();


    void Update(double DeltaTime);
    
    _forceinline std::shared_ptr<Camera> GetCamera() { return m_Camera; }
    
    _forceinline glm::vec3 GetPosition() const { return Position; }
    

private:

    glm::vec3 Position;
    std::shared_ptr<Camera> m_Camera;
    std::shared_ptr<World> m_World;
    
};
