#pragma once

#include "color.h"
#include "transform.h"

enum LightType
{
	Directional,
	Point,
	Spot
};

class Light
{
public:
	Transform transform;
	LightType type;
	Color color;
	float intensity;
	//����˥������
	float kc;
	//����˥������
	float kl;
	//����˥������
	float kq;
	//���Դ��Χ
	float range;
	//�۹����׶��
	float spotInnerAngle;
	//�۹����׶��
	float spotOuterAngle;
	//�۹��ָ������
	float pf;
};