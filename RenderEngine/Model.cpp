#include "Model.h"
#include<sstream>
#include<fstream>

namespace RenderEngine {

	Model::Model(const char *filename)
	{
		LoadModel(filename);
		material = std::make_shared<Material>();
	}

	Model::~Model() {}

	void Model::LoadModel(const char *filename)
	{
		std::vector<Vector4> verts;
		std::vector<Vector3> texs;
		std::vector<Vector4> norms;
		std::ifstream in;
		in.open(filename, std::ifstream::in);
		if (in.fail()) return;
		std::string line;
		float maxVx = -FLT_MAX, maxVy = -FLT_MAX, maxVz = -FLT_MAX;
		float minVx = FLT_MAX, minVy = FLT_MAX, minVz = FLT_MAX;
		while (!in.eof()) {
			std::getline(in, line);
			std::istringstream iss(line.c_str());
			char trash;
			if (!line.compare(0, 2, "v ")) {
				iss >> trash;
				float x, y, z;
				iss >> x;
				iss >> y;
				iss >> z;
				Vector4 v(x, y, z, 1.f);
				verts.push_back(v);
				maxVx = std::fmaxf(maxVx, v.getX());
				maxVy = std::fmaxf(maxVy, v.getY());
				maxVz = std::fmaxf(maxVz, v.getZ());
				minVx = std::fminf(minVx, v.getX());
				minVy = std::fminf(minVy, v.getY());
				minVz = std::fminf(minVz, v.getZ());
			}
			else if (!line.compare(0, 3, "vt ")) {
				iss >> trash;
				iss >> trash;
				float x, y;
				iss >> x;
				iss >> y;
				if (x > 1.0f) x -= std::floor(x);
				if (y > 1.0f) y -= std::floor(y);
				Vector3 t(x, y, 0.f);
				texs.push_back(t);
			}
			else if (!line.compare(0, 3, "vn ")) {
				iss >> trash;
				iss >> trash;
				float x, y, z;
				iss >> x;
				iss >> y;
				iss >> z;
				Vector4 n(x, y, z, 0.f);
				norms.push_back(n);
			}
			else if (!line.compare(0, 2, "f ")) {
				std::vector<int> v;
				std::vector<int> t;
				std::vector<int> n;
				int vx, vy, vz;
				int tx, ty, tz;
				int nx, ny, nz;

				iss >> trash;
				iss >> vx >> trash >> tx >> trash >> nx >>
					vy >> trash >> ty >> trash >> ny >>
					vz >> trash >> tz >> trash >> nz;

				// in wavefront obj all indices start at 1, not zero
				vx--; tx--; nx--;
				vy--; ty--; ny--;
				vz--; tz--; nz--;
				// default color of vertex is white
				Vertex vert0(verts[vx], norms[nx], Colour(1.f, 1.f, 1.f), texs[tx]);
				Vertex vert1(verts[vy], norms[ny], Colour(1.f, 1.f, 1.f), texs[ty]);
				Vertex vert2(verts[vz], norms[nz], Colour(1.f, 1.f, 1.f), texs[tz]);
				Triangle triangle(vert0, vert1, vert2);
				faces.push_back(triangle);
			}
		}
		center = Vector4((maxVx + minVx) / 2, (maxVy + minVy) / 2, (maxVz + minVz) / 2, 1.0f);
		std::cerr << "v#  " << verts.size() <<
			"		  vn# " << norms.size() <<
			"		  vt# " << texs.size() <<
			"		  f#  " << faces.size() << std::endl;
		in.close();
	}

	ModelArray::ModelArray() {}

	ModelArray::~ModelArray() {}

	void ModelArray::LoadModel(const char *filename)
	{
		auto model = std::make_shared<Model>(filename);
		models.push_back(model);
	}

}
