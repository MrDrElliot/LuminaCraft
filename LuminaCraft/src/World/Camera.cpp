#include "Camera.h"
#include <algorithm>
#include "../Input/Input.h"
#include "../Events/KeyCodes.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera()
{
    Position = glm::vec3(0.0f, 50.0f, 0.0f);
    Velocity = glm::vec3(0.0f);

    Yaw = -90.0f;
    Pitch = 0.0f;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    
    UpdateCameraVectors();
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewProjectionMatrix()
{
    glm::mat4 view = GetViewMatrix();
    glm::mat4 projection = GetProjectionMatrix();
    
    return projection * view;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetProjectionMatrix()
{
    /* zNear and zFar are flipped to use reverse depth */
    return glm::perspective(glm::radians(FOV), AspectRatio, 0.01f, 10000.0f);
}

glm::vec3 Camera::GetCameraForwardVector() const
{
    return Front;
}

void Camera::Rotate(float YawOffset, float PitchOffset, float RollOffset, bool LockPitch)
{
    Yaw += YawOffset;

    if (LockPitch)
    {
        if (Pitch + PitchOffset> 89.0f)
        {
            Pitch = 89.0f;
        }
        else if (Pitch + PitchOffset < -89.0f)
        {
            Pitch = -89.0f;
        }
        else
        {
            Pitch += PitchOffset;
        }
    }
    else
    {
        Pitch += PitchOffset;
    }
    
    UpdateCameraVectors();
}

void Camera::Move(glm::vec3 Direction)
{
    Position += Right * Direction.x;
    Position += Up * Direction.y;
    Position += Front * Direction.z;

}

void Camera::Update(double DeltaTime)
{
    glm::vec2 MousePos = Input::GetMousePos();
    MousePos.y = -MousePos.y;
    if (bFirstMouse)
    {
        LastMousePos = MousePos;
        bFirstMouse = false;
    }

    glm::vec2 MouseDelta = MousePos - LastMousePos;
    LastMousePos = MousePos;

    
    Yaw += MouseDelta.x * 0.1f;
    Pitch = std::clamp(Pitch + MouseDelta.y * 0.1f, -89.9f, 89.9f);

    float velocity = 6.0f * DeltaTime;
    if(Input::IsKeyPressed(Key::LeftShift))
    {
        velocity *= 4;
    }
    if(Input::IsKeyPressed(Key::W))
    {
        Position += Front * velocity;
    }
    if(Input::IsKeyPressed(Key::S))
    {
        Position -= Front * velocity;
    }
    if(Input::IsKeyPressed(Key::A))
    {
        Position -= Right * velocity;
    }
    if(Input::IsKeyPressed(Key::D))
    {
        Position += Right * velocity;
    }
    UpdateCameraVectors();
    
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
