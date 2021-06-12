#pragma once

#include "Vector.h"
#include "CGVertex.h"
#include "Colour.h"

namespace RenderEngine {

	struct ModelInfo
	{
		std::vector<CGVertex> vertexVec;
		std::vector<VerticeIndex> verteciesIndexVec;
	};

	struct Face {
		int i1, i2, i3;
	};
}