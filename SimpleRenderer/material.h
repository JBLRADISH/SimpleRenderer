#pragma once

#include "color.h"
#include "texture.h"

enum ShadingMode
{
	Wireframe,//�߿�ģʽ
	Constant, //�̶���ɫ
	Flat, //�㶨��ɫ
	Gouraud //�𶥵���ɫ
};

class Material
{
public:
	ShadingMode shadingMode;

	Color cAmbient;
	Color cDiffuse;
	Color cSpecular;

	//���淴��ָ��
	float power;

	Texture diffuseTex;
	Texture specularTex;
	Texture normalTex;

	Material()
	{
		shadingMode = ShadingMode::Wireframe;
		cAmbient = Color::black;
		cDiffuse = Color::white;
		cSpecular = Color::black;
		power = 0.0f;
	}
};