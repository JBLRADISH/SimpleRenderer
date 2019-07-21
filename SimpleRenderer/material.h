#pragma once

#include "color.h"
#include "texture.h"

class Material
{
public:

	Color cAmbient;
	Color cDiffuse;
	Color cSpecular;
	//���淴��ָ��
	float power;

	Material()
	{
		cAmbient = Color::black;
		cDiffuse = Color::white;
		cSpecular = Color::black;
		power = 0.0f;
	}
};