#pragma once

#include <vector>
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

	int vertexCount()
	{
		return vertices.size();
	}
};
