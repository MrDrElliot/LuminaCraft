#include "Player.h"

#include "imgui.h"
#include "../Logging/Log.h"
#include "../World/Block.h"
#include "../World/Camera.h"
#include "../World/World.h"

Player::Player(World* InWorld)
{
    Position = glm::vec3(0.0f, 100.0f, 0.0f);
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
    
}
