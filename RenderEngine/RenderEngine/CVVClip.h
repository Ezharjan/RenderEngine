#pragma once

#include "CGVertex.h"
#include <vector>

namespace RenderEngine {

	// ����βü��ռ��һ�������ν��вü�������ʱ��������εĶ��㶼Ӧ������ -w<=x<=w,-w<=xy=w,0=z<=w,w=ZCamera DX-Standard
	void ClipTriangle(CGVertex* p1, CGVertex* p2, CGVertex* p3);
	
	// CVV(��οռ�)�ü�
	void CVVClip(std::vector<CGVertex>& plist);

	void CVVClip(const Vector4& p1,const Vector4& p2, const Vector4& p3);
	

}
