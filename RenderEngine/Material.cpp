#include "Material.h"

namespace RenderEngine {

	Material::Material() :
		normalMap(nullptr),
		diffuseMap(nullptr),
		specularMap(nullptr),
		castShadow(true),
		receiveShadow(true),
		transparent(false)
	{
	}

	Material::~Material()
	{
	}
}
