#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Window.h"


namespace RenderEngine {

#define TO_RADIANS(degrees) ((MATH_PI / 180) * (degrees))
#define TO_DEGREES(radians) ((180 / MATH_PI) * (radians))

	class Camera
	{
	public:
		Camera(Vector4 p, Vector4 t, Vector4 u, float a)
			:position(p), target(t), up(u), aspect(a),
			trans({ 0.0f,0.0f,10.0f })
		{
			model = Matrix4f::getIdentityMatrix();
		};
		~Camera() {};

		void SetPostion(const Vector4& p) { position = p; }
		Vector4 GetPostion() { return position; } const

			void SetTarget(const Vector4& t) { target = t; }
		Vector4 GetTarget() { return target; } const

			void SetUp(const Vector4& u) { up = u; }
		Vector4 GetUp() { return up; } const

			Matrix4f GetModelMatrix() const { return model; }
		Matrix4f GetInvModelMatrix() const { return invModel; }

		Matrix4f GetViewMatrix() const { return view; } const
			Matrix4f GetStaticViewMatrix() const { return staticView; }

		Matrix4f GetProjectionMatrix() const { return proj; }

		void SetTrans(const Vector3& t) { trans = t; }
		Vector3 GetTrans() const { return trans; }

		void SetViewRot(const Vector3& v) { viewRot = v; }
		Vector3 GetViewRot() const { return viewRot; }

		void SetModelRot(const Vector3& v) { modelRot = v; }
		Vector3 GetModelRot() const { return modelRot; }

		float GetNear() const { return Near; }
		float GetFar() const { return Far; }
		float GetFovY() const { return FovY; }

		void Update();

	private:
		void UpdateViewMatrix();
		void UpdateProjectionMatirx();

	private:
		Vector4 position;
		Vector4 target;
		Vector4 up;
		float aspect;

		Matrix4f model;
		Matrix4f invModel;
		Matrix4f view;
		Matrix4f staticView;
		Matrix4f proj;

		Vector3 trans;

		Vector3 viewRot;

		Vector3 modelRot;

		// consts 
		const float Near = 1.0f;
		const float Far = 500.f;
		const float FovY = TO_RADIANS(90);
	};

}