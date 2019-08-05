#pragma once

#include "material.h"
#include "mesh.h"

class Model
{
public:
	Material const * material;
	Mesh const * mesh;
};