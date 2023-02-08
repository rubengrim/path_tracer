#pragma once
#include "Eigen/Dense"
#include <vector>

class Camera
{
public:
	Camera() = default;
	Camera(Eigen::Vector3f position, Eigen::Vector3f forwardDirection, int viewportWidth, int viewportHeight);

	void Update(float timeStep);
	void Resize(int viewportWidth, int viewportHeight);
	void ResetForwardAndPosition();
	bool HasMoved() const { return m_HasMoved; }

	inline const std::vector<Eigen::Vector3f>& GetRayDirections() const { return m_RayDirections; }
	
	inline Eigen::Vector3f GetPosition() const { return m_Position; }
	inline Eigen::Vector3f GetForwardDirection() const { return m_ForwardDirection; }
	
	inline float GetPitch() const { return m_Pitch; }
	inline float GetYaw() const { return m_Yaw; }
	
	inline int GetViewportWidth() const { return m_ViewportWidth; }
	inline int GetViewportHeight() const { return m_ViewportHeight; }
	
	inline float& MouseSensitivity() { return m_MouseSensitivity; }
	inline float& MovementSpeed() { return m_MovementSpeed; }

private:
	void RecalculateViewMatrix();
	void RecalculateProjectionMatrix();
	void RecalculateRayDirections();

private:
	bool m_HasMoved = false;

	Eigen::Vector3f m_Position;
	Eigen::Vector3f m_ForwardDirection;
	Eigen::Vector3f m_RightDirection;

	float m_MouseSensitivity = 0.07f;
	float m_MovementSpeed = 1.0f;

	int m_ViewportWidth;
	int m_ViewportHeight;

	float m_FovY = 0.25f * 3.1415f;
	float m_NearClip = 0.3f;
	float m_FarClip = 50.0f;
		
	Eigen::Matrix4f m_ViewMatrix;
	Eigen::Matrix4f m_InverseViewMatrix;
	Eigen::Matrix4f m_ProjectionMatrix;
	Eigen::Matrix4f m_InverseProjectionMatrix;

	std::vector<Eigen::Vector3f> m_RayDirections;

	float m_Pitch = 0.0f;
	float m_Yaw = 0.0f;
};