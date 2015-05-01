#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
	m_View = XMMatrixIdentity();
}

Camera::Camera(ProjectionType type, float fieldOfView, float width, float height, float screenNear, float screenFar)
{
	SetProjection(type, fieldOfView, width, height, screenNear, screenFar);
}

Camera::~Camera()
{
}

PVOID Camera::operator new(size_t size)
{
	PVOID p = _aligned_malloc(size, 16);
	if (!p)
		throw std::bad_alloc();

	return p;
};

VOID Camera::operator delete(PVOID p)
{
	Camera *ptr = static_cast<Camera*>(p);
	_aligned_free(ptr);
};

void Camera::SetProjection(ProjectionType type, float fieldOfView, float width, float height, float screenNear, float screenFar)
{
	m_Width = width;
	m_Height = height;

	float aspectRatio;

	switch (type)
	{
		case Orthographic:
			m_Projection = XMMatrixOrthographicLH(width, height, screenNear, screenFar);
			break;

		case Perspective:
			aspectRatio = (float)width / (float)height;
			m_Projection = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, screenNear, screenFar);
			break;
	}

	// Create frustum.
	BoundingFrustum::CreateFromMatrix(m_Frustum, m_Projection);
}

void Camera::SetDistance(float distance)
{
	if (distance > 0.f)
	{
		m_Distance = distance;
		m_Look = XMVector3Normalize(m_Look) * m_Distance;
	}
}

void Camera::SetRotation(DirectX::XMVECTOR rotation)
{
	XMVECTOR look = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	m_Look = XMVector3Rotate(look, XMQuaternionRotationRollPitchYawFromVector(rotation));
	m_Up = XMVector3Rotate(up, XMQuaternionRotationRollPitchYawFromVector(rotation));

	// Reset distance.
	SetDistance(m_Distance);
}

void Camera::SetRotation(RotationPreset preset)
{
	switch (preset)
	{
		case Isometric:
			SetRotation(ISOMETRIC);
			break;
		default:
			SetRotation(XMVectorZero());
			break;
	}
}

void Camera::SetPosition(DirectX::XMVECTOR position)
{
	m_Position = position;
}

void Camera::SetFocus(DirectX::XMVECTOR focus)
{
	m_Position = focus - m_Look;
}

void Camera::SetFocus(Direction direction)
{
	XMVECTOR v[6] = 
	{
		XMVectorSet(0.f, XM_PIDIV2, 0.f, 0.f),
		XMVectorSet(0.f, -XM_PIDIV2, 0.f, 0.f),
		XMVectorSet(XM_PIDIV2, 0.f, 0.f, 0.f),
		XMVectorSet(-XM_PIDIV2, 0.f, 0.f, 0.f),
		XMVectorSet(0.f, 0.f, 0.f, 0.f),
		XMVectorSet(0.f, XM_PI, 0.f, 0.f),
	};

	SetRotation(v[direction]);
}

void Camera::Update(float deltaTime)
{
	// Update view.
	m_View = XMMatrixLookAtLH(m_Position, m_Position + m_Look, m_Up);
}

DirectX::XMMATRIX Camera::GetViewProjection()
{
	return m_View * m_Projection;
}

DirectX::XMMATRIX Camera::GetTransposedViewProjection()
{
	return XMMatrixTranspose(m_View * m_Projection);
}

const XMMATRIX Camera::GetView()
{
	return m_View;
}

const XMMATRIX Camera::GetProjection()
{
	return m_Projection;
}

const XMVECTOR& Camera::GetPosition()
{
	return m_Position;
}

const XMVECTOR& Camera::GetLookVector()
{
	return m_Look;
}

BoundingFrustum Camera::GetFrustum()
{
	BoundingFrustum f;

	// Inverse view matrix to transform into world space.
	m_Frustum.Transform(f, XMMatrixInverse(nullptr, m_View));

	return f;
}

DirectX::XMVECTOR Camera::UnprojectScreenPoint(POINT cursorPosition)
{
	return XMVector3Normalize(
		XMVector3Unproject(
		XMVectorSet((float)cursorPosition.x, (float)cursorPosition.y, 1, 1),
		0,
		0,
		m_Width,
		m_Height,
		0.f,
		1.f,
		m_Projection,
		m_View,
		XMMatrixIdentity()));
}

DirectX::XMVECTOR Camera::UnprojectScreenPoint(POINT cursorPosition, const XMMATRIX &world)
{
	return XMVector3Normalize(
		XMVector3Unproject(
		XMVectorSet((float)cursorPosition.x, (float)cursorPosition.y, 1, 1),
		0,
		0,
		m_Width,
		m_Height,
		0.f,
		1.f,
		m_Projection,
		m_View,
		world));
}

DirectX::XMVECTOR Camera::UnprojectScreenPoint(POINT cursorPosition, const D3D11_VIEWPORT &viewport, const XMMATRIX &world)
{
	return XMVector3Normalize(
		XMVector3Unproject(
		XMVectorSet((float)cursorPosition.x, (float)cursorPosition.y, 1, 1),
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		m_Projection,
		m_View,
		world));
}