#pragma once

#include "model.h"
#include "Transform.h"

struct GameObject
{
public:
	Model* const model;
	Transform transform;
};