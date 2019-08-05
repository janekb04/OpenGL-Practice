#pragma once

#include "External.h"
#include "texture.h"

struct Material
{
	Texture2D const * diffuse;
	Texture2D const * specular;
	Texture2D const * normal;
	float shininess;
};