/*主要是关于几何阶段的，世界变换，相机变换，透视投影变换，CVV裁剪，透视除法，视口变换，背面消除等方面*/
#pragma once

#include"3DStruct.h"
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <list>

#ifndef __GeoBase__
#define __GeoBase__

namespace RenderEngine {
	
	// 基本网络结构：顶点集合+索引集合
	struct MeshData
	{
		std::vector<CGVertex>	vertices;
		std::vector<UINT>	indices;
	};


	std::vector<std::string> MakeLineAnArray(std::string line)
	{
		std::vector<std::string> result;

		std::istringstream wordsInEachLine(line);
		std::string word;

		while (wordsInEachLine >> word)
		{
			result.push_back(word);
		}

		return result;
	}



	template <class T>
	T StringToNum(const std::string& str)
	{
		std::istringstream iss(str);
		T num;
		iss >> num;
		return num;
	}

	int CharToNum(const char& c)
	{
		return atoi(&c);
	}



	int GetLineCountsOfTheFile(const char* fileName) {
		FILE* fp = fopen(fileName, "r");
		fseek(fp, 0, 0);
		int count = 0;
		char line[256] = { 0 };
		while (fgets(line, 255, fp))
		{
			count++;
		}
		return count;
	}


	struct CubeModelData
	{
		std::vector<Vector3> vertecies;
		std::vector<Vector2> uvs;
		std::vector<Vector3> normals;
	};

	bool PrepareModelData(const std::string& ModelFileName, CubeModelData& cubeModelData) {

		std::vector<Vector3> vertices;
		std::vector<Vector2> uvs;
		std::vector<Vector3> normals;


		std::ifstream content(ModelFileName); // open the file to read the content in it 
		std::string line, word; // line may have spaces while word does not
		std::vector<std::string> lines;

		int lineCount = GetLineCountsOfTheFile(ModelFileName.c_str()); // empty line would be included also
		for (int i = 0; i < lineCount; i++)
		{
			getline(content, line);
			lines.push_back(line);
		}

		for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
		{
			std::istringstream wordsInEachLine(*it); // it points to current line 

			while (wordsInEachLine >> word)
			{
				if (word != "#")
				{
					if (word == "v")
					{
						std::vector<std::string> currentLine = MakeLineAnArray(*it);
						Vector3 currentVertex(StringToNum<float>(currentLine.at(1)), StringToNum<float>(currentLine.at(2)), StringToNum<float>(currentLine.at(3)));
						vertices.push_back(currentVertex);
					}
					else if (word == "vn")
					{
						std::vector<std::string> currentLine = MakeLineAnArray(*it);
						Vector3 currentNorm(StringToNum<float>(currentLine.at(1)), StringToNum<float>(currentLine.at(2)), StringToNum<float>(currentLine.at(3)));
						normals.push_back(currentNorm);
					}
					else if (word == "vt")
					{
						std::vector<std::string> currentLine = MakeLineAnArray(*it);
						Vector2 currentUV;
						currentUV.x = StringToNum<float>(currentLine.at(1));
						currentUV.y = StringToNum<float>(currentLine.at(2));
						uvs.push_back(currentUV);
					}
					else if (word == "f")
					{
						std::vector<std::string> currentLine = MakeLineAnArray(*it);
						//list<list<Vector3>> face;
						std::string w;
						int i = 0;
						while (wordsInEachLine >> w)
						{
							++i;
							//currentLine.at(i) ---> 1/1/1 2/2/2 3/3/3 4/4/4 

						}
					}
				}
			}

		}

		cubeModelData.vertecies = vertices;
		cubeModelData.uvs = uvs;
		cubeModelData.normals = normals;
		return true;
	}

	void GetOBJData()
	{
		CubeModelData cubeData;
		PrepareModelData("ModelData/2.obj", cubeData);

		std::cout << "------------vertecies------------" << std::endl;
		for (std::vector<Vector3>::iterator it = cubeData.vertecies.begin(); it != cubeData.vertecies.end(); ++it)
		{
			std::cout << "x: " << it->getX() << "   " << "y: " << it->getY() << "   " << "z: " << it->getZ() << " ." << std::endl;
		}

		std::cout << std::endl;
		std::cout << "------------normals------------" << std::endl;
		for (std::vector<Vector3>::iterator it = cubeData.normals.begin(); it != cubeData.normals.end(); ++it)
		{
			std::cout << "x: " << it->getX() << "   " << "y: " << it->getY() << "   " << "z: " << it->getZ() << " ." << std::endl;
		}

		std::cout << std::endl;
		std::cout << "------------uvs------------" << std::endl;
		for (std::vector<Vector2>::iterator it = cubeData.uvs.begin(); it != cubeData.uvs.end(); ++it)
		{
			std::cout << "x: " << it->x << "   " << "y: " << it->y << " ." << std::endl;
		}

		std::cout << std::endl;
	}

	// 建立一个左手坐标系的相机变换矩阵/视角矩阵
	Matrix4f MatrixLookAtLH(Vector3* eyePos, const Vector3* lookAt, const Vector3* up, Vector3& newCameraUpAxis)
	{
		Vector3 zAxis = *lookAt - *eyePos;
		VectorNormalize(zAxis);
		Vector3 xAxis = *up ^ zAxis;
		VectorNormalize(xAxis);
		Vector3 yAxis = zAxis ^ xAxis;
		newCameraUpAxis = yAxis; // store the last upaxis
		

		Matrix4f result;
	
		/// while(major==raw) => do: below
		result.matrix[0][0] = xAxis.getX();
		result.matrix[0][1] = yAxis.getX();
		result.matrix[0][2] = zAxis.getX();
		result.matrix[0][3] = 0;
		result.matrix[1][0] = xAxis.getY();
		result.matrix[1][1] = yAxis.getY();
		result.matrix[1][2] = zAxis.getY();
		result.matrix[1][3] = 0;
		result.matrix[2][0] = xAxis.getZ();
		result.matrix[2][1] = yAxis.getZ();
		result.matrix[2][2] = zAxis.getZ();
		result.matrix[2][3] = 0;
		result.matrix[3][0] = -(xAxis * (*eyePos));
		result.matrix[3][1] = -(yAxis * (*eyePos));
		result.matrix[3][2] = -(zAxis * (*eyePos));
		result.matrix[3][3] = 1;


		/*
		Matrix-Structure(
				xaxis.x,			yaxis.x,			zaxis.x,			0,
				xaxis.y,			yaxis.y,			zaxis.y,			0,
				xaxis.z,			yaxis.z,			zaxis.z,			0,
				-xaxis.Dot(eyePos), -yaxis.Dot(eyePos), -zaxis.Dot(eyePos), 1
		);*/
		return result;
	}



	// 建立一个左手坐标系的透视投影矩阵，视平面和近截面为同一个平面，FOV视角在YOZ平面上
	Matrix4f MatrixPerspectiveFovLH(const float& FovYOZ,const float& ScreenAspect,const float& ScreenNear,const float& ScreenFar)
	{
		Matrix4f mProjectionMatrix;

		// 清除为0
		ZeroMemory(&mProjectionMatrix, sizeof(mProjectionMatrix));

		float angle = RadToDegree(FovYOZ);

		// 半角
		angle /= 2.0f;

		// 求出各类参数
		float s1 = 1 / (ScreenAspect*(float)tan(angle));
		float s2 = 1 / tan(angle);
		float a = ScreenFar / (ScreenFar - ScreenNear);
		float b = -(ScreenNear*ScreenFar) / (ScreenFar - ScreenNear);

		mProjectionMatrix.matrix[0][0] = s1;
		mProjectionMatrix.matrix[1][1] = s2;
		mProjectionMatrix.matrix[2][2] = a;
		mProjectionMatrix.matrix[3][2] = b;
		mProjectionMatrix.matrix[2][3] = 1.0f;

		return mProjectionMatrix;
	}

	// 对点集进行透视除法，会变到NDC空间 --- Normalized Device Coordinate
	void PerspectiveDivede(std::vector<CGVertex>& pList)
	{
		for (std::vector<CGVertex>::iterator it = pList.begin(); it != pList.end(); ++it)
		{
			(*it).x = (*it).x / (*it).w;
			(*it).y = (*it).y / (*it).w;
			(*it).z = (*it).z / (*it).w;
			(*it).w = (*it).w / (*it).w;
		}
	}

	// 一般而言，视口变换矩阵的TopLeftX=0,TopLeftY=0,MaxDepth=1.0,MinDepth=0.0
	Matrix4f  MatrixViewPort(const float& ScreenWidth,const float& ScreenHeight,
						   const float& MaxDepth,   const float& MinDepth,
						   const float& TopLeftX,   const float& TopLeftY)
	{
		Matrix4f MatrixViewPort;

		// 清除为0
		ZeroMemory(&MatrixViewPort, sizeof(MatrixViewPort));

		MatrixViewPort.matrix[0][0] = ScreenWidth / 2.0f;
		MatrixViewPort.matrix[1][1] = -ScreenHeight / 2.0f;
		MatrixViewPort.matrix[2][2] = MaxDepth - MinDepth;
		MatrixViewPort.matrix[3][0] = TopLeftX + ScreenWidth / 2.0f;
		MatrixViewPort.matrix[3][1] = TopLeftY + ScreenHeight / 2.0f;
		MatrixViewPort.matrix[3][2] = MinDepth;
		MatrixViewPort.matrix[3][3] = 1.0f;

		return MatrixViewPort;
	}




	// 顶点/向量进行矩阵变换
	template <typename T>
	void Transform(T PointPtr, const Matrix4f* matrixPtr4f)
	{
		if (PointPtr == nullptr || matrixPtr4f == nullptr)
		{
			return;
		}
		else
		{
			// 不能在中途改变点的值后再用改变的点的值去计算，所以要先保存顶点的XYZW值
			float x = PointPtr->getX();
			float y = PointPtr->getY();
			float z = PointPtr->getZ();
			float w = PointPtr->getW();

			PointPtr->setX(x * matrixPtr4f->matrix[0][0] + y * matrixPtr4f->matrix[1][0] + z * matrixPtr4f->matrix[2][0] + w * matrixPtr4f->matrix[3][0]);
			PointPtr->setY(x * matrixPtr4f->matrix[0][1] + y * matrixPtr4f->matrix[1][1] + z * matrixPtr4f->matrix[2][1] + w * matrixPtr4f->matrix[3][1]);
			PointPtr->setZ(x * matrixPtr4f->matrix[0][2] + y * matrixPtr4f->matrix[1][2] + z * matrixPtr4f->matrix[2][2] + w * matrixPtr4f->matrix[3][2]);
			PointPtr->setW(x * matrixPtr4f->matrix[0][3] + y * matrixPtr4f->matrix[1][3] + z * matrixPtr4f->matrix[2][3] + w * matrixPtr4f->matrix[3][3]);
		}
	}



	void ObjectTransform(std::vector<CGVertex>& pList, const Matrix4f* matrix)
	{
		for (std::vector<CGVertex>::iterator it = pList.begin(); it != pList.end(); ++it)
		{
			Transform(&(*it), matrix);
		}
	}


	// 单纯将物体的法向量进行变换
	void TransormObjectNormalToWorld(std::vector<CGVertex> &pList, const Matrix4f* matrix4f)
	{
		for (std::vector<CGVertex>::iterator it = pList.begin(); it != pList.end(); ++it)
		{
			CGVertex *p = &(*it);

			Vector4* normal = new Vector4(p->nx, p->ny, p->nz);

			// 执行向量的变换
			Transform(&(*normal), matrix4f);

			p->nx = normal->getX();
			p->ny = normal->getY();
			p->nz = normal->getZ();
		}
	}


	float GetKFactor(float para) 
	{
		return 1.0 / ((para - 1.0f) * 0.5f);
	}

	void ArcBallTransform(const Vector2& pointOnScreen, Vector3& pointOnSphere) 
	{
		Vector2 tempVec2;
		
		// convert width&height to [-1,1]
		tempVec2.x = (pointOnScreen.x * GetKFactor(screenWidth)) - 1.f;
		tempVec2.y = 1.f - (pointOnScreen.y * GetKFactor(screenHeight));

		float length = pow(tempVec2.x, 2) + pow(tempVec2.y, 2);
		
		// if it's outside the ball
		if (length > 1.f)
		{
			// scale to sphere
			float norm = -1.f / sqrtf(length);

			pointOnSphere.setX(tempVec2.x * norm);
			pointOnSphere.setY(tempVec2.y * norm);
			pointOnSphere.setZ(0.f);
		}
		// if it's inside the ball
		else
		{
			pointOnSphere.setX(tempVec2.x);
			pointOnSphere.setY(tempVec2.y);
			pointOnSphere.setZ(sqrtf(1.f - length));
		}
	}


	// Get cosine angle of the rotation
	float GetArcAngleCos(const Vector3& startPosition, const Vector3& endPosition)
	{
		float r = CameraPos.getZ();
		float pointDistance = sqrtf(pow((endPosition.getX() - startPosition.getX()), 2) + pow((endPosition.getY() - startPosition.getY()), 2) + pow((endPosition.getZ() - startPosition.getZ()), 2));
		float consAngle = (pow(r, 2) + pow(r, 2) - pow(pointDistance, 2)) / (2 * r * r); // cosine formula
		return consAngle;
	}



	Vector4 GetQuaternion(const Vector2& lastPointV2, const Vector2& curPointV2)
	{
		Vector4 result;

		Vector3 lastPointV3;
		Vector3 curPointV3;
		ArcBallTransform(lastPointV2, lastPointV3);
		ArcBallTransform(curPointV2, curPointV3);
		
		Vector3 perp;
		perp = lastPointV3 ^ curPointV3;

		if (perp.getLength() > MATH_Epsilon)
		{
			result.setX(perp.getX());
			result.setY(perp.getY());
			result.setZ(perp.getZ());
			// w=cos(rotationAngle/2) ---> formula
			result.setW(lastPointV3 * curPointV3);
		}
		else
		{
			result.setX(.0f);
			result.setY(.0f);
			result.setZ(.0f);
			result.setW(.0f);
		}

		std::cout << "rotated in degree --->  " << RadToDegree(2 * acos(result.getW())) << std::endl;
		return result;
	}




	Matrix3f GetMatrixFromQuat(const Vector4& quaternion)
	{
		Matrix3f result;

		float n, s;
		float xs, ys, zs;
		float wx, wy, wz;
		float xx, xy, xz;
		float yy, yz, zz;

		n = powf(quaternion.getX(), 2) + powf(quaternion.getY(), 2) + powf(quaternion.getZ(), 2) + powf(quaternion.getW(), 2);
		s = (n > 0.f) ? (2.f / n) : 0.f;

		xs = quaternion.getX() * s;  ys = quaternion.getY() * s;  zs = quaternion.getZ() * s;
		wx = quaternion.getW() * xs; wy = quaternion.getW() * ys; wz = quaternion.getW() * zs;

		xx = quaternion.getX()* xs; xy = quaternion.getX()* ys; xz = quaternion.getX()* zs;
		yy = quaternion.getY() * ys; yz = quaternion.getY() * zs; zz = quaternion.getZ() * zs;

		result.matrix[0][0] = 1.0f - (yy + zz);
		result.matrix[0][1] = xy - wz;
		result.matrix[0][2] = xz + wy;
		result.matrix[1][0] = xy + wz;
		result.matrix[1][1] = 1.0f - (xx + zz);
		result.matrix[1][2] = yz - wx;
		result.matrix[2][0] = xz - wy;
		result.matrix[2][1] = yz + wx;
		result.matrix[2][2] = 1.0f - (xx + yy);

		return result;
	}

}

#endif // !__GeoBase__

