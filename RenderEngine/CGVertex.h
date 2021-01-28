#pragma once

#include "Vector.h"
#include "Colour.h"
#include "Textcoord.h"

namespace RenderEngine {

#pragma pack(push)
	struct CGVertex
	{
		Vector4 pos;
		Colour color;
		Texcoord tex;
		Vector4 normal;
		float rhw; // reciprocal of homogenous W 
	};
#pragma pack(pop)


	typedef struct
	{
		int index[3];
	}VerticeIndex;
}
