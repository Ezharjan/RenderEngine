#include <vector>
#include "CVVClip.h"
#include "Transform.h"
#include "CGVertex.h"


void RenderEngine::ClipTriangle(CGVertex * p1, CGVertex * p2, CGVertex * p3)
{
	bool isP1OutsideCVV;
	bool isP2OutsideCVV;
	bool isP3OutsideCVV;

	// 判断P1，P2，P3三个点是否在CVV外
	isP1OutsideCVV = Transform::getInstance().IsOutsideCVV(p1->pos);
	isP2OutsideCVV = Transform::getInstance().IsOutsideCVV(p2->pos);
	isP3OutsideCVV = Transform::getInstance().IsOutsideCVV(p3->pos);

	//如果三个点都在CVV内，则不进行裁剪，否则全部进行裁剪，统一处理
	if (isP1OutsideCVV == true && isP2OutsideCVV == true && isP3OutsideCVV == true)
	{
		p1->shouldCull = true;
		p2->shouldCull = true;
		p3->shouldCull = true;
	}
	else
	{
		return;
	}
}

void RenderEngine::CVVClip(std::vector<CGVertex>& plist)
{
	for (size_t i = 0; i != plist.size(); ++i)
	{
		if ((i % 3 == 0) && (plist[i].shouldCull == false))
		{
			// 三个一组
			ClipTriangle(&plist[i], &plist[i + 1], &plist[i + 2]);
		}
	}
}

void RenderEngine::CVVClip(const Vector4 & p1, const Vector4 & p2, const Vector4 & p3)
{
		//std::vector<CGVertex> vert_list{ p1, p2, p3 };
		std::vector<CGVertex> in_list1;

		//ClipWithPlane(Vector4(0, 0, camera->GetNear(), 1), Vector4(0, 0, 1, 0), vert_list, in_list1);	//near

		int num_vertex = (int)in_list1.size() - 2;

		Vector4 world_pos[3];

		for (int i = 0; i < num_vertex; ++i)
		{
			int index0 = 0;
			int index1 = i + 1;
			int index2 = i + 2;
			//Triangle cam_triangle(in_list1[index0], in_list1[index1], in_list1[index2]);

			//DrawTriangleColor(cam_triangle);
		}
}

