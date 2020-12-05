#pragma once

#ifndef __CVVClip__
#define __CVVClip__


namespace RenderEngine {

	// �ú����ж�һ�������Ƿ���CVV(����۲���)����
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


	// ����βü��ռ��һ�������ν��вü�������ʱ��������εĶ��㶼Ӧ������ -w<=x<=w,-w<=xy=w,0=z<=w,w=ZCamera DX-Standard
	void ClipTriangle(CGVertex* p1, CGVertex* p2, CGVertex* p3)
	{
		bool isP1InCVV;
		bool isP2InCVV;
		bool isP3InCVV;

		// �ж�P1��P2��P3�������Ƿ���CVV��
		isP1InCVV = IsInCVV(p1);
		isP2InCVV = IsInCVV(p2);
		isP3InCVV = IsInCVV(p3);

		//��������㶼��CVV�ڣ��򲻽��вü�������ȫ�����вü���ͳһ����
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


	// CVV(��οռ�)�ü�
	void CVVClip(std::vector<CGVertex>& plist)
	{
		for (size_t i = 0; i != plist.size(); ++i)
		{
			if ((i % 3 == 0) && (plist[i].IsCull == false))
			{
				// ����һ��
				ClipTriangle(&plist[i], &plist[i + 1], &plist[i + 2]);
			}
		}
	}

}

#endif // !__CVVClip__
