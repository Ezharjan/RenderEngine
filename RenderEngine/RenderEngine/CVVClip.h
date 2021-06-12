#pragma once

#include "CGVertex.h"
#include <vector>

namespace RenderEngine {

	// 在齐次裁剪空间对一个三角形进行裁剪，即此时组成三角形的顶点都应该满足 -w<=x<=w,-w<=xy=w,0=z<=w,w=ZCamera DX-Standard
	void ClipTriangle(CGVertex* p1, CGVertex* p2, CGVertex* p3);
	
	// CVV(齐次空间)裁剪
	void CVVClip(std::vector<CGVertex>& plist);

	void CVVClip(const Vector4& p1,const Vector4& p2, const Vector4& p3);
	

}
