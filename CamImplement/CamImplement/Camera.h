//
//
//

#ifndef CAMERA__H
#define CAMERA__H

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <Windows.h>
#include <d3d11.h>
#include <new>

//		Opposite corner of a cube require
//
//		x rotation 35.264 deg	= 0.615 rad
//		y rotation 45.0 deg		= 0.785 rad
//			 = isometric view. 

#define ISOMETRIC		DirectX::XMVectorSet(0.615f, 0.785f, 0.f, 0.f)
#define DEFAULT_DISTANCE 30.0f

enum ProjectionType
{
	Orthographic,
	Perspective
};

enum RotationPreset
{
	Isometric
};

class Camera
{
	private:
		DirectX::XMMATRIX m_View = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX m_Projection = DirectX::XMMatrixIdentity();

		FLOAT m_Distance = 10;
		DirectX::XMVECTOR m_Up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
		DirectX::XMVECTOR m_Look = DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f);
		DirectX::XMVECTOR m_Position = DirectX::XMVectorZero();

		DirectX::BoundingFrustum m_Frustum;

		FLOAT m_Width;
		FLOAT m_Height;

		enum Direction
		{
			Right,
			Left,
			Up,
			Down,
			Forward,
			Backward
		};

	public:
		Camera();
		Camera(ProjectionType type,
			float fieldOfView,
			float width,
			float height,
			float screenNear,
			float screenFar);

		virtual ~Camera();

		void* operator new(size_t size);
		void operator delete(void *p);

		void SetProjection(ProjectionType type,
			float fieldOfView,
			float width,
			float height,
			float screenNear,
			float screenFar);
		
		void SetDistance(float distance = DEFAULT_DISTANCE);
		
		void SetRotation(DirectX::XMVECTOR rotation);
		void SetRotation(RotationPreset preset);

		void SetPosition(DirectX::XMVECTOR position);
		
		void SetFocus(DirectX::XMVECTOR focus);
		void SetFocus(Direction direction);

		virtual void Update(float deltaTime);

		DirectX::XMMATRIX GetViewProjection();
		DirectX::XMMATRIX GetTransposedViewProjection();

		const DirectX::XMMATRIX GetView();
		const DirectX::XMMATRIX GetProjection();

		const DirectX::XMVECTOR& GetPosition();
		const DirectX::XMVECTOR& GetLookVector();
		const DirectX::XMVECTOR& GetRotation();

		DirectX::BoundingFrustum GetFrustum();

		// Convert screen point into object space ray. Originating from camera position.
		DirectX::XMVECTOR UnprojectScreenPoint(POINT cursorPosition);
		DirectX::XMVECTOR UnprojectScreenPoint(POINT cursorPosition, const DirectX::XMMATRIX &world);
		DirectX::XMVECTOR UnprojectScreenPoint(POINT cursorPosition, const D3D11_VIEWPORT &viewport, const DirectX::XMMATRIX &world);
};



#endif /// CAMERA__H