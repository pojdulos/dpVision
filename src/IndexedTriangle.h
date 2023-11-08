#pragma once

#include "Triangle.h"

using namespace std;

class  DPVISION_EXPORT CIndexedTriangle : public CTriangle {
public:
	int m_idx;
	int m_iA, m_iB, m_iC;

	CIndexedTriangle();
	CIndexedTriangle(int idx, CMesh &mesh);
	~CIndexedTriangle();
};

