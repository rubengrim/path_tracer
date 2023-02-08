#include "Camera.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Logger.h"

#include <iostream>

Camera::Camera(Eigen::Vector3f position, Eigen::Vector3f forwardDirection, int viewportWidth, int viewportHeight)
	: m_Position(position), m_ForwardDirection(forwardDirection), m_ViewportWidth(viewportWidth), m_ViewportHeight(viewportHeight)
{
	RecalculateViewMatrix();
	RecalculateProjectionMatrix();
	RecalculateRayDirections();
}

void Camera::Update(float timeStep)
{
	m_ForwardDirection = (m_InverseViewMatrix * Eigen::Vector4f(0.0f, 0.0f, -1.0f, 0.0f)).head(3).normalized();
	m_RightDirection = Eigen::Vector3f::UnitY().cross(m_ForwardDirection).normalized();

	if (Mouse::ButtonPressed(MouseButton::Right))
	{
		Mouse::HideCursor();
		
		m_HasMoved = false;

		if (Mouse::GetDeltaX() != 0.0f || Mouse::GetDeltaY() != 0.0f)
		{
			m_HasMoved = true;
		
			m_Pitch -= Mouse::GetDeltaY() * m_MouseSensitivity * timeStep;
			m_Pitch = std::clamp(m_Pitch, -1.5708f, 1.5708f); // Clamp pitch between -90 and 90 deg.
			m_Yaw -= Mouse::GetDeltaX() * m_MouseSensitivity * timeStep;
		}

		Eigen::Vector3f prevPosition = m_Position;

		float movementFactor = m_MovementSpeed * timeStep;

		if (Keyboard::KeyPressed(Key::W)) // Forward
			m_Position += m_ForwardDirection * movementFactor;

		if (Keyboard::KeyPressed(Key::S)) // Backward
			m_Position -= m_ForwardDirection * movementFactor;

		if (Keyboard::KeyPressed(Key::D)) // Right
			m_Position -= m_RightDirection * movementFactor;

		if (Keyboard::KeyPressed(Key::A)) // Left
			m_Position += m_RightDirection * movementFactor;

		if (Keyboard::KeyPressed(Key::E)) // Up
			m_Position += Eigen::Vector3f::UnitY() * movementFactor;

		if (Keyboard::KeyPressed(Key::Q)) // Down
			m_Position -= Eigen::Vector3f::UnitY() * movementFactor;

		if (prevPosition != m_Position)
		{
			m_HasMoved = true;
		}

		if (m_HasMoved)
		{
			RecalculateViewMatrix();
			RecalculateRayDirections();
		}
	}
	else
	{
		Mouse::ShowCursor();
		m_HasMoved = false;
	}
}

void Camera::ResetForwardAndPosition()
{
	m_Pitch = 0.0f;
	m_Yaw = 0.0f;
	m_Position = Eigen::Vector3f::Zero();

	RecalculateViewMatrix();
	RecalculateRayDirections();
}

void Camera::Resize(int viewportWidth, int viewportHeight)
{
	m_ViewportWidth = viewportWidth;
	m_ViewportHeight = viewportHeight;

	RecalculateProjectionMatrix();
	RecalculateRayDirections();
}

void Camera::RecalculateViewMatrix()
{
	Eigen::AngleAxisf yawAngle(m_Yaw, Eigen::Vector3f::UnitY());
	Eigen::AngleAxisf pitchAngle(m_Pitch, Eigen::Vector3f::UnitX());
		
	Eigen::Quaternion<float> q = yawAngle * pitchAngle;

	Eigen::Matrix3f rotationMatrix = q.matrix();

	Eigen::Matrix4f transformMatrix;
	transformMatrix.setIdentity();
	transformMatrix.block<3, 3>(0, 0) = rotationMatrix;
	transformMatrix.block<3, 1>(0, 3) = m_Position;

	m_InverseViewMatrix = transformMatrix;
	m_ViewMatrix = transformMatrix.inverse();
}

void Camera::RecalculateProjectionMatrix()
{
	m_ProjectionMatrix.setIdentity();

	float aspectRatio = float(m_ViewportWidth) / float(m_ViewportHeight);
	float tanInv = 1.0f / tan(m_FovY * 0.5f);

	m_ProjectionMatrix(0, 0) = tanInv / aspectRatio;
	m_ProjectionMatrix(1, 1) = tanInv;
	m_ProjectionMatrix(2, 2) = (m_FarClip + m_NearClip) / (m_NearClip - m_FarClip);
	m_ProjectionMatrix(3, 2) = -1;
	m_ProjectionMatrix(2, 3) = (2 * m_FarClip * m_NearClip) / (m_NearClip - m_FarClip);
	m_ProjectionMatrix(3, 3) = 0;

	m_InverseProjectionMatrix = m_ProjectionMatrix.inverse();
}

void Camera::RecalculateRayDirections()
{
	m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

	for (int y = 0; y < m_ViewportHeight; y++)
	{
		for (int x = 0; x < m_ViewportWidth; x++)
		{
			//y = m_ViewportHeight - y - 1;
			//x = m_ViewportWidth - x - 1;
			float xCoordinateInViewport = ((float)x / (float)m_ViewportWidth) * 2.0f - 1.0f;
			float yCoordinateInViewport = ((float)y / (float)m_ViewportHeight) * 2.0f - 1.0f;

			//Eigen::Vector4f target = m_InverseProjectionMatrix * Eigen::Vector4f(xCoordinateInViewport, yCoordinateInViewport, 1, 1);
			//Eigen::Vector3f targetInter = (target / target.w()).head(3).normalized();
			//Eigen::Vector3f rayDirection = (m_InverseViewMatrix * Eigen::Vector4f(targetInter.x(), targetInter.x(), targetInter.z(), 0)).head(3);

			Eigen::Vector4f target = m_InverseViewMatrix * m_InverseProjectionMatrix * Eigen::Vector4f(xCoordinateInViewport, yCoordinateInViewport, 1, 1);
			Eigen::Vector3f rayDirection = (target.head(3) / target.w()).normalized();

			m_RayDirections[x + (m_ViewportHeight - 1 - y) * m_ViewportWidth] = rayDirection;
 		}
	}
}