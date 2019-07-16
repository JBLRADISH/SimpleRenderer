#pragma once

#include <vector>
#include "vector2.h"
#include "vector3.h"
#include "alignallocator.h"
#include "face.h"
using namespace std;

class Mesh
{
public:
	//SSE��Ҫ��ַ16�ֽڶ��� ����Ҫ�Զ���Allocator
	vector<Vector3, AlignmentAllocator<Vector3>> vertices;
	vector<Face> faces;
	vector<Vector3, AlignmentAllocator<Vector3>> normals;
	vector<Vector2> uv;

	int vertexCount();

	void RecalculateNormals();
};
