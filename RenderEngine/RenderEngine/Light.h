#pragma once

#include "Vector.h"
#include "Colour.h"
#include "Math.h"
#include "CGVertex.h"

namespace RenderEngine {

	struct Light
	{
		Vector3 direction;
		Colour color;

		Light(const Vector3& direction, const Colour& color) :direction(direction), color(color) {}
		Light(const Colour& color) : color(color) {}
	};

	class DirectionalLight : public Light {
	public:
		// important to know that the initializer should be initiated from parent
		DirectionalLight(const Colour& c, const Vector3& position, const Vector3& target, float intensity)
			:Light(c), position(position), targetPosition(target), intensity(intensity) {}
		DirectionalLight(const Colour& c, const Vector3& direction, float intensity)
			:Light(Vector3::Normalize(direction), c), intensity(Clamp(intensity, 0.f, 1.f)) {}
		~DirectionalLight() {}


		// Golbal directional light using Lambert Shading 
		// I = Ia * Ka + Ip * Kd * (N¡¤L)
		// Ia * Ka --> ambient light
		// back side will be total balck in the case of removing ambient light
		static void LambertLightOn(Colour& ambientColour, DirectionalLight* light, Vector3& normalInWorldSpace, Colour& vertexColor)
		{
			//float I, Ia, Ip, Kd, cosTheta;
			float Ia, Ip, Kd, cosTheta;
			Ia = 0.2f; //ambient light intensity
			Ip = light->intensity; //directional light intensity
			Kd = 1.f; //coefficients of directional light
			light->direction.Normalize();
			normalInWorldSpace.Normalize(); // must be normalized!!!
			cosTheta = light->direction * normalInWorldSpace; // incident angel
			cosTheta > 1.f ? (cosTheta = 1.f) : (cosTheta < 0.f ? cosTheta = 0.f : cosTheta = cosTheta);
			////Formula: I = Ia * Ka + Ip * Kd * cosTheta;
			Colour result = ambientColour * (Ia * Kd) + light->color * (Ip * Kd * cosTheta);
			// clamp the colours' rgb value into valid range in case of bounding outside
			if (result.r > 1.f || result.g > 1.f || result.b > 1.f)
				result.r = result.g = result.b = 1.f;
			else if (result.r < 0.f || result.g < 0.f || result.b < 0.f)
				result.r = result.g = result.b = 0.f;
			vertexColor = result;
		}


		void ComputeLighting(const DirectionalLight& light, const Vector3& P, const Vector3& N,
			const Vector3& eyePosition, const float shininess,
			Colour& diffuseResult, Colour& specularResult, const float attenuate)
		{
			// caculate diffuse
			Vector3 L = (light.position - P);
			L.Normalize();
			float diffuseLight = max(N * L, 0); // always get the sitive value to ignore the error case such as shading from the back side
			diffuseResult = light.color * diffuseLight * attenuate;

			// caculate reflection
			Vector3 V = eyePosition - P;
			V.Normalize();
			Vector3 H = L + V;
			H.Normalize();
			float specularLight = pow(max((N*H), 0), shininess);
			if (diffuseLight <= 0) specularLight = 0;
			specularResult = light.color * specularLight * attenuate;
		}


	private:
		Vector3 targetPosition;
		Vector3 position;
		float intensity;

		// calculate the normal vector of the light in world space coordinate
		inline Vector3 DirectionInWorldSpace() {
			Vector3 direction;
			direction = position - targetPosition;
			direction.Normalize();
			return direction;
		};
	};
}
