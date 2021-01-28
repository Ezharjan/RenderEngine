#include <vector>
#include "Math.h"
#include "Vector.h"
#include "GeoBase.h"
#include "Device.h"
#include "CustomizedException.h"


namespace RenderEngine {

	std::vector<std::string> MakeLineIntoAnArray(std::string line)
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

	int GetLineCountsOfTheFile(const char * fileName)
	{
		FILE* fp = fopen(fileName, "r");
		fseek(fp, 0, 0);
		int count = 0;
		char line[256] = { 0 };
		while (fgets(line, 255, fp)) count++;
		return count;
	}

	void StringSplit(const std::string& s, const char splitchar, std::vector<std::string>& vec)
	{
		if (vec.size() > 0) // check the vec is empty
			vec.clear(); // otherwise, clear it
		size_t length = s.length();
		int start = 0;
		for (int i = 0; i < length; i++)
		{
			if (s[i] == splitchar && i == 0) // when the spilt character is at first index
			{
				start += 1;
			}
			else if (s[i] == splitchar)
			{
				vec.push_back(s.substr(start, i - start));
				start = i + 1;
			}
			else if (i == length - 1) // reach the last
			{
				vec.push_back(s.substr(start, i + 1 - start));
			}
		}
	}

	char *ReadFile(const char* file)
	{
		FILE *pFile = fopen(file, "rb");
		if (!pFile) {
			return NULL;
		}

		char *pBuf;
		fseek(pFile, 0, SEEK_END);
		int len = ftell(pFile);
		pBuf = new char[len + 1];
		rewind(pFile);
		fread(pBuf, 1, len, pFile);
		pBuf[len] = '\0';
		fclose(pFile);
		return pBuf;
	}

	void ReadOBJFileIntoOBJModel(const std::string& path, ModelInfo * obj)
	{
		std::ifstream in(path);
		int vertexIndex = 0;
		int vertexCount = 0;
		std::string txt = "";

		if (in)
		{
			while (getline(in, txt)) // line excludes '\n' of each line
			{
				// vertex position
				if (txt[0] == 'v' && txt[1] == ' ')
				{
					std::vector<std::string> num;
					txt.erase(0, 2);
					StringSplit(txt, ' ', num);
					CGVertex newVertex;
					newVertex.pos = Vector4((float)atof(num[0].c_str()), (float)atof(num[1].c_str()), (float)atof(num[2].c_str()), 1.f);
					obj->vertexVec.push_back(newVertex);
					vertexCount++;
				}
				// vertex normal
				else if (txt[0] == 'v' && txt[1] == 'n')
				{
					std::vector<std::string> num;
					txt.erase(0, 3);
					StringSplit(txt, ' ', num);
					obj->vertexVec[vertexIndex].normal = Vector4((float)atof(num[0].c_str()), (float)atof(num[1].c_str()), (float)atof(num[2].c_str()), 0.0);
					vertexIndex++;
					if (vertexIndex == vertexCount)
					{
						vertexIndex = 0;
					}
				}
				// uv
				else if (txt[0] == 'v' && txt[1] == 't')
				{
					std::vector<std::string> num;
					txt.erase(0, 3);
					StringSplit(txt, ' ', num);
					obj->vertexVec[vertexIndex].tex = { (float)atof(num[0].c_str()), (float)atof(num[1].c_str()) };
					obj->vertexVec[vertexIndex].rhw = 1.0f;
					vertexIndex++;
					if (vertexIndex == vertexCount)
					{
						vertexIndex = 0;
					}
				}
				else if (txt[0] == 'f' && txt[1] == ' ')
				{
					std::vector<std::string> num;
					txt.erase(0, 2);
					StringSplit(txt, ' ', num);
					std::vector<std::string> index;
					for (int i = 0; i < num.size(); i++)
					{
						std::vector<std::string> threeIndex;
						StringSplit(num[i], '/', threeIndex);
						index.push_back(threeIndex[i]);
					}
					VerticeIndex indexes = { atoi(index[0].c_str()), atoi(index[1].c_str()), atoi(index[2].c_str()) };
					obj->verteciesIndexVec.push_back(indexes);
				}
			}
		}
		else
		{
			std::cout << "No such file or path named \'" << path << "\' !" << std::endl;
		}
	}


	int LoadMesh(const char *file, CGVertex*& pVertexs, int& vsize, Face*& pFaces, int& fsize)
	{
		char* pFile;
		pFile = ReadFile(file);
		if (!pFile) {
			//return 0;
			throw new FileNotFoundException();
		}

		char* pSrc;
		pSrc = pFile;

		int i, vc, fc;

		// count the vertecies and faces
		i = 0, vc = 0, fc = 0;
		char line[1024];
		memset(line, 0, 1024);
		for (; *pSrc != '\0';) {
			if (*pSrc == '\n') {
				if (line[0] == 'v') {
					++vc;
				}
				else if (line[0] == 'f') {
					++fc;
				}

				i = 0;
				memset(line, 0, 1024);
			}
			else {
				if (i > 1024) {
					delete pFile;
					return 0;
				}
				line[i++] = *pSrc;
			}
			++pSrc;
		}
		if (vc == 0 || fc == 0) {
			delete pFile;
			return 0;
		}

		vsize = vc; fsize = fc;
		pVertexs = new CGVertex[vc];
		pFaces = new Face[fc];

		pSrc = pFile;

		// read data
		i = 0, vc = 0, fc = 0;
		memset(line, 0, 1024);
		for (; *pSrc != '\0';) {
			if (*pSrc == '\n') {
				if (line[0] == 'v') {
					float x, y, z;
					sscanf(line, "v %f %f %f", &x, &y, &z);

					Vector4 vec4Point(x, y, z, 1.f);
					Colour col(0.f, 0.f, 0.f);
					Vector4 norl(0.f, 0.f, 0.f);
					//should get texcoord(vt:u,v) here!!!
					pVertexs[vc++] = { vec4Point, col,{0.f, 1.f}, norl, 1.f };
				}
				else if (line[0] == 'f') {
					int p1, p2, p3;
					sscanf(line, "f %d %d %d", &p1, &p2, &p3);

					pFaces[fc++] = { p1 - 1, p2 - 1, p3 - 1 };
				}

				i = 0;
				memset(line, 0, 1024);
			}
			else {
				line[i++] = *pSrc;
			}
			++pSrc;
		}

		delete pFile;

		return 1;
	}


	bool LoadBmp(const std::string bmpName, Colour**& pixelColorResult, int& widthResult, int& heightResult)
	{
		const char* charPath = bmpName.data();
		int linebyte;
		BYTE* bmpBuffer = NULL;
		FILE *fp;
		if ((fp = fopen(charPath, "rb")) == NULL)  // open the file in binary mode 
		{
			std::cout << "The file " << charPath << "was not opened" << std::endl;
			return false;
		}
		if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))  // escape BITMAPFILEHEADE  
		{
			std::cout << "error happened! " << std::endl;
			return false;
		}
		BITMAPINFOHEADER infoHead;
		fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);   // read BITMAPINFOHEADER into infoHead

		widthResult = infoHead.biWidth;
		heightResult = infoHead.biHeight;

		linebyte = widthResult * 3; // 24 bits of color, 8 bits for each 
		bmpBuffer = new BYTE[linebyte*heightResult];
		fread(bmpBuffer, sizeof(char), linebyte*heightResult, fp);

		if (widthResult > 0 && heightResult > 0)
		{
			pixelColorResult = new Colour*[heightResult];
			for (int i = 0; i < heightResult; ++i)
			{
				pixelColorResult[i] = new Colour[widthResult]; // initialize texture
			}
			int index = 2;
			for (int i = 0; i < heightResult; i++)
			{
				for (int j = 0; j < widthResult; j++)
				{
					pixelColorResult[j][i] = Colour((float)bmpBuffer[index - 0] / 255.0f, (float)bmpBuffer[index - 1] / 255.0f, (float)bmpBuffer[index - 2] / 255.0f);
					index = index + 3;
				}
			}
			fclose(fp);
			delete[] bmpBuffer;
			return true;
		}
		return false;
	}


	Colour BilinearInterp(Colour** textureColorData, const int textureWidth, const int textureHeight, const float u, const float v)
	{
		//float y = u * textureHeight, x = v * textureWidth;
		float y = v, x = u;

		int x0 = (int)floorf(x);
		int y0 = (int)floorf(y);
		int x1 = x0 + 1, y1 = y0 + 1;

		// border correction
		if (x0 < 0) {
			x0 = 0;
			x1 = 1;
			x = 0;
		}
		if (y0 < 0) {
			y0 = 0;
			y1 = 1;
			y = 0;
		}
		if (x1 > textureWidth - 1) {
			x1 = textureWidth - 1;
			x0 = textureWidth - 2;
			x = (float)textureWidth - 1;
		}
		if (y1 > textureHeight - 1) {
			y1 = textureHeight - 1;
			y0 = textureHeight - 2;
			y = (float)textureHeight - 1;
		}


		float w00 = (float)((y1 - y) * (x1 - x));
		float w01 = (float)((y1 - y) * (x - x0));
		float w11 = (float)((y - y0) * (x - x0));
		float w10 = (float)((y - y0) * (x1 - x));


		Colour c00 = textureColorData[x0][y0];
		Colour c01 = textureColorData[x1][y0];
		Colour c10 = textureColorData[x0][y1];
		Colour c11 = textureColorData[x1][y1];

		Colour interpedResult00 = c00 * w00;
		Colour interpedResult01 = c01 * w01;
		Colour interpedResult10 = c10 * w10;
		Colour interpedResult11 = c11 * w11;

		Colour c = interpedResult00 + interpedResult01 + interpedResult10 + interpedResult11;

		return c;
	}


	bool GetTriangleInterp(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& p, float *u, float *v)
	{
		float a, b, c;

		float nv1 = ((v1.getY() - v2.getY())*v3.getX() + (v2.getX() - v1.getX())*v3.getY() + v1.getX()*v2.getY() - v2.getX()*v1.getY());
		float nv2 = ((v1.getY() - v3.getY())*v2.getX() + (v3.getX() - v1.getX())*v2.getY() + v1.getX()*v3.getY() - v3.getX()*v1.getY());

		if (nv1 == 0.f || nv2 == 0.f) {
			*u = 1.f; *v = 0.f;
			return 1;
		}

		c = ((v1.getY() - v2.getY())*p.getX() + (v2.getX() - v1.getX())*p.getY() + v1.getX()*v2.getY() - v2.getX()*v1.getY()) / nv1;
		if (c < 0 || c > 1) return 0;
		b = ((v1.getY() - v3.getY())*p.getX() + (v3.getX() - v1.getX())*p.getY() + v1.getX()*v3.getY() - v3.getX()*v1.getY()) / nv2;
		if (b < 0 || b > 1) return 0;
		a = 1 - b - c;
		if (a < 0 || a > 1) return 0;

		*u = a; *v = b;
		return true;
	}

	void VertexRHWInit(CGVertex& v)
	{
		v.rhw = 1 / v.pos.getW();

		// open the codes below to make model color ranges from near to far
		/*v.color.r *= v.rhw;
		v.color.g *= v.rhw;
		v.color.b *= v.rhw;*/

		v.tex.u *= v.rhw;
		v.tex.v *= v.rhw;
		v.normal.setX(v.normal.getX() * v.rhw);
		v.normal.setY(v.normal.getY() * v.rhw);
		v.normal.setZ(v.normal.getZ() * v.rhw);
	}


	void InvertVertexRHWInit(CGVertex& v)
	{
		if (v.rhw == 0.f || v.rhw - EPSILON == 0.f) {
			$log("%%%%%%%%%%%%%%%%%%");
		}

		v.pos.setW(1/ v.rhw);

		v.tex.u = v.pos.getW() * v.tex.u;
		v.tex.v = v.pos.getW() * v.tex.v;
		v.normal.setX(v.normal.getX() * v.pos.getW());
		v.normal.setY(v.normal.getY() * v.pos.getW());
		v.normal.setZ(v.normal.getZ() * v.pos.getW());
	}

	float GetKFactorForArcBall(const float para)
	{
		// mouse point ranges from [0, window_width - 1]
		return 1.f / ((para - 1.f) * 0.5f);
	}

	void ArcBallTransform(const Vector2& pointOnScreen, Vector3& pointOnSphere)
	{
		// convert width&height to [-1,1], left-hand coordinate
		Vector2 tempVec2((pointOnScreen.getX() * GetKFactorForArcBall(WINDOW_WIDTH)) - 1.f, (pointOnScreen.getY() * GetKFactorForArcBall(WINDOW_HEIGHT)) - 1.f);
		//Vector2 tempVec2((pointOnScreen.getX() * GetKFactorForArcBall(WINDOW_WIDTH)) - 1.f,1.f - (pointOnScreen.getY() * GetKFactorForArcBall(WINDOW_HEIGHT))); // left-hand coordinate
		float length = pow(tempVec2.getX(), 2) + pow(tempVec2.getY(), 2);

		// if it's outside the ball
		if (length > 1.f)
		{
			// scale to sphere
			float norm = -1.f / sqrtf(length);

			pointOnSphere.setX(tempVec2.getX() * norm);
			pointOnSphere.setY(tempVec2.getY() * norm);
			pointOnSphere.setZ(0.f);
		}
		// if it's inside the ball
		else
		{
			pointOnSphere.setX(tempVec2.getX());
			pointOnSphere.setY(tempVec2.getY());
			pointOnSphere.setZ(sqrtf(1.f - length));
		}
	}

	float GetArcAngleCosineValue(const Vector3& position, const Vector3& startPosition, const Vector3& endPosition)
	{
		float r = position.getZ();
		float pointDistance = sqrtf(
			pow((endPosition.getX() - startPosition.getX()), 2)
			+ pow((endPosition.getY() - startPosition.getY()), 2)
			+ pow((endPosition.getZ() - startPosition.getZ()), 2));
		float consAngle = (pow(r, 2) + pow(r, 2) - pow(pointDistance, 2)) / (2 * r * r); // cosine formula
		return consAngle;
	}

	Vector4 GetQuaternion(const Vector2 & lastPointV2, const Vector2 & curPointV2)
	{
		Vector4 result;

		Vector3 lastPointV3;
		Vector3 curPointV3;
		ArcBallTransform(lastPointV2, lastPointV3);
		ArcBallTransform(curPointV2, curPointV3);

		Vector3 perp;
		perp = lastPointV3 ^ curPointV3;

		if (perp.getLength() > EPSILON)
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

		///std::cout << "rotated (in degree)" << RadToDegree(2 * acosf(result.getW())) << std::endl;
		return result;
	}
}

