#include "Player.h"
#include "../Logging/Log.h"
#include "../World/Camera.h"
#include "../World/World.h"

Player::Player(World* InWorld)
{
    Position = glm::vec3(0.0f, 50.0f, 0.0f);
    m_World.reset(InWorld);
    m_Camera = std::make_shared<Camera>();
}

Player::~Player()
{
}

void Player::Update(double DeltaTime)
{
    m_Camera->Update(DeltaTime);
    Position = m_Camera->GetPosition();

    glm::vec3 Start = GetPosition();
    glm::vec3 End = GetPosition() + (m_Camera->GetCameraForwardVector() * 50.0f);

    glm::ivec3 Pos;
    
    GetWorld()->Raycast(Start, End, Pos, 1000.0f);
    
}
