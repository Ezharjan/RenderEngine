#pragma once

#ifndef __CVVClip__
#define __CVVClip__


namespace RenderEngine {

	// 该函数判断一个顶点是否在CVV(规则观察体)体中
	bool IsInCVV(const CGVertex* p)
	{
		float x = p->x;
		float y = p->y;
		float z = p->z;
		float w = p->w;

		if (x <= w && x >= -w && y <= w && y >= -w && z <= w && z >= 0)
			return true;

		return false;

	}


	// 在齐次裁剪空间对一个三角形进行裁剪，即此时组成三角形的顶点都应该满足 -w<=x<=w,-w<=xy=w,0=z<=w,w=ZCamera DX-Standard
	void ClipTriangle(CGVertex* p1, CGVertex* p2, CGVertex* p3)
	{
		bool isP1InCVV;
		bool isP2InCVV;
		bool isP3InCVV;

		// 判断P1，P2，P3三个点是否在CVV内
		isP1InCVV = IsInCVV(p1);
		isP2InCVV = IsInCVV(p2);
		isP3InCVV = IsInCVV(p3);

		//如果三个点都在CVV内，则不进行裁剪，否则全部进行裁剪，统一处理
		if (isP1InCVV == true && isP2InCVV == true && isP3InCVV == true)
		{
			return;
		}
		else
		{
			p1->IsCull = true;
			p2->IsCull = true;
			p3->IsCull = true;
		}
	}


	// CVV(齐次空间)裁剪
	void CVVClip(std::vector<CGVertex>& plist)
	{
		for (size_t i = 0; i != plist.size(); ++i)
		{
			if ((i % 3 == 0) && (plist[i].IsCull == false))
			{
				// 三个一组
				ClipTriangle(&plist[i], &plist[i + 1], &plist[i + 2]);
			}
		}
	}

}

#endif // !__CVVClip__
