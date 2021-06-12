#pragma once

#include <sstream>
#include <fstream>
#include <vector>
#include "Config.h"
#include "CGVertex.h"
#include "ModelInfos.h"

namespace RenderEngine {

	std::vector<std::string> MakeLineIntoAnArray(std::string line);

	int GetLineCountsOfTheFile(const char* fileName);
	
	void StringSplit(const std::string& s, const char splitchar, std::vector<std::string>& vec);

	char *ReadFile(const char* file);

	void ReadOBJFileIntoOBJModel(const std::string& path, ModelInfo * obj);

	int LoadMesh(const char *file, CGVertex*& pVertexs, int& vsize, Face*& pFaces, int& fsize);
	
	bool LoadBmp(const std::string bmpName, Colour**& pixelColorResult, int& widthResult, int& heightResult);

	Colour BilinearInterp(Colour** textureColorData, const int textureWidth, const int textureHeight, const float u, const float v);

	bool GetTriangleInterp(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& p, float *u, float *v);

	void VertexRHWInit(CGVertex& v);

	float GetKFactorForArcBall(float para);

	void ArcBallTransform(const Vector2& pointOnScreen, Vector3& pointOnSphere);

	float GetArcAngleCosineValue(const Vector3& position, const Vector3& startPosition, const Vector3& endPosition);

	Vector4 GetQuaternion(const Vector2 & lastPointV2, const Vector2 & curPointV2);
	
}
