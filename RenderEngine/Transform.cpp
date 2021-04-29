#include "Transform.h"
#include "Light.h"

namespace RenderEngine {

	void Transform::Initialize(int width, int height)
	{
		canvasWidth = (float)width;
		canvasHeight = (float)height;

		worldMatrix = Matrix4f::getIdentityMatrix();
		lightSpaceMatrix = Matrix4f::getIdentityMatrix();
		viewMatrix = Matrix4f::getIdentityMatrix();
		lightViewMatrix = Matrix4f::getIdentityMatrix();

		//// Get perspective projection matrix
		float aspectRatio = canvasWidth / canvasHeight; // Never try to change aspectRatio's type from float to integer!
		perspectiveProjectionMatrix = Matrix4f::getPerspectiveMatrix(90.f, aspectRatio, 0.2f, 500.f);

		//// Get orthographic projection matrix
		float left, bottom, right, top; // conus size
		left = bottom = -2.7f; // not a magic number ---> got from Triangle Formula
		right = top = 2.7f;
		orthographicProjectionMatrix = Matrix4f::getOrthographicMatrix(left, right, bottom, top, -1.21f, 500.f); // -1 is somewhat magic

		//// use one kind of projection matrix
		UpdateTransform();
	}

	void Transform::UpdateTransform()
	{
		Matrix4f m = worldMatrix * viewMatrix;

		if (gRenderInPerspectiveMode) projectionMatrix = perspectiveProjectionMatrix;
		else projectionMatrix = orthographicProjectionMatrix;

		transformMatrix = m * projectionMatrix;
	}

	void Transform::UpdateTransformInvert()
	{
		Matrix4f m = Matrix4f::getMatrixInvert(viewMatrix) * Matrix4f::getMatrixInvert(worldMatrix);

		if (gRenderInPerspectiveMode) projectionMatrix = perspectiveProjectionMatrix;
		else projectionMatrix = orthographicProjectionMatrix;

		transformMatrix = Matrix4f::getMatrixInvert(projectionMatrix) * m;
	}

	void Transform::UpdateTransformForShadowMap()
	{
		Matrix4f m = lightSpaceMatrix * lightViewMatrix;
		lightTransformMatrix = m * orthographicProjectionMatrix;
	}

	void Transform::ApplyTransform(Vector4& result, const Vector4& vec)
	{
		result = Vector4DotMatrix4f(vec, transformMatrix);
	}

	void Transform::ModelToWorld(Vector4& result, const Vector4& vec)
	{
		result = Vector4DotMatrix4f(vec, worldMatrix);
	}

	void Transform::WorldToModel(Vector4& result, const Vector4& vec)
	{
		result = Vector4DotMatrix4f(vec, Matrix4f::getMatrixInvert(worldMatrix));
	}

	
	Vector4 Transform::Homogenize(Vector4& result, const Vector4& vec4ToBeHomogenized)
	{
		if (vec4ToBeHomogenized.getW() == 0.f) {
			return Vector4();
		}
		float rhw = 1 / vec4ToBeHomogenized.getW();
		result.setX((1.f + vec4ToBeHomogenized.getX() * rhw) * canvasWidth * 0.5f);	// screen coordinate
		result.setY((1.f - vec4ToBeHomogenized.getY() * rhw) * canvasHeight * 0.5f); //screen coordinate ---> top down
		result.setZ(vec4ToBeHomogenized.getZ() * rhw);
		result.setW(rhw);
		return result;
	}

	Vector4 Transform::HomogenizeInvertion(Vector4 & result, const Vector4 & vec4ToBeInverted)
	{
		if (vec4ToBeInverted.getW() == 0.f) {
			return Vector4();
		}

		float rhw = 1.f / vec4ToBeInverted.getW();
		
		float reciprocalOfCanvasHeight = 1.f / canvasHeight;
		float reciprocalOfCanvasWidth = 1.f / canvasWidth;

		result.setX(((2 * vec4ToBeInverted.getX() * reciprocalOfCanvasWidth) - 1.f) * rhw);
		result.setY((1.f - (2 * vec4ToBeInverted.getY() * reciprocalOfCanvasHeight)) * rhw);
		result.setZ(vec4ToBeInverted.getZ() * rhw);
		result.setW(rhw);

		return result;
	}

	bool Transform::IsOutsideCVV(const Vector4& v)
	{
		float x = v.getX();
		float y = v.getY();
		float z = v.getZ();
		float w = v.getW();

		if (x <= w && x >= -w && y <= w && y >= -w && z <= w && z >= 0)
			return false;

		return true;
	}

	bool Transform::ShouldCullBack(const Vector4& vec)
	{
		// anti-clock wise culling
		Vector4 v(0.f, 0.f, 1.f, 0.f);
		if (v * vec > 0) return false;
		return true;
	}

	Vector4 Transform::GetNormalVectorForBackCulling(const Vector4& p1, const Vector4& p2, const Vector4& p3)
	{
		Vector4 s1, s2, pn;
		s1 = p2 - p1;
		s2 = p3 - p2;
		pn = s2 ^ s1;
		pn.Normalize();
		return pn;
	}

	void Transform::SetCamera(Vector3 & eyePos, Vector3 & lookAt, Vector3 & upAxis)
	{
		viewMatrix = Matrix4f::getLookAtMatrixForLeftHandCoordinate(&eyePos, &lookAt, &upAxis, upAxis);
		UpdateTransform();
	}

	void Transform::SetLightAsCamera(Vector3 & eyePos, Vector3 & lookAt, Vector3 & upAxis)
	{
		lightViewMatrix = Matrix4f::getLookAtMatrixForLeftHandCoordinate(&eyePos, &lookAt, &upAxis, upAxis);
		UpdateTransformForShadowMap();
	}

	void Transform::ModelToLightSpace(Vector4& result, const Vector4& vec)
	{
		result = Vector4DotMatrix4f(vec, lightSpaceMatrix);
	}

	void Transform::ApplyLightTransform(Vector4 & result, const Vector4 & vec)
	{
		result = Vector4DotMatrix4f(vec, lightTransformMatrix);
	}

	bool gRenderInPerspectiveMode = true;


	void Transform::TransformLight(Light & light, float theta)
	{
		Vector3 axisToRotateAround(0.f, 1.f, 0.f);
		Matrix4f rotationMatrix = Matrix4f::getRotationMatrix(axisToRotateAround, theta);

		worldMatrix = rotationMatrix;
		UpdateTransform();

		lightSpaceMatrix = rotationMatrix;
		UpdateTransformForShadowMap();

		Vector4 lightDirectioin(light.direction.getX(), light.direction.getY(), light.direction.getZ(), 0.f);
		ModelToWorld(lightDirectioin, { -0.3f, 1.f, -0.3f, 0.f });
		light.direction.Normalize();
	}
}
