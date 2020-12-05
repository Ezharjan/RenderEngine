#pragma once

#include"Math.h"
#include"3DStruct.h"


namespace RenderEngine 
{
	void DrawTriangle(Vector2 t0, Vector2 t1, Vector2 t2, Colour color) {
		if (t0.getY() == t1.getY() && t0.getY() == t2.getY()) return;
		// sort the vertices, t0, t1, t2 lower−to−upper 
		if (t0.getY() > t1.getY()) std::swap(t0, t1);
		if (t0.getY() > t2.getY()) std::swap(t0, t2);
		if (t1.getY() > t2.getY()) std::swap(t1, t2);
		int total_height = t2.getY() - t0.getY();
		for (int i = 0; i < total_height; i++) {
			bool second_half = i > t1.getY() - t0.getY() || t1.getY() == t0.getY();
			int segment_height = second_half ? t2.getY() - t1.getY() : t1.getY() - t0.getY();
			float alpha = (float)i / total_height;
			float beta = (float)(i - (second_half ? t1.getY() - t0.getY() : 0)) / segment_height; // be careful: with above conditions no division by zero here 
			Vector2 A = t0 + (t2 - t0)*alpha;
			Vector2 B = second_half ? t1 + (t2 - t1)*beta : t0 + (t1 - t0)*beta;
			if (A.getX() > B.getX()) std::swap(A, B);
		}
	}
}