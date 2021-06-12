#pragma once

#include<vector>
#include "Light.h"
#include "CGVertex.h"
#include "ModelInfos.h"
#include "Matrix.h"

namespace RenderEngine {

	/// forward declaration (not avaible writing in this way in implemention code)
	class Transform;
	struct Colour;
	struct Texcoord;

	class Device
	{
	public:
		static Device& getInstance()
		{
			static Device s_instance;
			return s_instance;
		}

		Device();
		~Device();
		void Initialize(int w, int h, unsigned int* fb, Transform* ts);
		void SetState(const int s);
		int GetState(int s);
		void ChangeInterp();
		void ChangeCullMode();
		void RasterTriangleInAnotherWay(const CGVertex& t1, const CGVertex& t2, const CGVertex& t3);
		void DrawScanline(const CGVertex& A, const CGVertex& B, int y);
		void ScanlineFill(const CGVertex & left, const CGVertex & right, const int yIndex, const Colour** texture);
		void TriangleRasterization(const CGVertex& p1, const CGVertex& p2, const CGVertex& p3, const Colour** texture = NULL);
		void DrawModel(ModelInfo& model, const float theta, const Vector3& translationDeltaOnXYZ);
		void DrawPixel(int x, int y, unsigned int hexColor);
		void DrawPixel(const Vector2& point, unsigned int hexColor);
		void DrawPixel(int x, int y, const Colour& color);
		void DrawPixel(const Vector2& point, const Colour& color);
		void DrawPoint(const Vector4& p, const Colour& color, const Texcoord& tc = { 0.f,0.f }, const Vector3& normal = (0.f, 0.f, 0.f));
		void DrawLine(const Vector4& p1, const Vector4& p2); // Draw line via Point-by-Point Comparison Method
		void DrawBresenhemLine(const Vector4& startPoint, const Vector4& endPoint);
		void DrawBresenhemLineInUnoptimizedWay(const Vector4& startPoint, const Vector4& endPoint);
		void DrawTriangle(CGVertex& v1, CGVertex& v2, CGVertex& v3, const Colour** texture);
		void DrawPlane(const CGVertex& v1, const CGVertex& v2, const CGVertex& v3, const CGVertex& v4, const bool isDrawTestBox = false);
		void DrawBox(float theta);
		void UpdateModelRotation(const Vector2& lastPointV2, const Vector2& curPointV2);
		void UpdateLightSpaceMatrix(); // a way of optimization using Obsever Pattern
		float GetBiasDynamically(const float initialBias, const Vector4 rightPointNormal, const Vector4 leftPointNormal, const float lerpFactor);
		void UpdateLightPosition();
		void RenderUpdate(ModelInfo& objModel);
		void DrawPlaneForShadowShowing();
		void Close();

		int** m_createdTextureHolder;

		float theta;
		float phi = 90.f; // magic number for initialized light position on sphere

		float gModelScaleFactor = 1.f; // scale model via scale matrix

		Colour** pixelColorData;
		Colour** pixelColorData4ShadowPlane;
		int textureWidth, textureHeight;


	private:
		void CodesForTest();
		void Device::DrawOriginalPos();
		void Device::DrawLightPositionAsABox(const Vector4 & lightPos);
		void FixUV(CGVertex& v1, CGVertex& v2, CGVertex& v3);
		void FixNormal(CGVertex& v1, CGVertex& v2, CGVertex& v3);

		void ScanLineTriangleRasterization(const CGVertex & v1, const CGVertex & v2, const CGVertex & v3,
			const Vector4& transformedVertPos1, const Vector4& transformedVertPos2, const Vector4& transformedVertPos3,
			const Vector4& transformedVertNormal1, const Vector4& transformedVertNormal2, const Vector4& transformedVertNormal3,
			const Vector4& homogenizedVertPos1, const Vector4& homogenizedVertPos2, const Vector4& homogenizedVertPos3, const Colour** texture);

		void HalfSpaceTriangleRasterization(const CGVertex & v1, const CGVertex & v2, const CGVertex & v3,
			const Vector4& transformedVertPos1, const Vector4& transformedVertPos2, const Vector4& transformedVertPos3,
			const Vector4& transformedVertNormal1, const Vector4& transformedVertNormal2, const Vector4& transformedVertNormal3,
			const Vector4& homogenizedVertPos1, const Vector4& homogenizedVertPos2, const Vector4& homogenizedVertPos3);

		void ScreenSpaceLerpCGVertex(CGVertex& v, const CGVertex& v1, const CGVertex& v2, float t);
		void DrawTopTriangle(const CGVertex & p1, const CGVertex & p2, const CGVertex & p3, const Colour** texture);
		void DrawBottomTriangle(const CGVertex& p1, const CGVertex& p2, const CGVertex& p3, const Colour** texture);
		void DrawTriangleForShadowMap(CGVertex& v1, CGVertex& v2, CGVertex& v3);
		//bool Device::IsVertexInShadow(const Vector4& world_point);
		void ClearBuffers();


		inline unsigned int GetHEXColor(Colour color) {
			unsigned int r = (unsigned int)(color.r * 255.f);
			unsigned int g = (unsigned int)(color.g * 255.f);
			unsigned int b = (unsigned int)(color.b * 255.f);
			unsigned int hexColor = (r << 16) | (g << 8) | (b);
			return hexColor;
		}
		inline Colour GetColorFromHEX(unsigned int hexColor) {
			float inv = (float)1 / 255;
			Colour color((hexColor >> 16) * inv, (hexColor >> 8 & 0xff) * inv, (hexColor & 0xff) * inv);
			return color;
		}
		inline float LerpInDeviceContext(const float a, const float b, const float t)
		{
			if (t <= 0.f) return a;
			else if (t >= 1.f) return b;
			else return (b - a) * t + a; // optimization
		}
		inline Vector4 LerpInDeviceContext(const Vector4 vecA, const Vector4 vecB, const float t)
		{
			Vector4 result(
				LerpInDeviceContext(vecA.getX(), vecB.getX(), t),
				LerpInDeviceContext(vecA.getY(), vecB.getY(), t),
				LerpInDeviceContext(vecA.getZ(), vecB.getZ(), t),
				LerpInDeviceContext(vecA.getW(), vecB.getW(), t)
			);
			return result;
		}
		inline CGVertex LerpInDeviceContext(const CGVertex vecA, const CGVertex  vecB, const float t)
		{
			CGVertex result{
				LerpInDeviceContext(vecA.pos, vecB.pos, t),
				LerpInDeviceContext(vecA.color, vecB.color, t),
				LerpInDeviceContext(vecA.tex.u, vecB.tex.u, t),
				LerpInDeviceContext(vecA.tex.v, vecB.tex.v, t),
				LerpInDeviceContext(vecA.normal, vecB.normal, t),
				LerpInDeviceContext(vecA.rhw, vecB.rhw, t),
				LerpInDeviceContext(vecA.posInWorldSpace, vecB.posInWorldSpace, t)
			};
			return result;
		}
		inline float CleanLerp(const float a, const float b, const float t) {
			return (b - a) * t + a; // optimization of Lerp formula in Unity API
		}
		inline Colour LerpInDeviceContext(const Colour& a, const Colour& b, float t)
		{
			Colour result;
			result.r = Lerp(a.r, b.r, t);
			result.g = Lerp(a.g, b.g, t);
			result.b = Lerp(a.b, b.b, t);
			return result;
		}
		inline float ClampInDeviceContext(float param, float min, float max) {
			return ((param < min) ? min : ((param > max) ? max : param));
		}
		inline float MapTo0_255fInDeviceContext(const float x)
		{
			float result = ClampInDeviceContext(x, 0.f, 0.999999f) * 255.f;
			if (result - EPSILON >= 1.f)
				result = floorf(ClampInDeviceContext(x, 0.f, 0.999999f) * 256.f);
			return result;
		}
		inline Colour GetGrayValueViaGamaCorrection(const float rgbValue)
		{
			float numerator = powf(MapTo0_255fInDeviceContext(rgbValue), 2.2f) + powf((1.5f*MapTo0_255fInDeviceContext(rgbValue)), 2.2f) + powf((0.6f*MapTo0_255fInDeviceContext(rgbValue)), 2.2f);
			float denominator = 1.f + powf(1.5f, 2.2f) + powf(0.6f, 2.2f);
			float result = powf((numerator / denominator), 1.f / 2.2f);
			return Colour(result, result, result);
		}
		inline Colour GetBrightnessViaGamaCorrection(const float rgbValue)
		{
			float numerator = powf(rgbValue, 2.2f) + powf((1.5f*rgbValue), 2.2f) + powf((0.6f*rgbValue), 2.2f);
			float denominator = 1.f + powf(1.5f, 2.2f) + powf(0.6f, 2.2f);
			float result = powf((numerator / denominator), 1.f / 2.2f);
			return Colour(result, result, result);
		}
		inline void Vector4DotMatrix4fInDeviceContext(Vector4& result, const Vector4 & vec4, const Matrix4f & m4f)
		{
			result.setX(vec4.getX() * m4f.matrix[0][0] + vec4.getY() * m4f.matrix[1][0] + vec4.getZ() * m4f.matrix[2][0] + vec4.getW() * m4f.matrix[3][0]);
			result.setY(vec4.getX() * m4f.matrix[0][1] + vec4.getY() * m4f.matrix[1][1] + vec4.getZ() * m4f.matrix[2][1] + vec4.getW() * m4f.matrix[3][1]);
			result.setZ(vec4.getX() * m4f.matrix[0][2] + vec4.getY() * m4f.matrix[1][2] + vec4.getZ() * m4f.matrix[2][2] + vec4.getW() * m4f.matrix[3][2]);
			result.setW(vec4.getX() * m4f.matrix[0][3] + vec4.getY() * m4f.matrix[1][3] + vec4.getZ() * m4f.matrix[2][3] + vec4.getW() * m4f.matrix[3][3]);
		}

		Transform* m_transform;
		std::vector<float> m_zbuffer;
		unsigned int ** m_framebuffer;
		int m_state;
		int m_width;
		int m_height;
		int m_interp;
		bool mLightOn;
		int m_cullMode;
		Matrix4f m_rotationFromOutsideInteraction;
		std::vector<float> depthBufferFromLightPos;
		unsigned int m_backgroundColour, m_foregroundColour;
		bool gradientRampedBackgroundColor = false;
		bool m_showShadowBuffer = false;


		/// declarations are seperated from definitions
		/// codes below are for inverttion of a position on screen to the lightScreen space
		bool m_useInvertMethod2GetWorldPos = true;
		Matrix4f m_invertedMatrix;
		Vector4 posOnScreen;
		Vector4 invertedHomogenizedPos;
		Vector4 posInWorldSpace;
		Matrix4f lightSpaceMatrix;
		Vector4 lightScreenPosBeforsHomogenized;
		Vector4 posInLightScreen;

		/// other tags
		bool m_scalePlaneForShadowShowing = false;
		bool m_showPosInLightScreenDepth = false;
	};
}