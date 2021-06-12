#pragma once

// ����任

#include "math.h"

class Transform
{
public:
	void init(int width, int height);
	void update();
	void apply(Vector& b, const Vector& a);
	void applyMV(Vector& b, const Vector& a);
	void homogenize(Vector& b, const Vector& a);
	int checkCvv(const Vector& v);
	int checkBackCulling(const Vector& v);

	void setWorld(const Matrix& m);
	void setView(const Matrix& m);

private:
	Matrix world;
	Matrix view;
	Matrix projection;
	Matrix transform;
	float width, height; // screen
};
