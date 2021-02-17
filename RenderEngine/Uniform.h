#pragma once

#include "Vector.h"
#include "Colour.h"


namespace RenderEngine {

	class Uniform
	{
	public:
		Uniform() {};
		Uniform(const Vector4& wor, const Vector4& cam, const Vector4& lig,
			const Vector3& tex, const Vector4& nor, const Colour& col) :
			worldPos(wor), cameraScreenPos(cam), lightScreenPos(lig),
			texcoord(tex), normal(nor), color(col) {}

		Uniform(const Uniform& u)
		{
			worldPos = u.worldPos;
			cameraScreenPos = u.cameraScreenPos;
			lightScreenPos = u.cameraScreenPos;
			texcoord = u.texcoord;
			normal = u.normal;
			color = u.color;
		}
		Uniform& operator=(const Uniform& u)
		{
			worldPos = u.worldPos;
			cameraScreenPos = u.cameraScreenPos;
			lightScreenPos = u.cameraScreenPos;
			texcoord = u.texcoord;
			normal = u.normal;
			color = u.color;
			return *this;
		}
		~Uniform() {};

		auto Clone() const { return std::make_shared<Uniform>(*this); }

		static inline Uniform ClampLerp(const std::shared_ptr<Uniform> &u1, const std::shared_ptr<Uniform> &u2, float factor)
		{
			factor = Clamp(factor, 0.0f, 1.0f);
			return Uniform::Lerp(u1, u2, factor);
		}

		static inline Uniform Lerp(const std::shared_ptr<Uniform> u1, const std::shared_ptr<Uniform> u2, float factor)
		{
			Vector4 wor = u1->worldPos + (u2->worldPos - u1->worldPos)* factor;
			Vector4 cam = u1->cameraScreenPos + (u2->cameraScreenPos - u1->cameraScreenPos)* factor;
			Vector4 lig = u1->lightScreenPos + (u2->lightScreenPos - u1->lightScreenPos)* factor;
			Vector3 tex = u1->texcoord + (u2->texcoord - u1->texcoord)* factor;
			Vector4 nor = u1->normal + (u2->normal - u1->normal)* factor;
			Colour col = u1->color + (u2->color - u1->color)*factor;
			return Uniform(wor, cam, lig, tex, nor, col);
		}

	public:
		Vector4 worldPos;
		Vector4 cameraScreenPos;
		Vector4 lightScreenPos;
		Vector3 texcoord;
		Vector4 normal;
		Colour color;
	};
}

