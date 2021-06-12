#pragma once

#include "Math.h"
#include "Vector.h"
#include "Matrix.h"
#include "Light.h"
#include "CGVertex.h"

namespace RenderEngine {

	class Transform
	{
	public:
		static Transform& getInstance()
		{
			static Transform s_instance;
			return s_instance;
		}

		void Initialize(int width, int height);
		void UpdateTransform();
		void UpdateTransformInvert();
		void UpdateTransformForShadowMap();
		void ApplyTransform(Vector4& result, const Vector4& vec);
		void WorldToModel(Vector4& result, const Vector4& vec);
		void ModelToWorld(Vector4& result, const Vector4& vec);
		Vector4 Homogenize(Vector4 & result, const Vector4 & vec4ToBeHomogenized);
		Vector4  HomogenizeInvertion(Vector4 & result, const Vector4 & vec4ToBeInverted);
		bool IsOutsideCVV(const Vector4& v);
		bool ShouldCullBack(const Vector4& v);
		Vector4 GetNormalVectorForBackCulling(const Vector4& p1, const Vector4& p2, const Vector4& p3);

		void SetCamera(Vector3& eyePos, Vector3& lookAt, Vector3& upAxis);
		void TransformLight(Light& light, float theta);

		Matrix4f orthographicProjectionMatrix;
		Matrix4f perspectiveProjectionMatrix;


		Matrix4f viewMatrix;
		Matrix4f worldMatrix;
		Matrix4f lightSpaceMatrix;
		Matrix4f transformMatrix;
		Matrix4f projectionMatrix;
	private:
		float canvasWidth, canvasHeight; // screen



	public:
		void SetLightAsCamera(Vector3 & eyePos, Vector3 & lookAt, Vector3 & upAxis);
		void ApplyLightTransform(Vector4& result, const Vector4& vec);
		void ModelToLightSpace(Vector4& result, const Vector4& vec);
		Matrix4f lightViewMatrix;
		Matrix4f lightTransformMatrix;


	};


	extern bool gRenderInPerspectiveMode;
}
