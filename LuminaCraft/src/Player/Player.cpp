#include "Player.h"
#include "../World/Camera.h"

Player::Player()
{
    m_Camera = std::make_shared<Camera>();
    
    // Initialize the player's bounding box (centered at the camera's initial position)
    m_BoxCollision.x = m_Camera->GetPosition().x;
    m_BoxCollision.y = m_Camera->GetPosition().y;
    m_BoxCollision.w = 1.0f;          // Player's width
    m_BoxCollision.h = 2.0f;          // Player's height
    m_BoxCollision.vx = 0.0f;         // Initial velocity
    m_BoxCollision.vy = 0.0f;         // Initial velocity
}

Player::~Player()
{
}

void Player::Update(double DeltaTime)
{
    m_Camera->Update(DeltaTime);
    
    // Update the player's bounding box based on the camera's current position
    m_BoxCollision.x = m_Camera->GetPosition().x - 1.0f / 2.0f;  // Center the box at the camera
    m_BoxCollision.y = m_Camera->GetPosition().y - 2.0f; // Adjust the Y position
    m_BoxCollision.vx = m_Camera->GetVelocity().x;  // Update the velocity based on camera movement
    m_BoxCollision.vy = m_Camera->GetVelocity().y;
}
