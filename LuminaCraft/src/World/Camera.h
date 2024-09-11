#pragma once

#include <glm/glm.hpp>

class Camera
{
public:

	Camera();
	~Camera();

	glm::mat4 GetViewProjectionMatrix();
	glm::mat4 GetViewMatrix();
	glm::vec3 GetPosition() const { return Position; }
	glm::vec3 GetVelocity() const { return Velocity; }
	glm::mat4 GetProjectionMatrix();
	glm::vec3 GetCameraForwardVector() const;
	
	void Rotate(float YawOffset, float PitchOffset, float RollOffset, bool LockPitch);
	void Move(glm::vec3 Direction);
    
	void Update(double DeltaTime);
	void UpdateCameraVectors();
	void SetAspectRatio(float Ratio) { AspectRatio = Ratio; }
	void SetFOV(float InFOV) { FOV = InFOV; }

private:
    
	glm::vec2 LastMousePos = glm::vec2(0.0f, 0.0f);
	bool bFirstMouse = true;
    
	glm::vec3 Velocity;
	glm::vec3 Position;

	float FOV = 50.0f;
	float Pitch = 0.0f;
	float Yaw = 0.0f;
	float AspectRatio = 16.0/8.0f;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
};
