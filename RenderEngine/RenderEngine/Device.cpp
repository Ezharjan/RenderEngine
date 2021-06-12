#include <Windows.h>
#include <vector>
#include <math.h>
#include "Light.h"
#include "Vector.h"
#include "Config.h"
#include "Device.h"
#include "Window.h"
#include "Colour.h"
#include "GeoBase.h"
#include "CGVertex.h"
#include "Transform.h"
#include "Textcoord.h"
#include "ModelInfos.h"


namespace RenderEngine {

	//static float biasDelta = 0.0013f; // experienced bias --> 0.0013f ~ 0.0011f
	static float biasDelta = 0.0005f; // get the shadow of the hand
	static float biasDeltaStorage = 0.f; // get the shadow of the hand

	static float gLightPosX = 0.f;
	static float gLightPosY = 0.f;
	static float gLightPosZ = 1.f;
	static float gDirectionalLightIntensity = 1.f;


	Device::Device() : m_transform(NULL), m_framebuffer(NULL), m_backgroundColour(0), m_foregroundColour(0),
		m_width(0), m_height(0), m_state(0), m_interp(0), m_cullMode(0)
	{
		m_rotationFromOutsideInteraction = Matrix4f::getIdentityMatrix();
	}

	Device::~Device()
	{
		delete[] pixelColorData;
		delete[] pixelColorData4ShadowPlane;
	}

	void Device::Initialize(int w, int h, unsigned int* fb, Transform* ts)
	{
		m_width = w;
		m_height = h;

		m_framebuffer = (unsigned int**)malloc(h * sizeof(unsigned int*));
		for (int y = 0; y < h; y++) {
			m_framebuffer[y] = fb + y * w;
		}

		m_zbuffer.assign(w * h, 1.f); // initialize to furthest depth
		depthBufferFromLightPos.assign(w * h, 1.f); // initialize to furthest depth

		m_backgroundColour = 0xc0c0c0; // silver
		m_foregroundColour = 0xffffff; // white

		m_transform = ts;

		mLightOn = true;
	}

	void Device::ClearBuffers()
	{
		m_zbuffer.assign(m_zbuffer.size(), 1.f); // clear to furthest depth
		depthBufferFromLightPos.assign(depthBufferFromLightPos.size(), 1.f); // clear to furthest depth

		float inv_h = (float)1 / m_height;
		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				int c = (int)((float)0xc0 * inv_h * (m_height - (float)y)); // silver
				if (gradientRampedBackgroundColor)
					m_framebuffer[y][x] = (c << 16 | c << 8 | c); // ---> gradient ramp background
				else
					//if (m_showShadowBuffer)
					if (m_showShadowBuffer || m_showPosInLightScreenDepth) // for light-screen-pos test 
						m_framebuffer[y][x] = GetHEXColor(Colour(m_zbuffer[y*m_width + x], m_zbuffer[y*m_width + x], m_zbuffer[y*m_width + x]));
					else
						m_framebuffer[y][x] = 0;
			}
		}
	}

	void Device::Close()
	{
		if (m_framebuffer != NULL)
			free(m_framebuffer);
	}

	void Device::SetState(const int s)
	{
		m_state = s;
	}

	int Device::GetState(int s)
	{
		s = s % 6;
		if (s == 0) {
			return STATE_DRAW_COLOR;
		}
		else if (s == 1) {
			return STATE_DRAW_COLOR | STATE_DRAW_LINE;
		}
		else if (s == 2) {
			return STATE_DRAW_TEX;
		}
		else if (s == 3)
		{
			return DRAW_MODEL | STATE_DRAW_LINE;
		}
		else if (s == 4)
		{
			return DRAW_MODEL | STATE_DRAW_TEX;
		}
		else if (s == 5)
		{
			return DRAW_CREATED_TEX;
		}
		return 0;
	}

	void Device::ChangeInterp()
	{
		std::cout << m_interp << std::endl;
		if (m_interp == INTERP_NONE) {
			m_interp = INTERP_BILINEAR;
		}
		else {
			m_interp = INTERP_NONE;
		}
	}

	void Device::ChangeCullMode()
	{
		if (m_cullMode == CULL_MODE_NONE) {
			m_cullMode = CULL_MODE_BACK;
		}
		else {
			m_cullMode = CULL_MODE_NONE;
		}
	}

	void Device::DrawPixel(int x, int y, unsigned int hexColor)
	{
		if ((x < m_width) && ((y < m_height)) && ((x >= 0)) && ((y >= 0)))
		{
			m_framebuffer[y][x] = hexColor;
		}
	}

	void Device::DrawPixel(const Vector2 & point, unsigned int hexColor)
	{
		int x = (int)point.getX();
		int y = (int)point.getY();
		DrawPixel(x, y, hexColor);
	}

	void Device::DrawPixel(int x, int y, const Colour & color)
	{
		if ((x < m_width) && ((y < m_height))
			&& ((x >= 0)) && ((y >= 0)))
		{
			m_framebuffer[y][x] = GetHEXColor(color);
		}
	}

	void Device::DrawPixel(const Vector2 & point, const Colour & color)
	{
		int x = (int)point.getX();
		int y = (int)point.getY();
		DrawPixel(x, y, color);
	}


	void Device::DrawPoint(const Vector4& p, const Colour& color, const Texcoord& tc, const Vector3& normal)
	{
		int y = (int)p.getY();
		int x = (int)p.getX();

		if (y < 0 || y >= m_height) return;
		if (x < 0 || x >= m_width) return;

		float& z = m_zbuffer[y * m_width + x];

		if (z < p.getZ()) return;

		unsigned int fcolor = 0;

		int s = GetState(m_state);

		if (s & STATE_DRAW_COLOR)
		{
			fcolor = GetHEXColor(color);
		}
		else
		{
			Colour textureColor;
			if (s & STATE_DRAW_TEX) {

				if (m_interp == INTERP_NONE) {
					int tx = (int)roundf((tc.u * (textureWidth - 1)));
					int ty = (int)roundf((tc.v * (textureHeight - 1)));
					textureColor = pixelColorData[ty][tx];
				}
				else
				{
					textureColor = BilinearInterp(pixelColorData, textureWidth, textureHeight, tc.u, tc.v);
				}
			}

			// default ambient light
			Colour ambient(0.f, 1.f, 1.f);
			float ambientIntensity = AMBIENT_INTENSITY;

			// influence of ambient
			ambient.r *= ambientIntensity * textureColor.r;
			ambient.g *= ambientIntensity * textureColor.g;
			ambient.b *= ambientIntensity * textureColor.b;

			//fcolor = (int(ambient.r * 255) << 16 | int(ambient.g * 255) << 8 | int(ambient.b * 255));
			fcolor = GetHEXColor(Colour(ambient.r, ambient.g, ambient.b)); // simple functions of the code above
		}

		m_framebuffer[y][x] = fcolor;
		z = p.getZ();
	}

	// Draw line via Point-by-Point Comparison Method
	void Device::DrawLine(const Vector4& p1, const Vector4& p2)
	{
		Colour color = GetColorFromHEX(m_foregroundColour);

		Texcoord tex = { 0.f, 0.f };
		Vector3 normal(0.f, 0.f, 0.f);

		int x1, y1, x2, y2;
		x1 = (int)p1.getX();
		y1 = (int)p1.getY();
		x2 = (int)p2.getX();
		y2 = (int)p2.getY();

		float y, x;
		y = p1.getY();
		x = p1.getX();

		if (x1 == x2 && y1 == y2) {
			DrawPoint(p1, color, tex, normal);
		}
		else if (x1 == x2) {
			DrawPoint(p1, color, tex, normal);
			int increment = (y1 < y2) ? 1 : -1;
			while (1) {
				y += increment;
				if (increment == 1 && y >= y2) break;
				if (increment == -1 && y <= y2) break;
				Vector4 p(x, y, 0.f, 1.f);
				DrawPoint(p, color, tex, normal);
			}
			DrawPoint(p2, color, tex, normal);
		}
		else if (y1 == y2) {
			DrawPoint(p1, color, tex, normal);
			int increment = (x1 < x2) ? 1 : -1;
			while (1) {
				x += increment;
				if (increment == 1 && x >= x2) break;
				if (increment == -1 && x <= x2) break;
				Vector4 p(x, y, 0.f, 1.f);
				DrawPoint(p, color, tex, normal);
			}
			DrawPoint(p2, color, tex, normal);
		}
		else {
			DrawPoint(p1, color, tex, normal);
			float t = (float)abs(x2 - x1) / abs(y2 - y1);
			int xIncrement = (p1.getX() < p2.getX()) ? 1 : -1;
			int yIncrement = (p1.getY() < p2.getY()) ? 1 : -1;
			while (1) {
				y += yIncrement;
				if (yIncrement == 1 && y >= y2) break;
				if (yIncrement == -1 && y <= y2) break;
				x += t * xIncrement;
				Vector4 p(x, y, 0.f, 1.f);
				DrawPoint(p, color, tex, normal);
			}
			DrawPoint(p2, color, tex, normal);
		}
	}

	void Device::DrawBresenhemLine(const Vector4 & startPoint, const Vector4 & endPoint)
	{
		if (startPoint == endPoint)	return;

		int x0 = (int)startPoint.getX();
		int y0 = (int)startPoint.getY();
		int x1 = (int)endPoint.getX();
		int y1 = (int)endPoint.getY();

		bool steep = false;
		if (std::abs(x0 - x1) < std::abs(y0 - y1))
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
			steep = true;
		}
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}
		int dx = x1 - x0;
		int dy = y1 - y0;
		int derror2 = std::abs(dy) * 2;
		int error2 = 0;
		int y = y0;
		for (int x = x0; x <= x1; ++x) {
			if (steep) {
				Colour co(1.f, 1.f, 1.f);
				//DrawPixel(y, x, GetColorIntensity(co)); // way 1
				Vector4 p((float)y, (float)x, 0.f, 0.f);
				DrawPoint(p, co); // way 2
			}
			else {
				Colour co(1.f, 1.f, 1.f);
				//DrawPixel(x, y, GetColorIntensity(co)); // way 1
				Vector4 p((float)x, (float)y, 0.f, 0.f);
				DrawPoint(p, co); // way 2
			}
			error2 += derror2;
			if (error2 > dx) {
				y += (y1 > y0 ? 1 : -1);
				error2 -= dx * 2;
			}
		}
	}

	void Device::DrawBresenhemLineInUnoptimizedWay(const Vector4 & startPoint, const Vector4 & endPoint)
	{
		if (startPoint == endPoint)	return;

		float x1 = startPoint.getX();
		float y1 = startPoint.getY();
		float x2 = endPoint.getX();
		float y2 = endPoint.getY();

		float xTemp, yTemp;


		/*1：Draw the line segment with no slope.*/

		if (x1 == x2)
		{
			// avoid oversteping the boundary of visual plane
			if (x1 > m_width || x1 < 0)	return;

			if (y1 > y2)
			{
				yTemp = y1;
				y1 = y2;
				y2 = yTemp;
			}
			for (float y = y1; y < y2&&y < m_height&& y >= 0; ++y)
			{
				DrawPixel(Vector2(x1, y), Colour(1.f, 1.f, 1.f));
			}
			return;
		}



		/*2：Draw the line segment with no slope.*/

		// stipulate X1<X2, otherwise, swap them
		if (x1 > x2)
		{
			xTemp = x1;
			x1 = x2;
			x2 = xTemp;

			yTemp = y1;
			y1 = y2;
			y2 = yTemp;
		}

		float k = (y2 - y1) / (x2 - x1);

		// 1：as the gradient factor is less than 1 or bigger than 0
		if (k >= 0.0f&&k <= 1.f)
		{
			// draw pixel crosswise from x1 to x2 with attention of limiting inside window width
			for (float x = x1, y = y1; x <= x2 && x >= 0 && x < m_width; ++x)
			{
				float dis = (x - x1) * k + y1 - y;
				if (dis >= 0.5)
				{
					++y;
				}
				DrawPixel(Vector2(x, y), Colour(1.f, 1.f, 1.f));
			}
		}

		// 2：as the gradient factor is less than 0 or bigger than -1
		else if (k < 0.0f&&k >= -1.f)
		{
			for (float x = x1, y = y1; x <= x2 && x >= 0 && x < m_width; ++x)
			{
				float dis = (x - x1)*k + y1 - y;
				if (dis < -0.5)
				{
					--y;
				}
				DrawPixel(Vector2(x, y), Colour(1.f, 1.f, 1.f));
			}
		}

		// 3：as the gradient factor is bigger than -1
		else if (k > 1.f)
		{
			float k1 = 1.f / k;
			for (float y = y1, x = x1; y <= y2 && y >= 0 && y < m_height; ++y)
			{
				float dis = (y - y1)*k1 + x1 - x;
				if (dis >= 0.5)
				{
					++x;
				}

				DrawPixel(Vector2(x, y), Colour(1.f, 1.f, 1.f));
			}
		}

		// 4：as the gradient factor is less than -1
		else if (k < -1.f)
		{
			float k1 = 1.f / k;
			for (float y = y2, x = x2; y <= y1 && y < m_height && y >= 0; ++y)
			{
				float dis = (y - y2)*k1 + x2 - x;
				if (dis <= -0.5)
				{
					--x;
				}

				DrawPixel(Vector2(x, y), Colour(1.f, 1.f, 1.f));
			}
		}
	}

	Colour ambientLight = Colour(0.3f, 0.3f, 0.3f);
	Vector3& direction = Vector3(0.f, 0.f, 0.f);
	DirectionalLight& directionalLight = DirectionalLight(Colour(0.f, 0.f, 0.f), direction, 0.f);

	void Device::DrawTriangle(CGVertex& v1, CGVertex& v2, CGVertex& v3, const Colour** texture)
	{
		if (m_useInvertMethod2GetWorldPos) {
			// Shadow detection and caculation for drawing
			m_invertedMatrix = Matrix4f::getMatrixInvert(m_transform->perspectiveProjectionMatrix) * Matrix4f::getMatrixInvert(m_transform->viewMatrix);
		}

		Vector4 pos1AfterMVP, pos2AfterMVP, pos3AfterMVP;

#ifdef MVP_DONE_IN_ONE_TIME
		m_transform->ApplyTransform(pos1AfterMVP, v1.pos);
		m_transform->ApplyTransform(pos2AfterMVP, v2.pos);
		m_transform->ApplyTransform(pos3AfterMVP, v3.pos);
#else

		Vector4 pos1TransformedToWorld, pos2TransformedToWorld, pos3TransformedToWorld;
		m_transform->ModelToWorld(pos1TransformedToWorld, v1.pos);
		m_transform->ModelToWorld(pos2TransformedToWorld, v2.pos);
		m_transform->ModelToWorld(pos3TransformedToWorld, v3.pos);

		v1.posInWorldSpace = pos1TransformedToWorld;
		v2.posInWorldSpace = pos2TransformedToWorld;
		v3.posInWorldSpace = pos3TransformedToWorld;

		Vector4 pos1InView, pos2InView, pos3InView;
		Vector4DotMatrix4fInDeviceContext(pos1InView, pos1TransformedToWorld, m_transform->viewMatrix);
		Vector4DotMatrix4fInDeviceContext(pos2InView, pos2TransformedToWorld, m_transform->viewMatrix);
		Vector4DotMatrix4fInDeviceContext(pos3InView, pos3TransformedToWorld, m_transform->viewMatrix);

		Vector4DotMatrix4fInDeviceContext(pos1AfterMVP, pos1InView, m_transform->projectionMatrix);
		Vector4DotMatrix4fInDeviceContext(pos2AfterMVP, pos2InView, m_transform->projectionMatrix);
		Vector4DotMatrix4fInDeviceContext(pos3AfterMVP, pos3InView, m_transform->projectionMatrix);

#endif

		Vector4 transformedVertNormal1, transformedVertNormal2, transformedVertNormal3;

		// CVV Clip here, way 2 --- Clip when three point goes out together.(Way 1 is clipping when only one point is outside)
		if (m_transform->IsOutsideCVV(pos1AfterMVP)
			&& m_transform->IsOutsideCVV(pos2AfterMVP)
			&& m_transform->IsOutsideCVV(pos3AfterMVP)) return;

		m_transform->ModelToWorld(transformedVertNormal1, v1.normal);
		m_transform->ModelToWorld(transformedVertNormal2, v2.normal);
		m_transform->ModelToWorld(transformedVertNormal3, v3.normal);


		if (mLightOn)
		{
			direction = Vector3(gLightPosX, gLightPosY, gLightPosZ);
			directionalLight = DirectionalLight(WHITE, direction, gDirectionalLightIntensity);
			DirectionalLight::LambertLightOn(ambientLight, &directionalLight, Vector3(transformedVertNormal1.getX(), transformedVertNormal1.getY(), transformedVertNormal1.getZ()), v1.color);
			DirectionalLight::LambertLightOn(ambientLight, &directionalLight, Vector3(transformedVertNormal2.getX(), transformedVertNormal2.getY(), transformedVertNormal2.getZ()), v2.color);
			DirectionalLight::LambertLightOn(ambientLight, &directionalLight, Vector3(transformedVertNormal3.getX(), transformedVertNormal3.getY(), transformedVertNormal3.getZ()), v3.color);
		}

		Vector4 homogenizedVertPos1, homogenizedVertPos2, homogenizedVertPos3;
		/// Set projection transform ---> To NDC
		m_transform->Homogenize(homogenizedVertPos1, pos1AfterMVP);
		m_transform->Homogenize(homogenizedVertPos2, pos2AfterMVP);
		m_transform->Homogenize(homogenizedVertPos3, pos3AfterMVP);


		// CULL_MODE_BACK is set to 0 and m_cullMode is in initial value --> 0
		if (m_cullMode == CULL_MODE_BACK) {

			/*Vector4 s1, s2, pn;
			s1 = p2 - p1;
			s2 = p3 - p2;
			pn = s2 ^ s1;
			pn.Normalize();*/
			Vector4 pn = Transform::getInstance().GetNormalVectorForBackCulling(homogenizedVertPos1, homogenizedVertPos2, homogenizedVertPos3);
			if (m_transform->ShouldCullBack(pn)) return;
		}


		int s = GetState(m_state);
		if ((s & STATE_DRAW_LINE)) {
			// way 1
			/*DrawLine(homogenizedVertPos1, homogenizedVertPos2);
			DrawLine(homogenizedVertPos1, homogenizedVertPos3);
			DrawLine(homogenizedVertPos2, homogenizedVertPos3);*/
			// way 2
			DrawBresenhemLineInUnoptimizedWay(homogenizedVertPos1, homogenizedVertPos2);
			DrawBresenhemLineInUnoptimizedWay(homogenizedVertPos1, homogenizedVertPos3);
			DrawBresenhemLineInUnoptimizedWay(homogenizedVertPos2, homogenizedVertPos3);
		}
		else {
			ScanLineTriangleRasterization(v1, v2, v3,
				pos1AfterMVP, pos2AfterMVP, pos3AfterMVP,
				transformedVertNormal1, transformedVertNormal2, transformedVertNormal3,
				homogenizedVertPos1, homogenizedVertPos2, homogenizedVertPos3, texture);
		}
	}

	void Device::FixUV(CGVertex & v1, CGVertex & v2, CGVertex & v3)
	{
		v1.tex.u = v1.tex.v = 0.f;
		v2.tex.u = v2.tex.v = 0.f;
		v3.tex.u = v3.tex.v = 0.f;
	}

	void Device::FixNormal(CGVertex & v1, CGVertex & v2, CGVertex & v3)
	{
		Vector4& p1 = v1.pos;
		Vector4& p2 = v2.pos;
		Vector4& p3 = v3.pos;

		Vector4 edge1, edge2, pn;
		edge1 = p2 - p1;
		edge2 = p3 - p2;
		pn = edge1 ^ edge2;
		pn.Normalize();

		pn.setW(0.f);

		v1.normal = pn;
		v2.normal = pn;
		v3.normal = pn;
	}


	void Device::DrawPlane(const CGVertex & v1, const CGVertex & v2, const CGVertex & v3, const CGVertex & v4, const bool isDrawTestBox)
	{
		// Reorder in case of messing up the UVs
		CGVertex t1 = v1, t2 = v2, t3 = v3, t4 = v4;
		t1.tex.u = 0.f; t2.tex.u = 1.f; t3.tex.u = 1.f; t4.tex.u = 0.f;
		t1.tex.v = 0.f; t2.tex.v = 0.f; t3.tex.v = 1.f; t4.tex.v = 1.f;

		// Delete the 'if' condition below to include the shadow of plane
		if (isDrawTestBox) {
			DrawTriangleForShadowMap(t1, t2, t3);
			DrawTriangleForShadowMap(t3, t4, t1);
		}

		const Colour** texture = (const Colour**)pixelColorData4ShadowPlane;
		DrawTriangle(t1, t2, t3, texture);
		DrawTriangle(t3, t4, t1, texture);
	}


	void Device::DrawBox(float theta)
	{
		Matrix4f rotationMatrix;
		Vector3 rotationAxisX(1.f, 0.f, 0.f);
		Vector3 rotationAxisY(0.f, 1.f, 0.f);

		Matrix4f translationMatrix;
		translationMatrix = Matrix4f::getTranslateMatrix(0.f, 0.f, 0.f);

		Matrix4f influencialMatrix = m_rotationFromOutsideInteraction * translationMatrix;

		Transform::getInstance().worldMatrix = influencialMatrix;
		Transform::getInstance().lightSpaceMatrix = influencialMatrix;

		Transform::getInstance().UpdateTransform();
		Transform::getInstance().UpdateTransformForShadowMap();

		Vector4 vp1(-1.f, 1.f, 1.f, 1.f);
		Colour co1(1.0f, 0.0f, 0.0f);
		Vector4 nor1(-1.f, 1.f, -1.f, 0.f);

		Vector4 vp2(-1.f, 1.f, -1.f, 1.f);
		Colour co2(0.0f, 1.0f, 0.0f);
		Vector4 nor2(-1.f, 1.f, 1.f, 0.f);

		Vector4 vp3(1.f, 1.f, -1.f, 1.f);
		Colour co3(0.0f, 0.0f, 1.0f);
		Vector4 nor3(1.f, 1.f, 1.f, 0.f);

		Vector4 vp4(1.f, 1.f, 1.f, 1.f);
		Colour co4(1.0f, 1.0f, 0.0f);
		Vector4 nor4(1.f, 1.f, -1.f, 0.f);

		Vector4 vp5(-1.f, -1.f, 1.f, 1.f);
		Colour co5(0.0f, 0.0f, 1.0f);
		Vector4 nor5(-1.f, -1.f, -1.f, 0.f);

		Vector4 vp6(-1.f, -1.f, -1.f, 1.f);
		Colour co6(1.0f, 1.0f, 0.0f);
		Vector4 nor6(-1.f, -1.f, 1.f, 0.f);

		Vector4 vp7(1.f, -1.f, -1.f, 1.f);
		Colour co7(1.0f, 0.0f, 0.0f);
		Vector4 nor7(1.f, -1.f, 1.f, 0.f);

		Vector4 vp8(1.f, -1.f, 1.f, 1.f);
		Colour co8(0.0f, 1.0f, 0.0f);
		Vector4 nor8(1.f, -1.f, -1.f, 0.f);


		CGVertex vs[8] = {
			{vp1, co1, {0.f, 0.f}, nor1, 1.f },
			{vp2, co2, {0.f, 1.f}, nor2, 1.f },
			{vp3, co3, {1.f, 1.f}, nor3, 1.f },
			{vp4, co4, {0.f, 1.f}, nor4, 1.f },

			{vp5, co5, {1.f, 0.f}, nor5, 1.f },
			{vp6, co6, {0.f, 1.f}, nor6, 1.f },
			{vp7, co7, {1.f, 1.f}, nor7, 1.f },
			{vp8, co8, {1.f, 1.f}, nor8, 1.f },
		};

		DrawPlane(vs[6], vs[5], vs[4], vs[7]);
		DrawPlane(vs[2], vs[1], vs[5], vs[6]);
		DrawPlane(vs[5], vs[1], vs[0], vs[4]);
		DrawPlane(vs[2], vs[6], vs[7], vs[3]);
		DrawPlane(vs[7], vs[4], vs[0], vs[3]);
		DrawPlane(vs[0], vs[1], vs[2], vs[3]);
	}

	void Device::UpdateLightPosition()
	{
		// formula of finding a point on a globe learned in high school
		float x = sinf(MATH_PI - phi) * cosf(MATH_PI - theta);
		float z = sinf(MATH_PI - phi) * sinf(MATH_PI - theta);
		float y = cosf(MATH_PI - phi);

		gLightPosX = x;
		gLightPosY = y;
		gLightPosZ = z;
	}

	void Device::HalfSpaceTriangleRasterization(const CGVertex & v1, const CGVertex & v2, const CGVertex & v3,
		const Vector4& transformedVertPos1, const Vector4& transformedVertPos2, const Vector4& transformedVertPos3,
		const Vector4& transformedVertNormal1, const Vector4& transformedVertNormal2, const Vector4& transformedVertNormal3,
		const Vector4& homogenizedVertPos1, const Vector4& homogenizedVertPos2, const Vector4& homogenizedVertPos3)
	{
		Vector4 min, max;

		int s = GetState(m_state);
		min = homogenizedVertPos1; max = homogenizedVertPos1;

		if (homogenizedVertPos2.getX() < min.getX()) min.setX(homogenizedVertPos2.getX());
		if (homogenizedVertPos3.getX() < min.getX()) min.setX(homogenizedVertPos3.getX());
		if (homogenizedVertPos2.getY() < min.getY()) min.setY(homogenizedVertPos2.getY());
		if (homogenizedVertPos3.getY() < min.getY()) min.setY(homogenizedVertPos3.getY());

		if (homogenizedVertPos2.getX() > max.getX()) max.setX(homogenizedVertPos2.getX());
		if (homogenizedVertPos3.getX() > max.getX()) max.setX(homogenizedVertPos3.getX());
		if (homogenizedVertPos2.getY() > max.getY()) max.setY(homogenizedVertPos2.getY());
		if (homogenizedVertPos3.getY() > max.getY()) max.setY(homogenizedVertPos3.getY());

		CGVertex t1 = v1, t2 = v2, t3 = v3;
		t1.pos = homogenizedVertPos1; t2.pos = homogenizedVertPos2; t3.pos = homogenizedVertPos3;
		t1.normal = transformedVertNormal1; t2.normal = transformedVertNormal2; t3.normal = transformedVertNormal3;
		t1.pos.setW(transformedVertPos1.getW());
		t2.pos.setW(transformedVertPos2.getW());
		t3.pos.setW(transformedVertPos3.getW());

		VertexRHWInit(t1);
		VertexRHWInit(t2);
		VertexRHWInit(t3);

		for (int j = (int)min.getY(); j < (int)max.getY(); j++) {
			for (int i = (int)min.getX(); i < (int)max.getX(); i++) {
				Vector4 p((float)i, (float)j, 0.f, 1.f);
				float c1, c2, c3;
				if (GetTriangleInterp(homogenizedVertPos1, homogenizedVertPos2, homogenizedVertPos3, p, &c1, &c2)) {
					c3 = 1.f - c1 - c2;

					// Perspective rectification 
					float w = 1.f / (c1 * t1.rhw + c2 * t2.rhw + c3 * t3.rhw);

					float r = t1.color.r * c1 + t2.color.r * c2 + t3.color.r * c3;
					float g = t1.color.g * c1 + t2.color.g * c2 + t3.color.g * c3;
					float b = t1.color.b * c1 + t2.color.b * c2 + t3.color.b * c3;

					float u = t1.tex.u * c1 + t2.tex.u* c2 + t3.tex.u * c3;
					float v = t1.tex.v * c1 + t2.tex.v * c2 + t3.tex.v * c3;

					float nx = t1.normal.getX() * c1 + t2.normal.getX()* c2 + t3.normal.getX() * c3;
					float ny = t1.normal.getY() * c1 + t2.normal.getY()* c2 + t3.normal.getY() * c3;
					float nz = t1.normal.getZ() * c1 + t2.normal.getZ()* c2 + t3.normal.getZ() * c3;
					Vector3 normal(w*nx, w*ny, w*nz);
					normal.Normalize();

					float z = t1.pos.getZ() * t1.rhw * c1 + t2.pos.getZ() * t2.rhw * c2 + t3.pos.getZ() * t3.rhw * c3;
					p.setZ(w * z);

					Colour pointColor(w * r, w * g, w * b); // interpolated color that make the box colorful

					Texcoord texcoord = { w * u, w * v };
					DrawPoint(p, pointColor, texcoord, normal);
				}
			}
		}
	}


	void Device::ScanLineTriangleRasterization(const CGVertex & v1, const CGVertex & v2, const CGVertex & v3,
		const Vector4& transformedVertPos1, const Vector4& transformedVertPos2, const Vector4& transformedVertPos3,
		const Vector4& transformedVertNormal1, const Vector4& transformedVertNormal2, const Vector4& transformedVertNormal3,
		const Vector4& homogenizedVertPos1, const Vector4& homogenizedVertPos2, const Vector4& homogenizedVertPos3,
		const Colour** texture) {

		CGVertex t1 = v1, t2 = v2, t3 = v3;
		t1.pos = homogenizedVertPos1; t2.pos = homogenizedVertPos2; t3.pos = homogenizedVertPos3;
		t1.normal = transformedVertNormal1; t2.normal = transformedVertNormal2; t3.normal = transformedVertNormal3;
		t1.pos.setW(transformedVertPos1.getW());
		t2.pos.setW(transformedVertPos2.getW());
		t3.pos.setW(transformedVertPos3.getW());

		// init rhw to get the correct effect
		VertexRHWInit(t1);
		VertexRHWInit(t2);
		VertexRHWInit(t3);

		TriangleRasterization(t1, t2, t3, texture);
	}


	void Device::DrawPlaneForShadowShowing()
	{
		Matrix4f translationMatrix;
		Matrix4f scaleMatrix;

		if (m_scalePlaneForShadowShowing) {
			translationMatrix = Matrix4f::getTranslateMatrix(0.f, -3.25f, 0.5f);
			scaleMatrix = Matrix4f::getScaleMatrix(2.1f, 2.1f, 2.3f);
		}
		else {
			translationMatrix = Matrix4f::getTranslateMatrix(0.f, -2.25f, 0.f);
			scaleMatrix = Matrix4f::getScaleMatrix(1.f, 1.f, 1.f);
		}


		Transform::getInstance().worldMatrix = scaleMatrix * translationMatrix;
		Transform::getInstance().UpdateTransform();
		Transform::getInstance().lightSpaceMatrix = scaleMatrix * translationMatrix;
		Transform::getInstance().UpdateTransformForShadowMap();

		Vector4 vp1(-1.f, 1.f, 1.f, 1.f);
		Colour co1(1.0f, 0.0f, 0.0f);
		Vector4 nor1(-1.f, 1.f, -1.f, 0.f);

		Vector4 vp2(-1.f, 1.f, -1.f, 1.f);
		Colour co2(0.0f, 1.0f, 0.0f);
		Vector4 nor2(-1.f, 1.f, 1.f, 0.f);

		Vector4 vp3(1.f, 1.f, -1.f, 1.f);
		Colour co3(0.0f, 0.0f, 1.0f);
		Vector4 nor3(1.f, 1.f, 1.f, 0.f);

		Vector4 vp4(1.f, 1.f, 1.f, 1.f);
		Colour co4(1.0f, 1.0f, 0.0f);
		Vector4 nor4(1.f, 1.f, -1.f, 0.f);


		CGVertex vs[8] = {
			{vp1, co1, {0.f, 0.f}, nor1, 1.f },
			{vp2, co2, {0.f, 1.f}, nor2, 1.f },
			{vp3, co3, {1.f, 1.f}, nor3, 1.f },
			{vp4, co4, {0.f, 1.f}, nor4, 1.f }
		};

		DrawPlane(vs[0], vs[1], vs[2], vs[3]);
	}



	void Device::DrawModel(ModelInfo& model, const float theta, const Vector3& translationDeltaOnXYZ)
	{
		float xDelta = translationDeltaOnXYZ.getX();
		float yDelta = translationDeltaOnXYZ.getY();
		float zDelta = translationDeltaOnXYZ.getZ();

		Vector3 rotationAxisX(1.f, 0.f, 0.f);
		Vector3 rotationAxisY(0.f, 1.f, 0.f);
		Vector3 rotationAxisZ(0.f, 0.f, 1.f);

#pragma region Making the model at the center of the scene.
		Matrix4f mRot1 = Matrix4f::getRotationMatrix(rotationAxisX, 90.f);
		Matrix4f mRot2 = Matrix4f::getRotationMatrix(rotationAxisZ, 180.f);
		Matrix4f mRot3 = Matrix4f::getRotationMatrix(rotationAxisY, 180.f);
		Matrix4f translateOBJ = Matrix4f::getTranslateMatrix(0.f, -0.77f, 1.f);
#pragma endregion

		Matrix4f scaleMatrix = Matrix4f::getScaleMatrix(gModelScaleFactor, gModelScaleFactor, gModelScaleFactor);

		Matrix4f translateOBJFromOutside = Matrix4f::getTranslateMatrix(xDelta, yDelta, zDelta);
		Transform::getInstance().worldMatrix = (scaleMatrix*mRot1*mRot2*mRot3*m_rotationFromOutsideInteraction*translateOBJ*translateOBJFromOutside);

		Transform::getInstance().UpdateTransform();

		Transform::getInstance().lightSpaceMatrix = (mRot1*mRot2*mRot3*m_rotationFromOutsideInteraction*translateOBJ*translateOBJFromOutside);
		Transform::getInstance().UpdateTransformForShadowMap();


		for (int i = 0; i < model.verteciesIndexVec.size(); i++)
		{
			int index0 = model.verteciesIndexVec[i].index[0] - 1;
			int index1 = model.verteciesIndexVec[i].index[1] - 1;
			int index2 = model.verteciesIndexVec[i].index[2] - 1;

			if (!m_showShadowBuffer)
				DrawTriangleForShadowMap(model.vertexVec[index0], model.vertexVec[index1], model.vertexVec[index2]);

			const Colour** modelTexture = (const Colour**)pixelColorData;
			DrawTriangle(model.vertexVec[index0], model.vertexVec[index1], model.vertexVec[index2], modelTexture);
		}
	}

	void Device::UpdateModelRotation(const Vector2& lastPointV2, const Vector2& curPointV2)
	{
		Vector4 quat = GetQuaternion(lastPointV2, curPointV2);
		Matrix3f currentRotation = Matrix3f::getRotationMatrixFromQuaternion(quat);
		m_rotationFromOutsideInteraction = m_rotationFromOutsideInteraction * Matrix4f(currentRotation);
	}

	void Device::DrawTriangleForShadowMap(CGVertex & v1, CGVertex & v2, CGVertex & v3)
	{
		Vector4 pos1AfterMVP, pos2AfterMVP, pos3AfterMVP;
		Vector4 transformedVertNormal1, transformedVertNormal2, transformedVertNormal3;

#ifdef MVP_DONE_IN_ONE_TIME
		m_transform->ApplyLightTransform(pos1AfterMVP, v1.pos);
		m_transform->ApplyLightTransform(pos2AfterMVP, v2.pos);
		m_transform->ApplyLightTransform(pos3AfterMVP, v3.pos);
#else

		Vector4 pos1TransformedToLightSpace, pos2TransformedToLightSpace, pos3TransformedToLightSpace;
		m_transform->ModelToLightSpace(pos1TransformedToLightSpace, v1.pos);
		m_transform->ModelToLightSpace(pos2TransformedToLightSpace, v2.pos);
		m_transform->ModelToLightSpace(pos3TransformedToLightSpace, v3.pos);

		Vector4 pos1InView, pos2InView, pos3InView;
		Vector4DotMatrix4fInDeviceContext(pos1InView, pos1TransformedToLightSpace, m_transform->lightViewMatrix);
		Vector4DotMatrix4fInDeviceContext(pos2InView, pos2TransformedToLightSpace, m_transform->lightViewMatrix);
		Vector4DotMatrix4fInDeviceContext(pos3InView, pos3TransformedToLightSpace, m_transform->lightViewMatrix);

		Vector4DotMatrix4fInDeviceContext(pos1AfterMVP, pos1InView, m_transform->orthographicProjectionMatrix);
		Vector4DotMatrix4fInDeviceContext(pos2AfterMVP, pos2InView, m_transform->orthographicProjectionMatrix);
		Vector4DotMatrix4fInDeviceContext(pos3AfterMVP, pos3InView, m_transform->orthographicProjectionMatrix);

#endif

		if (m_transform->IsOutsideCVV(pos1AfterMVP)
			&& m_transform->IsOutsideCVV(pos2AfterMVP)
			&& m_transform->IsOutsideCVV(pos3AfterMVP)) return;

		m_transform->ModelToLightSpace(transformedVertNormal1, v1.normal);
		m_transform->ModelToLightSpace(transformedVertNormal2, v2.normal);
		m_transform->ModelToLightSpace(transformedVertNormal3, v3.normal);

		Vector4 homogenizedVertPos1, homogenizedVertPos2, homogenizedVertPos3;

		/// Set projection transform ---> To NDC
		m_transform->Homogenize(homogenizedVertPos1, pos1AfterMVP);
		m_transform->Homogenize(homogenizedVertPos2, pos2AfterMVP);
		m_transform->Homogenize(homogenizedVertPos3, pos3AfterMVP);

		CGVertex t1 = v1, t2 = v2, t3 = v3;
		t1.pos = homogenizedVertPos1; t2.pos = homogenizedVertPos2; t3.pos = homogenizedVertPos3;
		t1.normal = transformedVertNormal1; t2.normal = transformedVertNormal2; t3.normal = transformedVertNormal3;
		t1.pos.setW(pos1AfterMVP.getW());
		t2.pos.setW(pos2AfterMVP.getW());
		t3.pos.setW(pos3AfterMVP.getW());

		// init rhw to get the correct effect
		VertexRHWInit(t1);
		VertexRHWInit(t2);
		VertexRHWInit(t3);

		TriangleRasterization(t1, t2, t3);
	}


	void Device::RasterTriangleInAnotherWay(const CGVertex& t1, const CGVertex& t2, const CGVertex& t3)
	{
		Vector4 homogenizedPos1, homogenizedPos2, homogenizedPos3;

		m_transform->Homogenize(homogenizedPos1, t1.pos);
		m_transform->Homogenize(homogenizedPos2, t2.pos);
		m_transform->Homogenize(homogenizedPos3, t3.pos);

		// Perspective correction to be done
	
		float total_height = homogenizedPos3.getY() - homogenizedPos1.getY();
		// plus 0.5 for rounding
		for (int y = (int)(homogenizedPos1.getY() + 0.5f); y <= (int)(homogenizedPos2.getY() + 0.5f); y++)
		{
			// Because of up set down Y. Add a checkpoint here.
			if (y >= m_height || y < 0) continue;

			float segement_height = homogenizedPos2.getY() - homogenizedPos1.getY() + (float)EPSILON;
			//if(segement_height < 1.0f) continue;
			float alpha = (float)(y - homogenizedPos1.getY()) / total_height;
			float beta = (float)(y - homogenizedPos1.getY()) / segement_height;
			
			CGVertex A = LerpInDeviceContext(t1, t2, alpha);
			CGVertex B = LerpInDeviceContext(t1, t2, beta);

			//draw line from left to right
			if (A.pos.getX() > B.pos.getX())
			{
				std::swap(A, B);
			}

			DrawScanline(A, B, y);
		}
		// plus 0.5 for rounding
		for (int y = (int)(homogenizedPos2.getY() + 0.5f); y <= (int)(homogenizedPos3.getY() + 0.5f); y++)
		{
			// Because of up set down Y. Add a checkpoint here.
			if (y >= m_height || y < 0) continue;

			
			float segement_height = homogenizedPos3.getY() - homogenizedPos2.getY() + (float)EPSILON;
			//if (segement_height < 1.0f) continue;
			float alpha = (float)(y - homogenizedPos1.getY()) / total_height;
			float beta = (float)(y - homogenizedPos2.getY()) / segement_height;
			//vertex lerp
			CGVertex A = LerpInDeviceContext(t1, t3, alpha);
			CGVertex B = LerpInDeviceContext(t2, t3, beta);
			//draw line from left to right
			if (A.pos.getX() > B.pos.getX())
			{
				std::swap(A, B);
			}

			DrawScanline(A, B, y);
		}
	}


	void Device::DrawScanline(const CGVertex& A, const CGVertex& B, int y)
	{
		Vector4 worldpos_A = A.posInWorldSpace;
		Vector4 worldpos_B = B.posInWorldSpace;

		Vector4 campos_A = A.pos;
		Vector4 campos_B = B.pos;

		Colour color_A = A.color;
		Colour color_B = B.color;

		Texcoord texture_A = A.tex;
		Texcoord texture_B = B.tex;

		Vector4 normal_A = A.normal;
		Vector4 normal_B = B.normal;

		//zbuffer caculation
		float scanline_depth = campos_B.getZ() - campos_A.getZ();
		float scanline_width = campos_B.getX() - campos_A.getX();
		float depth_ratio = scanline_depth / scanline_width;
		//float z_line_buffer = m_zbuffer[y]; 

		//w caculation
		float w_diff = campos_B.getW() - campos_A.getW();
		float w_ratio = w_diff / scanline_width;

		//world pos caculation
		Vector4 world_diff = worldpos_B - worldpos_A;
		Vector4 worldpos_ratio = world_diff / scanline_width;

		//color caculation
		Colour color_diff = color_B - color_A;
		Colour color_ratio = color_diff / scanline_width;

		//UV caculation
		Vector2 uv_diff = Vector2(texture_B.u - texture_A.u, texture_B.v - texture_A.v);
		Vector2 uv_ratio = uv_diff / scanline_width;

		//normal caculation
		Vector4 norm_diff = normal_B - normal_A;
		Vector4 norm_ratio = norm_diff / scanline_width;

		for (int j = (int)(campos_A.getX() + 0.5f); j < (int)(campos_B.getX() + 0.5f); ++j)
		{
			float step = (float)j - campos_A.getX() + 0.5f;
			if (j >= m_width || j < 0) continue;
			float z = depth_ratio * step + campos_A.getZ();
			float w = w_ratio * step + campos_A.getW();
			// Z test
			if (m_zbuffer[y*m_width + j] > z)
			{
				Colour color;
				color = (color_ratio * step + color_A);
				color = color / w;

				Vector4 normal;
				normal = (norm_ratio * step + normal_A);
				normal = normal / w;

				Vector4 world = (worldpos_ratio * step + worldpos_A);
				world = world / w;

				//Vector2 uv;
				//uv = (uv_ratio * step + Vector2(texture_A.u, texture_A.v));
				//uv = uv / w;

				// light caculation
				//CaculateLightColor(world, normal, uv, color);

				// Z write
				m_zbuffer[y*m_width + j] = z;

				//if (TestVertexInShadow(world, normal))
				//{
				//	float shadow = scnManager->GetCurrentLight()->GetAmbient();
				//	color = color * Color(shadow, shadow, shadow);
				//}

				//AlphaBlending(j, y, color);

				m_framebuffer[j][y] = GetHEXColor(color);
			}
		}
	}


	void Device::ScanlineFill(const CGVertex & leftPoint, const  CGVertex & rightPoint, const int yIndex, const Colour** texture)
	{
		float lineWidth = rightPoint.pos.getX() - leftPoint.pos.getX();
		float step = 1.f;
		if (lineWidth > 0.f) step = 1.f / lineWidth;
		else return;

		for (float x = leftPoint.pos.getX(); x <= rightPoint.pos.getX(); x++)
		{
			int xIndex = (int)(x + 0.5f);
			float lerpFactor = (x - leftPoint.pos.getX()) / lineWidth;
			float zValue = CleanLerp(leftPoint.pos.getZ(), rightPoint.pos.getZ(), lerpFactor);
			if (xIndex >= 0 && xIndex < m_width && zValue > 0.f && zValue < 1.f) // clip again
			{
				float rhwTemp = LerpInDeviceContext(leftPoint.rhw, rightPoint.rhw, lerpFactor);

				if (texture == NULL || m_showShadowBuffer)
				{// when gaining shadow map or presenting it on screen
					// Perpective correction! ---> useless in orthographic mode
					float w = 1.f / rhwTemp;

					// uv interpolation to get the texture color
					float u = LerpInDeviceContext(leftPoint.tex.u, rightPoint.tex.u, lerpFactor) * w * (textureWidth - 1);
					float v = LerpInDeviceContext(leftPoint.tex.v, rightPoint.tex.v, lerpFactor) * w * (textureHeight - 1);

					int uIndex = (int)(u + 0.5f); // maybe this is more effective than (int)roundf(fNum) 
					int vIndex = (int)(v + 0.5f);

					float& depthInBufferFromLightPos = depthBufferFromLightPos[yIndex * m_width + xIndex];

					if (zValue < depthInBufferFromLightPos)
					{
						depthInBufferFromLightPos = zValue; // write into z-buffer

						if (m_showShadowBuffer) {
							if (gRenderInPerspectiveMode)
								m_framebuffer[yIndex][xIndex] = GetHEXColor(GetBrightnessViaGamaCorrection(depthInBufferFromLightPos));
							else
								m_framebuffer[yIndex][xIndex] = GetHEXColor(GetGrayValueViaGamaCorrection(depthInBufferFromLightPos));
						}
					}
				}
				else
				{
					float& zInBuffer = m_zbuffer[yIndex * m_width + xIndex];

					if (zValue < zInBuffer)
					{
						// Perpective correction! ---> useless in orthographic mode
						float w = 1.f / rhwTemp;

						zInBuffer = zValue; // write into z-buffer

						// uv interpolation to get the texture color (multiplies w to gain perspective correction of uv-texcture)
						float u = LerpInDeviceContext(leftPoint.tex.u, rightPoint.tex.u, lerpFactor) * w * (textureWidth - 1);
						float v = LerpInDeviceContext(leftPoint.tex.v, rightPoint.tex.v, lerpFactor) * w * (textureHeight - 1);

						int uIndex = (int)(u + 0.5f); // maybe this is more effective than (int)roundf(fNum) 
						int vIndex = (int)(v + 0.5f);

						Colour vertexColor(1.f, 1.f, 1.f);

						if (uIndex <= textureWidth - 1 && uIndex >= 0
							&& vIndex <= textureHeight - 1 && vIndex >= 0) {
							if (mLightOn)
								vertexColor = LerpInDeviceContext(leftPoint.color, rightPoint.color, lerpFactor);

							Colour texColor;
							if (m_interp == INTERP_BILINEAR) {
								Colour** m_texture = (Colour**)texture;
								texColor = BilinearInterp(m_texture, textureWidth, textureHeight, u, v);
							}
							else {
								texColor = texture[uIndex][vIndex];
							}

							if (m_useInvertMethod2GetWorldPos) {
								posOnScreen = Vector4((float)xIndex, (float)yIndex, zValue, w);
								invertedHomogenizedPos = m_transform->HomogenizeInvertion(invertedHomogenizedPos, posOnScreen);
								Vector4DotMatrix4fInDeviceContext(posInWorldSpace, invertedHomogenizedPos, m_invertedMatrix);
								if (posInWorldSpace.getW() - 1.f != EPSILON)
									posInWorldSpace = posInWorldSpace / posInWorldSpace.getW();
							}
							else
							{
								float xHere = LerpInDeviceContext(leftPoint.posInWorldSpace.getX(), rightPoint.posInWorldSpace.getX(), lerpFactor);
								float yHere = LerpInDeviceContext(leftPoint.posInWorldSpace.getY(), rightPoint.posInWorldSpace.getY(), lerpFactor);
								float zHere = LerpInDeviceContext(leftPoint.posInWorldSpace.getZ(), rightPoint.posInWorldSpace.getZ(), lerpFactor);
								posInWorldSpace = Vector4(xHere, yHere, zHere, 1.f);
							}

							Vector4DotMatrix4fInDeviceContext(lightScreenPosBeforsHomogenized, posInWorldSpace, lightSpaceMatrix);
							m_transform->Homogenize(posInLightScreen, lightScreenPosBeforsHomogenized);

							int hX = (int)(posInLightScreen.getX() + 0.5f);
							int hY = (int)(posInLightScreen.getY() + 0.5f);

							float bias = GetBiasDynamically(biasDelta,rightPoint.normal, leftPoint.normal, lerpFactor);

							if (hX > 0 && hY > 0 && hX < m_width && hY < m_height) {
								if (posInLightScreen.getZ() - bias > depthBufferFromLightPos[hY * m_width + hX])
									texColor *= Colour(0.3f, 0.3f, 0.3f);
							}

							if (m_showPosInLightScreenDepth) { // for test
								if (posInLightScreen.getZ() < 1.f || abs(posInLightScreen.getZ()) > EPSILON) {
									m_framebuffer[yIndex][xIndex] = GetHEXColor(GetGrayValueViaGamaCorrection(posInLightScreen.getZ()));
								}
							}
							else {
								m_framebuffer[yIndex][xIndex] = GetHEXColor(vertexColor * texColor);
							}
						}
					}
				}
			}
		}
	}

	void Device::UpdateLightSpaceMatrix()
	{
		lightSpaceMatrix = m_transform->lightViewMatrix * m_transform->orthographicProjectionMatrix;
	}

	static float experiencedBias = 0.0003f;
	float Device::GetBiasDynamically(const float initialBias, const Vector4 rightPointNormal, const  Vector4 leftPointNormal, const float lerpFactor)
	{
		Vector4 lightDirection(gLightPosX, gLightPosY, gLightPosZ);
		Vector4 faceNormal = LerpInDeviceContext(rightPointNormal, leftPointNormal, lerpFactor);
		float dotNormalLightDir = faceNormal * lightDirection;
		float bias = max(experiencedBias * (1.f - dotNormalLightDir), initialBias); // defined max bias and min bias
		return bias;
	}

	void Device::TriangleRasterization(const CGVertex & p1, const CGVertex & p2, const CGVertex & p3, const Colour** texture)
	{
		if (p1.pos.getY() == p2.pos.getY())
		{
			if (p1.pos.getY() < p3.pos.getY())
			{// Flat top
				DrawTopTriangle(p1, p2, p3, texture);
			}
			else
			{// Flat bottom
				DrawBottomTriangle(p3, p1, p2, texture);
			}
		}
		else if (p1.pos.getY() == p3.pos.getY())
		{
			if (p1.pos.getY() < p2.pos.getY())
			{// Flat top
				DrawTopTriangle(p1, p3, p2, texture);
			}
			else
			{// Flat bottom
				DrawBottomTriangle(p2, p1, p3, texture);
			}
		}
		else if (p2.pos.getY() == p3.pos.getY())
		{
			if (p2.pos.getY() < p1.pos.getY())
			{// Flat top
				DrawTopTriangle(p2, p3, p1, texture);
			}
			else
			{// Flat bottom
				DrawBottomTriangle(p1, p2, p3, texture);
			}
		}
		else
		{// Segment the triangle 
			CGVertex top;

			CGVertex bottom;
			CGVertex middle;
			if (p1.pos.getY() > p2.pos.getY() && p2.pos.getY() > p3.pos.getY())
			{
				top = p3;
				middle = p2;
				bottom = p1;
			}
			else if (p3.pos.getY() > p2.pos.getY() && p2.pos.getY() > p1.pos.getY())
			{
				top = p1;
				middle = p2;
				bottom = p3;
			}
			else if (p2.pos.getY() > p1.pos.getY() && p1.pos.getY() > p3.pos.getY())
			{
				top = p3;
				middle = p1;
				bottom = p2;
			}
			else if (p3.pos.getY() > p1.pos.getY() && p1.pos.getY() > p2.pos.getY())
			{
				top = p2;
				middle = p1;
				bottom = p3;
			}
			else if (p1.pos.getY() > p3.pos.getY() && p3.pos.getY() > p2.pos.getY())
			{
				top = p2;
				middle = p3;
				bottom = p1;
			}
			else if (p2.pos.getY() > p3.pos.getY() && p3.pos.getY() > p1.pos.getY())
			{
				top = p1;
				middle = p3;
				bottom = p2;
			}
			else
			{
				// 3 points colinear
				return;
			}

			// get middle X by interpolation, get coeffiecient-t first
			float middleX = (middle.pos.getY() - top.pos.getY()) * (bottom.pos.getX() - top.pos.getX()) / (bottom.pos.getY() - top.pos.getY()) + top.pos.getX();
			float dy = middle.pos.getY() - top.pos.getY();
			float t = dy / (bottom.pos.getY() - top.pos.getY());

			// get the right and left point via interpolation
			CGVertex newMiddle;
			newMiddle.pos.setX(middleX);
			newMiddle.pos.setY(middle.pos.getY());
			newMiddle.normal = middle.normal;
			ScreenSpaceLerpCGVertex(newMiddle, top, bottom, t);

			DrawBottomTriangle(top, newMiddle, middle, texture);
			DrawTopTriangle(newMiddle, middle, bottom, texture);
		}
	}

	void Device::ScreenSpaceLerpCGVertex(CGVertex & v, const CGVertex & v1, const CGVertex & v2, float t)
	{
		//optimized below ---> using the inline Lerp functions rather than the one from Math.h
		v.rhw = LerpInDeviceContext(v1.rhw, v2.rhw, t);
		v.tex.u = LerpInDeviceContext(v1.tex.u, v2.tex.u, t);
		v.tex.v = LerpInDeviceContext(v1.tex.v, v2.tex.v, t);
		v.color = LerpInDeviceContext(v1.color, v2.color, t);
		v.pos = LerpInDeviceContext(v1.pos, v2.pos, t);
		v.normal = LerpInDeviceContext(v1.normal, v2.normal, t); // comment this line to stay normal in world space
		v.posInWorldSpace = LerpInDeviceContext(v1.posInWorldSpace, v2.posInWorldSpace, t);
	}

	void Device::DrawTopTriangle(const CGVertex & p1, const CGVertex & p2, const CGVertex & p3, const Colour** texture)
	{
		for (float y = p1.pos.getY(); y <= p3.pos.getY() && y >= 0 && y < m_height; y += 0.5f)
		{
			int yIndex = (int)roundf(y);
			if (yIndex >= 0 && yIndex < m_height)
			{
				float xl = (y - p1.pos.getY()) * (p3.pos.getX() - p1.pos.getX()) / (p3.pos.getY() - p1.pos.getY()) + p1.pos.getX();
				float x2 = (y - p2.pos.getY()) * (p3.pos.getX() - p2.pos.getX()) / (p3.pos.getY() - p2.pos.getY()) + p2.pos.getX();

				float dy = y - p1.pos.getY();
				float t = dy / (p3.pos.getY() - p1.pos.getY());

				// get the right and left point via interpolation
				CGVertex new1;
				new1.pos.setX(xl);
				new1.pos.setY(y);
				ScreenSpaceLerpCGVertex(new1, p1, p3, t);

				CGVertex new2;
				new2.pos.setX(x2);
				new2.pos.setY(y);
				ScreenSpaceLerpCGVertex(new2, p2, p3, t);

				if (new1.pos.getX() < new2.pos.getX())
				{
					ScanlineFill(new1, new2, yIndex, texture);
				}
				else
				{
					ScanlineFill(new2, new1, yIndex, texture);
				}
			}
		}
	}

	void Device::DrawBottomTriangle(const CGVertex & p1, const CGVertex & p2, const CGVertex & p3, const Colour** texture)
	{
		for (float y = p1.pos.getY(); y <= p2.pos.getY() && y >= 0 && y < m_height; y += 0.5f)
		{
			int yIndex = (int)roundf(y);
			if (yIndex >= 0 && yIndex < m_height)
			{
				float xl = (y - p1.pos.getY()) * (p2.pos.getX() - p1.pos.getX()) / (p2.pos.getY() - p1.pos.getY()) + p1.pos.getX();
				float x2 = (y - p1.pos.getY()) * (p3.pos.getX() - p1.pos.getX()) / (p3.pos.getY() - p1.pos.getY()) + p1.pos.getX();

				float dy = y - p1.pos.getY();
				float t = dy / (p2.pos.getY() - p1.pos.getY());

				// get the right and left point via interpolation
				CGVertex new1;
				new1.pos.setX(xl);
				new1.pos.setY(y);
				ScreenSpaceLerpCGVertex(new1, p1, p2, t);

				CGVertex new2;
				new2.pos.setX(x2);
				new2.pos.setY(y);
				ScreenSpaceLerpCGVertex(new2, p1, p3, t);

				if (new1.pos.getX() < new2.pos.getX())
				{
					ScanlineFill(new1, new2, yIndex, texture);
				}
				else
				{
					ScanlineFill(new2, new1, yIndex, texture);
				}
			}
		}
	}



	void Device::RenderUpdate(ModelInfo& objModel)
	{
		enum CameraLookingDirection {
			FrontBack, TopDown, BackFront, DownTop
		};
		static CameraLookingDirection gCameraLookingFrom = CameraLookingDirection::FrontBack;

#if _DEBUG
		CodesForTest();
#endif // _DEBUG


		float movOnX = 0.f; // CAMERA_POS_X;
		float movOnY = 0.f; // CAMERA_POS_Y;
		float rotationAngleInDegree = 1.f;
		float lightRotation = 1.f;

		float translateX = 0.f;
		float translateY = 0.f;
		float translateZ = 0.f;
		Vector3 translationForModel;

		Matrix4f rotationFromOutsideInteration = Matrix4f::getIdentityMatrix();


		while (!Window::getInstance().shouldExit())
		{
			Vector3 cameraPos(movOnX, movOnY, gDistanceFactor);
			Vector3 lookAtForCamera(LOOK_AT_POS_X + movOnX, LOOK_AT_POS_Y + movOnY, LOOK_AT_POS_Z);
			Vector3 lookAtForLight(LOOK_AT_POS_X, LOOK_AT_POS_Y, LOOK_AT_POS_Z);
			Vector3 cameraUpAxis(0.f, 1.f, 0.f);

			ClearBuffers();
			Window::getInstance().Dispatch();

			if (!m_showShadowBuffer) {
				switch (gCameraLookingFrom) {
				case CameraLookingDirection::FrontBack:
					Transform::getInstance().SetCamera(cameraPos, lookAtForCamera, cameraUpAxis);
					break;
				case CameraLookingDirection::TopDown:
					Transform::getInstance().SetCamera(Vector3(0.f, gDistanceFactor, 0.f), lookAtForLight, Vector3(0.f, 0.f, 1.f));
					break;
				case CameraLookingDirection::BackFront:
					Transform::getInstance().SetCamera(Vector3(0.f, 0.f, -gDistanceFactor), lookAtForLight, Vector3(0.f, 1.f, 0.f));
					break;
				case CameraLookingDirection::DownTop:
					Transform::getInstance().SetCamera(Vector3(0.f, -gDistanceFactor, 0.f), lookAtForLight, Vector3(0.f, 0.f, 1.f));
					break;
				default:
					break;
				}

				Transform::getInstance().SetLightAsCamera(Vector3(gLightPosX, gLightPosY, gLightPosZ), lookAtForLight, cameraUpAxis);
				Transform::getInstance().SetLightAsCamera(Vector3(gLightPosX, gLightPosY, gLightPosZ), lookAtForLight, cameraUpAxis);
			}
			else {
				Transform::getInstance().SetCamera(Vector3(gLightPosX, gLightPosY, gLightPosZ), lookAtForLight, cameraUpAxis);
			}


			// Translate the molde right
			if (Window::getInstance().IsKeyPressed(VK_A))
			{
				if (translateX <= 27) // limit:27 is magic number
				{
					translateX += 0.1f;
					translationForModel = Vector3(translateX, translateY, translateZ);
				}
			}

			// Translate the molde left
			if (Window::getInstance().IsKeyPressed(VK_D))
			{
				if (translateX >= -27) // limit:-27 is magic number
				{
					translateX -= 0.1f;
					translationForModel = Vector3(translateX, translateY, translateZ);
				}
			}

			// Translate the molde going far away from the user
			if (Window::getInstance().IsKeyPressed(VK_W))
			{
				if (translateZ >= -7) // limit:-7 is magic number
				{
					translateZ -= 0.1f;
					translationForModel = Vector3(translateX, translateY, translateZ);
				}
			}

			// Translate the molde going near to the user
			if (Window::getInstance().IsKeyPressed(VK_S))
			{
				if (translateZ <= 7) // limit:7 is magic number
				{
					translateZ += 0.1f;
					translationForModel = Vector3(translateX, translateY, translateZ);
				}
			}

			// Translate the molde up
			if (Window::getInstance().IsKeyPressed(VK_UP))
			{
				if (translateY <= 7) // limit:7 is magic number
				{
					translateY += 0.1f;
					translationForModel = Vector3(translateX, translateY, translateZ);
				}
			}

			// Translate the molde down
			if (Window::getInstance().IsKeyPressed(VK_DOWN))
			{
				if (translateY >= -13) // limit:-13 is magic number
				{
					translateY -= 0.1f;
					translationForModel = Vector3(translateX, translateY, translateZ);
				}
			}
			if (Window::getInstance().IsKeyPressed(VK_RIGHT))
			{
				// move camera on X axis
				movOnX -= 0.05f;
			}
			if (Window::getInstance().IsKeyPressed(VK_LEFT))
			{
				// move camera on X axis
				movOnX += 0.05f;
			}

			// Reset the model to position O
			if (Window::getInstance().GetKeyUpEvent(VK_SPACE))
			{
				translationForModel = Vector3(0.f, 0.f, 0.f);
			}


			///////////////////////////Instructions of interaction./////////////////////////////
			/*
			 * Space:	  Make the transform of the model to its initial place, rotation excluded.
			 * O:		  Lower the light intensity;
			 * P:		  Raise the light intensity;
			 * J:		  Change the cull mode of the model rendering;
			 * K:		  Change ineterpolation mode while drawing triangles with Half Space Rasterization Merthod;
			 * L:		  Turn on/off the directional light;
			 * Y:		  Turn on/off the rendering from the light position;
			 * Q:		  Render in perspective mode or in orthographic mode;
			 * R:		  Change the background color ramping mode;
			 * H:		  Raise the value of biasDelta;
			 * G:		  Lower the value of biasDelta;
			 * M:		  Scale the plane for shadow showing;
			 * V:		  Use invert-matrix-method to get the world position for shadowing;
			 * F:		  Show position in light screen depth;
			 * NUMPAD2:	  Scale the model to smaller;
			 * NUMPAD8:	  Scale the model to bigger;
			 * NUMPAD7:	  Camera looking from top to down;
			 * NUMPAD4:	  Camera looking from down to top;
			 * NUMPAD0:	  Camera looking from front to back;
			 * NUMPAD9:	  Camera looking from back to front;
			 * NUMPAD_ADD:		  Change the bias delta to NO-SELF-SHADOW mode;
			 * NUMPAD_SUBTRACT:	  Restore the bias delta to original value;
			 */

			if (Window::getInstance().GetKeyUpEvent(VK_F1))
				SetState(4);
			if (Window::getInstance().GetKeyUpEvent(VK_F2))
				SetState(3);
			if (Window::getInstance().GetKeyUpEvent(VK_F3))
				SetState(6);
			if (Window::getInstance().GetKeyUpEvent(VK_F4))
				SetState(5);
			if (Window::getInstance().GetKeyUpEvent(VK_F5))
				SetState(2);
			if (Window::getInstance().GetKeyUpEvent(VK_F6))
				SetState(1);

			if (Window::getInstance().GetKeyUpEvent(VK_J))
				ChangeInterp();
			if (Window::getInstance().GetKeyUpEvent(VK_K))
				ChangeCullMode();
			if (Window::getInstance().GetKeyUpEvent(VK_L))
				mLightOn = !mLightOn;

			if (Window::getInstance().IsKeyPressed(VK_O))
				gDirectionalLightIntensity - EPSILON <= 0.f ? (gDirectionalLightIntensity = 0.f) : gDirectionalLightIntensity -= 0.01f;
			else if (Window::getInstance().IsKeyPressed(VK_P))
				gDirectionalLightIntensity - EPSILON >= 1.f ? (gDirectionalLightIntensity = 1.f) : gDirectionalLightIntensity += 0.01f;

			if (Window::getInstance().GetKeyUpEvent(VK_Y))
				m_showShadowBuffer = !m_showShadowBuffer;

			if (Window::getInstance().GetKeyUpEvent(VK_Q))
				gRenderInPerspectiveMode = !gRenderInPerspectiveMode;

			if (Window::getInstance().GetKeyUpEvent(VK_R))
				gradientRampedBackgroundColor = !gradientRampedBackgroundColor;

			if (Window::getInstance().IsKeyPressed(VK_G))
			{
				biasDelta -= 0.00001f;
				$log(biasDelta);
			}
			if (Window::getInstance().IsKeyPressed(VK_H))
			{
				biasDelta += 0.00001f;
				$log(biasDelta);
			}

			
			if (Window::getInstance().GetKeyUpEvent(VK_M))
			{
				m_scalePlaneForShadowShowing = !m_scalePlaneForShadowShowing;
				m_scalePlaneForShadowShowing ?
					std::cout << "plane for shadowing is scaled big!" << std::endl : std::cout << "plane for shadowing is scaled small!" << std::endl;
			}
			if (Window::getInstance().GetKeyUpEvent(VK_V))
			{
				m_useInvertMethod2GetWorldPos = !m_useInvertMethod2GetWorldPos;
				m_useInvertMethod2GetWorldPos ?
					std::cout << "using invert-matrix-method to get world position!" << std::endl : std::cout << "using vertex-property to get world position!" << std::endl;
			}
			if (Window::getInstance().GetKeyUpEvent(VK_F))
			{
				m_showPosInLightScreenDepth = !m_showPosInLightScreenDepth;
			}
			if (Window::getInstance().IsKeyPressed(VK_NUMPAD2))
			{
				// scale model via scale matrix
				if (gModelScaleFactor >= 0.15f) gModelScaleFactor -= 0.05f;
			}
			if (Window::getInstance().IsKeyPressed(VK_NUMPAD8))
			{
				// scale model via scale matrix
				if (gModelScaleFactor <= 1.90f) gModelScaleFactor += 0.05f;
			}
			if (Window::getInstance().IsKeyPressed(VK_NUMPAD9))
			{
				gCameraLookingFrom = CameraLookingDirection::BackFront;
			}
			if (Window::getInstance().IsKeyPressed(VK_NUMPAD0))
			{
				gCameraLookingFrom = CameraLookingDirection::FrontBack;
			}
			if (Window::getInstance().IsKeyPressed(VK_NUMPAD7))
			{
				gCameraLookingFrom = CameraLookingDirection::TopDown;
			}
			if (Window::getInstance().IsKeyPressed(VK_NUMPAD4))
			{
				gCameraLookingFrom = CameraLookingDirection::DownTop;
			}
			if (Window::getInstance().GetKeyUpEvent(VK_ADD))
			{
				if (biasDelta != 0.0013f) {
					$log("Bias delta has been changed! BiasDelta:" << biasDelta);
					biasDeltaStorage = biasDelta;
					biasDelta = 0.0013f;
				}
			}
			if (Window::getInstance().GetKeyUpEvent(VK_SUBTRACT))
			{
				if (biasDeltaStorage != 0.f && biasDelta == 0.0013f) {
					$log("Bias delta has been restored! BiasDelta:" << biasDelta);
					biasDelta = biasDeltaStorage;
				}
			}

			int s = GetState(m_state);
			if (s & DRAW_MODEL)
			{
				DrawModel(objModel, rotationAngleInDegree, translationForModel);
				DrawPlaneForShadowShowing();
				//DrawOriginalPos();
				//DrawLightPositionAsABox(Vector3(gLightPosX, gLightPosY, gLightPosZ));
			}
			else
			{
				DrawBox(rotationAngleInDegree);
			}

			Window::getInstance().Dispatch();
			Window::getInstance().Update();
			Window::getInstance().ShowFPSInfo();

			Sleep(1);
		}
	}


	void Device::CodesForTest() { }

	void Device::DrawOriginalPos()
	{
		Matrix4f translationMatrix;
		translationMatrix = Matrix4f::getTranslateMatrix(2.3f, 1.f, 0.f);
		Matrix4f scaleMatrix = Matrix4f::getScaleMatrix(0.1f, 0.1f, 0.1f);
		Matrix4f influencialMatrix = scaleMatrix * translationMatrix;

		Transform::getInstance().worldMatrix = (influencialMatrix);

		Transform::getInstance().UpdateTransform();

		Vector4 vp1(-0.05f, 0.05f, 0.05f, 0.05f);
		Colour co1(0.05f, 0.0f, 0.0f);
		Vector4 nor1(-0.05f, 0.05f, -0.05f, 0.f);

		Vector4 vp2(-0.05f, 0.05f, -0.05f, 0.05f);
		Colour co2(0.0f, 0.05f, 0.0f);
		Vector4 nor2(-0.05f, 0.05f, 0.05f, 0.f);

		Vector4 vp3(0.05f, 0.05f, -0.05f, 0.05f);
		Colour co3(0.0f, 0.0f, 0.05f);
		Vector4 nor3(0.05f, 0.05f, 0.05f, 0.f);

		Vector4 vp4(0.05f, 0.05f, 0.05f, 0.05f);
		Colour co4(0.05f, 0.05f, 0.0f);
		Vector4 nor4(0.05f, 0.05f, -0.05f, 0.f);

		Vector4 vp5(-0.05f, -0.05f, 0.05f, 0.05f);
		Colour co5(0.0f, 0.0f, 0.05f);
		Vector4 nor5(-0.05f, -0.05f, -0.05f, 0.f);

		Vector4 vp6(-0.05f, -0.05f, -0.05f, 0.05f);
		Colour co6(0.05f, 0.05f, 0.0f);
		Vector4 nor6(-0.05f, -0.05f, 0.05f, 0.f);

		Vector4 vp7(0.05f, -0.05f, -0.05f, 0.05f);
		Colour co7(0.05f, 0.0f, 0.0f);
		Vector4 nor7(0.05f, -0.05f, 0.05f, 0.f);

		Vector4 vp8(0.05f, -0.05f, 0.05f, 0.05f);
		Colour co8(0.0f, 0.05f, 0.0f);
		Vector4 nor8(0.05f, -0.05f, -0.05f, 0.f);


		CGVertex vs[8] = {
			{vp1, co1, {0.f, 0.f}, nor1, 1.f },
			{vp2, co2, {0.f, 1.f}, nor2, 1.f },
			{vp3, co3, {1.f, 1.f}, nor3, 1.f },
			{vp4, co4, {0.f, 1.f}, nor4, 1.f },

			{vp5, co5, {1.f, 0.f}, nor5, 1.f },
			{vp6, co6, {0.f, 1.f}, nor6, 1.f },
			{vp7, co7, {1.f, 1.f}, nor7, 1.f },
			{vp8, co8, {1.f, 1.f}, nor8, 1.f },
		};

		DrawPlane(vs[6], vs[5], vs[4], vs[7]);
		DrawPlane(vs[2], vs[1], vs[5], vs[6]);
		DrawPlane(vs[5], vs[1], vs[0], vs[4]);
		DrawPlane(vs[2], vs[6], vs[7], vs[3]);
		DrawPlane(vs[7], vs[4], vs[0], vs[3]);
		DrawPlane(vs[0], vs[1], vs[2], vs[3]);
	}

	void Device::DrawLightPositionAsABox(const Vector4 & lightPos)
	{
		Vector4 normalizedLightPosition = lightPos.GetNormalizedVector();
		Matrix4f translationMatrix;
		translationMatrix = Matrix4f::getTranslateMatrix(lightPos.getX() + 2.3f, lightPos.getY() + 1.f, lightPos.getZ());
		Matrix4f scaleMatrix = Matrix4f::getScaleMatrix(0.1f, 0.1f, 0.1f);
		Matrix4f influencialMatrix = scaleMatrix * translationMatrix;

		Transform::getInstance().worldMatrix = (influencialMatrix);
		Transform::getInstance().lightSpaceMatrix = influencialMatrix;

		Transform::getInstance().UpdateTransform();
		Transform::getInstance().UpdateTransformForShadowMap();

		Vector4 vp1(-0.05f, 0.05f, 0.05f, 0.05f);
		Colour co1(0.05f, 0.0f, 0.0f);
		Vector4 nor1(-0.05f, 0.05f, -0.05f, 0.f);

		Vector4 vp2(-0.05f, 0.05f, -0.05f, 0.05f);
		Colour co2(0.0f, 0.05f, 0.0f);
		Vector4 nor2(-0.05f, 0.05f, 0.05f, 0.f);

		Vector4 vp3(0.05f, 0.05f, -0.05f, 0.05f);
		Colour co3(0.0f, 0.0f, 0.05f);
		Vector4 nor3(0.05f, 0.05f, 0.05f, 0.f);

		Vector4 vp4(0.05f, 0.05f, 0.05f, 0.05f);
		Colour co4(0.05f, 0.05f, 0.0f);
		Vector4 nor4(0.05f, 0.05f, -0.05f, 0.f);

		Vector4 vp5(-0.05f, -0.05f, 0.05f, 0.05f);
		Colour co5(0.0f, 0.0f, 0.05f);
		Vector4 nor5(-0.05f, -0.05f, -0.05f, 0.f);

		Vector4 vp6(-0.05f, -0.05f, -0.05f, 0.05f);
		Colour co6(0.05f, 0.05f, 0.0f);
		Vector4 nor6(-0.05f, -0.05f, 0.05f, 0.f);

		Vector4 vp7(0.05f, -0.05f, -0.05f, 0.05f);
		Colour co7(0.05f, 0.0f, 0.0f);
		Vector4 nor7(0.05f, -0.05f, 0.05f, 0.f);

		Vector4 vp8(0.05f, -0.05f, 0.05f, 0.05f);
		Colour co8(0.0f, 0.05f, 0.0f);
		Vector4 nor8(0.05f, -0.05f, -0.05f, 0.f);

		CGVertex vs[8] = {
			{vp1, co1, {0.f, 0.f}, nor1, 1.f },
			{vp2, co2, {0.f, 1.f}, nor2, 1.f },
			{vp3, co3, {1.f, 1.f}, nor3, 1.f },
			{vp4, co4, {0.f, 1.f}, nor4, 1.f },

			{vp5, co5, {1.f, 0.f}, nor5, 1.f },
			{vp6, co6, {0.f, 1.f}, nor6, 1.f },
			{vp7, co7, {1.f, 1.f}, nor7, 1.f },
			{vp8, co8, {1.f, 1.f}, nor8, 1.f },
		};

		DrawPlane(vs[6], vs[5], vs[4], vs[7]);
		DrawPlane(vs[2], vs[1], vs[5], vs[6]);
		DrawPlane(vs[5], vs[1], vs[0], vs[4]);
		DrawPlane(vs[2], vs[6], vs[7], vs[3]);
		DrawPlane(vs[7], vs[4], vs[0], vs[3]);
		DrawPlane(vs[0], vs[1], vs[2], vs[3]);
	}
}

