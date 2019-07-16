#pragma once

#include "color.h"

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

	//�����ⷴ��ϵ��
	float ka;
	//ɢ��ⷴ��ϵ��
	float kd;
	//���淴���ķ���ϵ��
	float ks;
	//���淴��ָ��
	float power;

	Color cDiffuse;
	Color cSpecular;

	static Material CreateWireframe(const Color& c);

	static Material CreateConstant(const Color& c);

	static Material CreateFlat(const Color& c, float ka = 1.0f, float kd = 1.0f);

	static Material CreateGouraud(float ka = 0.0f, float kd = 1.0f, float ks = 0.0f, float power = 128.0f, const Color& cDiffuse = Color::white, const Color& cSpecular = Color::white);
};