#pragma once
#include "vertex_t.h"

class edge_t
{
public:
	edge_t();
	~edge_t();

	vertex_t v1, v2, v;//v为经过插值之后的结果，v1,v2表示边的两个顶点

};

