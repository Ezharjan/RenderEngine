/*��Ҫ�ǻ��ߺ������������κ�����*/
#pragma once

#include<sstream>
#include<fstream>
#include"GeoBase.h"
#include "DrawTriangle.h"
#include "RasterTriangle.h"
#include "RasterLine.h"
#include "CVVClip.h"
#include "CustomizedException.h"


#ifndef __Preperation__
#define __Preperation__


namespace RenderEngine {

	// Culling the back: as the Y-axis has been reversed after the viewport transformation, Z-axis should be reversed too
	void BackCulling(std::vector<CGVertex> &pList)
	{
		for (size_t i = 0; i != pList.size(); ++i)
		{
			if ((i % 3 == 0) && (pList[i].IsCull == false))
			{
				Vector4 p1(pList[i].x, pList[i].y, pList[i].z, 1.f);
				Vector4 p2(pList[i + 1].x, pList[i + 1].y, pList[i + 1].z, 1.f);
				Vector4 p3(pList[i + 2].x, pList[i + 2].y, pList[i + 2].z, 1.f);

				Vector4 vec1 = p2 - p1;
				Vector4 vec2 = p3 - p1;
				Vector4 vec3 = vec1 ^ vec2; // cross product

				if (vec3.getZ() <= 0.0f)
				{
					pList[i].IsCull = true;
					pList[i + 1].IsCull = true;
					pList[i + 2].IsCull = true;
				}
			}
		}
	}


	bool LoadModelData(std::vector<CGVertex>& verts, const std::string& ModelFileName)
	{
		std::vector<Vector3> vertices;
		std::vector<Vector2> uvs;
		std::vector<Vector3> normals;

		int vertexCount = 0; // got from file by counting the lines started with 'v'
		int polygons = 0; // got from file by counting the lines started with 'f'
		int uvCount = 0; // got from file by counting the lines started with 'vt'

		std::vector<int> pointIndex;

		std::ifstream content(ModelFileName); // open the file to read the content in it 
		std::string line, word; // line may have spaces while word does not
		std::vector<std::string> lines;

		int lineCount = GetLineCountsOfTheFile(ModelFileName.c_str()); // empty lines would be included also
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
						++vertexCount;
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
						++uvCount;
						std::vector<std::string> currentLine = MakeLineAnArray(*it);
						Vector2 currentUV;
						currentUV.x = StringToNum<float>(currentLine.at(1));
						currentUV.y = StringToNum<float>(currentLine.at(2));
						uvs.push_back(currentUV);
					}
					else if (word == "f")
					{
						++polygons;
						std::vector<std::string> currentLine = MakeLineAnArray(*it);
						///cout << "///" << *it << " - - - "; // f 1 2 3 4 

						//std::cout << "the size of current line is :" << currentLine.size() << endl; //if: 5
						//std::cout << "the capacity of current line is :" << currentLine.capacity() << endl; //then: 6
						if (polygons <= uvCount / 2)
						{
							for (int i = 1; i < currentLine.size(); i++)
							{
								pointIndex.push_back(StringToNum<int>(currentLine.at(i)));
							}
						}
					}
				}
			}
		}

		if (pointIndex.size() == 0)
		{
			throw FileFormatNotSupportedException();
			return false;
		}


		//CGVertex m_vert[polygons * polygons]; // ---> the size of array defined in this way must be a const number 
		int indicies = pow(polygons, 2);
		CGVertex* m_vert = new CGVertex[indicies]; // square of number of line started with 'f'


		// Set points 
		//P1 --- 8
		m_vert[0].x = vertices.at(pointIndex.back() - 1).getX();
		m_vert[0].y = vertices.at(pointIndex.back() - 1).getY();
		m_vert[0].z = vertices.at(pointIndex.back() - 1).getZ();

		pointIndex.pop_back();

		//P2 --- 7
		m_vert[1].x = vertices.at(pointIndex.back() - 1).getX();
		m_vert[1].y = vertices.at(pointIndex.back() - 1).getY();
		m_vert[1].z = vertices.at(pointIndex.back() - 1).getZ();

		pointIndex.pop_back();

		//P8 --- 6
		m_vert[7].x = vertices.at(pointIndex.back() - 1).getX();
		m_vert[7].y = vertices.at(pointIndex.back() - 1).getY();
		m_vert[7].z = vertices.at(pointIndex.back() - 1).getZ();

		pointIndex.pop_back();

		//P14 --- 5
		m_vert[13].x = vertices.at(pointIndex.back() - 1).getX();
		m_vert[13].y = vertices.at(pointIndex.back() - 1).getY();
		m_vert[13].z = vertices.at(pointIndex.back() - 1).getZ();

		pointIndex.pop_back();


		//P12 --- 4
		m_vert[11].x = vertices.at(pointIndex.back() - 1).getX();
		m_vert[11].y = vertices.at(pointIndex.back() - 1).getY();
		m_vert[11].z = vertices.at(pointIndex.back() - 1).getZ();


		pointIndex.pop_back();

		//P6 --- 3
		m_vert[5].x = vertices.at(pointIndex.back() - 1).getX();
		m_vert[5].y = vertices.at(pointIndex.back() - 1).getY();
		m_vert[5].z = vertices.at(pointIndex.back() - 1).getZ();

		pointIndex.pop_back();


		//P3 --- 2
		m_vert[2].x = vertices.at(pointIndex.back() - 1).getX();
		m_vert[2].y = vertices.at(pointIndex.back() - 1).getY();
		m_vert[2].z = vertices.at(pointIndex.back() - 1).getZ();

		pointIndex.pop_back();


		//P18 --- 1
		m_vert[17].x = vertices.at(pointIndex.back() - 1).getX();
		m_vert[17].y = vertices.at(pointIndex.back() - 1).getY();
		m_vert[17].z = vertices.at(pointIndex.back() - 1).getZ();


		pointIndex.pop_back();


		//P9 --- 3
		m_vert[8].x = m_vert[5].x;
		m_vert[8].y = m_vert[5].y;
		m_vert[8].z = m_vert[5].z;


		//P4 --- 2
		m_vert[3] = m_vert[2];
		//P5 --- 7
		m_vert[4] = m_vert[1];


		
		//P7 --- 7
		m_vert[6].x = m_vert[1].x;
		m_vert[6].y = m_vert[1].y;
		m_vert[6].z = m_vert[1].z;


		

		//P10 --- 3
		m_vert[9] = m_vert[8];
		//P11 --- 6
		m_vert[10] = m_vert[7];



		//P13 --- 6
		m_vert[12].x = m_vert[7].x;
		m_vert[12].y = m_vert[7].y;
		m_vert[12].z = m_vert[7].z;



		//P15 --- 4
		m_vert[14].x = m_vert[11].x;
		m_vert[14].y = m_vert[11].y;
		m_vert[14].z = m_vert[11].z;


		//P16 --- 4
		m_vert[15] = m_vert[14];
		//P17 --- 5
		m_vert[16] = m_vert[13];



		//P19 --- 5
		m_vert[18].x = m_vert[13].x;
		m_vert[18].y = m_vert[13].y;
		m_vert[18].z = m_vert[13].z;


		//P20 --- 8
		m_vert[19].x = m_vert[0].x;
		m_vert[19].y = m_vert[0].y;
		m_vert[19].z = m_vert[0].z;

		//P21 --- 1
		m_vert[20].x = m_vert[17].x;
		m_vert[20].y = m_vert[17].y;
		m_vert[20].z = m_vert[17].z;


		//P22 --- 1
		m_vert[21] = m_vert[20];
		//P23 --- 8
		m_vert[22] = m_vert[19];

		//P24 --- 2
		m_vert[23].x = m_vert[2].x;
		m_vert[23].y = m_vert[2].y;
		m_vert[23].z = m_vert[2].z;


		//P25 --- 5
		m_vert[24].x = m_vert[13].x;
		m_vert[24].y = m_vert[13].y;
		m_vert[24].z = m_vert[13].z;


		//P26 --- 6
		m_vert[25].x = m_vert[7].x;
		m_vert[25].y = m_vert[7].y;
		m_vert[25].z = m_vert[7].z;


		//P27 --- 8
		m_vert[26].x = m_vert[0].x;
		m_vert[26].y = m_vert[0].y;
		m_vert[26].z = m_vert[0].z;


		//P28 --- 8
		m_vert[27] = m_vert[26];
		//P29 --- 6
		m_vert[28] = m_vert[25];

		//30 --- 7
		m_vert[29].x = m_vert[1].x;
		m_vert[29].y = m_vert[1].y;
		m_vert[29].z = m_vert[1].z;


		//31 --- 2
		m_vert[30].x = m_vert[2].x;
		m_vert[30].y = m_vert[2].y;
		m_vert[30].z = m_vert[2].z;


		//32 --- 3
		m_vert[31].x = m_vert[5].x;
		m_vert[31].y = m_vert[5].y;
		m_vert[31].z = m_vert[5].z;

		//33 --- 1
		m_vert[32].x = m_vert[17].x;
		m_vert[32].y = m_vert[17].y;
		m_vert[32].z = m_vert[17].z;

		//34 --- 1
		m_vert[33] = m_vert[32];
		//P35 --- 3
		m_vert[34] = m_vert[31];

		//36 --- 4
		m_vert[35].x = m_vert[11].x;
		m_vert[35].y = m_vert[11].y;
		m_vert[35].z = m_vert[11].z;



		for (size_t i = 0; i < indicies; i++)
		{

			// Set normals
			for (size_t j = 0; j < polygons; j++)
			{
				if (i >= polygons * j && i < polygons * (j + 1))
				{
					m_vert[i].nx = normals.at(j).getX();
					m_vert[i].ny = normals.at(j).getY();
					m_vert[i].nz = normals.at(j).getZ();
				}
			}
			

			// Set UVs
			if (i % polygons == 0)
			{
				m_vert[i].u = uvs.at(0).x;
				m_vert[i].v = uvs.at(0).y;

				m_vert[i].r = 255;
				m_vert[i].g = 0;
				m_vert[i].b = 0;
			}
			else if (i % polygons == uvCount - 3 || i % polygons == uvCount)
			{
				m_vert[i].u = uvs.at(uvCount - 3).x;
				m_vert[i].v = uvs.at(uvCount - 3).y;

				m_vert[i].r = 0;
				m_vert[i].g = 255;
				m_vert[i].b = 0;
			}
			else if (i % polygons == uvCount - uvCount / 2 || i % polygons == uvCount - 1)
			{
				m_vert[i].u = uvs.at(uvCount - uvCount / 2).x;
				m_vert[i].v = uvs.at(uvCount - uvCount / 2).y;

				m_vert[i].r = 0;
				m_vert[i].g = 0;
				m_vert[i].b = 255;
			}
			else if (i % polygons == uvCount + 1)
			{
				m_vert[i].u = uvs.at(uvCount - 1).x;
				m_vert[i].v = uvs.at(uvCount - 1).y;

				m_vert[i].r = 122;
				m_vert[i].g = 122;
				m_vert[i].b = 122;
			}
			//// original type of code befor optimizing
			/*if (i % polygons == 0)
			{
				m_vert[i].u = uvs.at(0).x;
				m_vert[i].v = uvs.at(0).y;
			}
			else if (i % polygons == 1 || i % polygons == 4)
			{
				m_vert[i].u = uvs.at(1).x;
				m_vert[i].v = uvs.at(1).y;
			}
			else if (i % polygons == 2 || i % polygons == 3)
			{
				m_vert[i].u = uvs.at(2).x;
				m_vert[i].v = uvs.at(2).y;
			}
			else if (i % polygons == 5)
			{
				m_vert[i].u = uvs.at(3).x;
				m_vert[i].v = uvs.at(3).y;
			}*/

			
		}


		for (int i = 0; i < indicies; i++)
		{
			m_vert[i].w = 1.f;
			verts.push_back(m_vert[i]);
		}
		delete[] m_vert;
		return true;
	}

	// ���ⲿ�������ݣ���������������
	bool  OriginalVersion_LoadModelData(std::vector<CGVertex>& PList, const std::string& ModelFileName)
	{
		std::ifstream in(ModelFileName);

		// line�����ļ����һ���У������ո��ַ��ȣ� ��word�����ļ����һ�����ʣ��޿ո���ڡ�
		std::string line, word;

		int mVertexCount;

		// �����ڵ�һ�ж�ȡ��������(���ﶥ����ÿ��������һ��������)
		std::getline(in, line);

		std::istringstream record(line);
		for (int i = 0; i < 3; ++i)
		{
			record >> word;
		}

		// ��������
		mVertexCount = atoi(&word[0]);

		// ������Data:����һ��
		getline(in, line);

		// ��ȡ�����Ķ�������
		for (int i = 0; i < mVertexCount; ++i)
		{
			getline(in, line);
			std::istringstream record(line);

			CGVertex pMem;
			pMem.w = 1.0f;

			//x
			record >> word;
			pMem.x = atof(&word[0]);

			//y
			record >> word;
			pMem.y = atof(&word[0]);

			//z
			record >> word;
			pMem.z = atof(&word[0]);

			//u
			record >> word;
			pMem.u = atof(&word[0]);

			//v
			record >> word;
			pMem.v = atof(&word[0]);

			//nx
			record >> word;
			pMem.nx = atof(&word[0]);

			//ny
			record >> word;
			pMem.ny = atof(&word[0]);

			//nz
			record >> word;
			pMem.nz = atof(&word[0]);

			// ����ÿ��������(ÿ��������)�ĵ�һ���������ɫ�Ǻ�ɫ����������ɫ�������������Ƶ������ɫ
			if (i % 6 == 0)
			{
				pMem.r = 255;
				pMem.g = 0;
				pMem.b = 0;

			}
			else if (i % 6 == 1)
			{
				pMem.r = 0;
				pMem.g = 255;
				pMem.b = 0;
			}
			else if (i % 6 == 2)
			{
				pMem.r = 0;
				pMem.g = 0;
				pMem.b = 255;
			}
			else if (i % 6 == 3)
			{
				pMem.r = 0;
				pMem.g = 0;
				pMem.b = 255;
			}
			else if (i % 6 == 4)
			{
				pMem.r = 0;
				pMem.g = 255;
				pMem.b = 0;
			}
			else if (i % 6 == 5)
			{
				pMem.r = 255;
				pMem.g = 0;
				pMem.b = 0;
			}
			PList.push_back(pMem);
		}

		return  true;
	}


	// ʹ��ԭ���ķ������ⲿ����BMPλͼ���ݣ�����������ͼ
	void LoadTextureData(const char* TexureFilename)
	{
		// ����Ƿ���BMP�ļ���������ǣ����׳�FormatNotSupported�쳣
		// ��ʹ���ļ���׺����ͨ�����BMT�ļ��ڲ����ݽṹ�������Ƿ�ΪBMP�ļ�


		char *buf;                                //�����ļ���ȡ������
		char *p;
		int r, g, b, pix;

		FILE *fp;                                 //�����ļ�ָ��
		FILE *fpw;                                //���屣���ļ�ָ��
		//DWORD w, h;                             //�����ȡͼ��ĳ��Ϳ�
		DWORD bitCorlorUsed;                      //����
		DWORD bitSize;                            //����ͼ��Ĵ�С
		BITMAPFILEHEADER bf;                      //ͼ���ļ�ͷ
		BITMAPINFOHEADER bi;                      //ͼ���ļ�ͷ��Ϣ

		fp = fopen(TexureFilename, "rb");
		if (fp == NULL)
		{
			throw new FileNotFoundException();
		}

		fread(&bf, sizeof(BITMAPFILEHEADER), 1, fp);//��ȡBMP�ļ�ͷ�ļ�
		fread(&bi, sizeof(BITMAPINFOHEADER), 1, fp);//��ȡBMP�ļ�ͷ�ļ���Ϣ
		textureWidth = bi.biWidth;                            //��ȡͼ��Ŀ�
		textureHeight = bi.biHeight;                           //��ȡͼ��ĸ�
		bitSize = bi.biSizeImage;                  //��ȡͼ���size
		buf = (char*)malloc(textureWidth*textureHeight * 3);                //���仺������С
		fseek(fp, long(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)), 0);//��λ��������ʼλ��
		fread(buf, 1, textureWidth*textureHeight * 3, fp);                   //��ʼ��ȡ����

		// ����
		std::cout << textureWidth << std::endl;
		std::cout << textureHeight << std::endl;

		// ����������,screen_h*screen_w * 3���ֽڴ�
		TextureBuffer = new BYTE[textureHeight*textureWidth * 3];

		p = buf;
		for (int j = 0; j < textureHeight; j++)
		{
			for (int i = 0; i < textureWidth; i++)
			{
				b = *p++; g = *p++; r = *p++;
				pix = RGB(r, g, b); // �����pix����ֱ����ȥʹ�ã���Ȼ�����������Ҳ���

				// �����ص���ɫ����������,ע����ɫ��˳��Ӧ����BGR
				TextureBuffer[i * textureWidth * 3 + (j + 1) * 3 - 1] = r;
				TextureBuffer[i * textureWidth * 3 + (j + 1) * 3 - 2] = g;
				TextureBuffer[i * textureWidth * 3 + (j + 1) * 3 - 3] = b;
			}
		}

		fclose(fp);
	}
}

#endif // !__Preperation__


