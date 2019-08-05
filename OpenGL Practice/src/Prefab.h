#pragma once

#include "External.h"
#include "mesh.h"
#include "texture.h"
#include "material.h"
#include "model.h"

class Prefab
{
	Texture2D diffuse;
	Texture2D specular;
	Material material;
	Mesh mesh;
	Model model;
};