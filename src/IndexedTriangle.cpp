#include "IndexedTriangle.h"

//using namespace std;

#include "Mesh.h"

CIndexedTriangle::~CIndexedTriangle() {}

CIndexedTriangle::CIndexedTriangle() : CTriangle() {}

CIndexedTriangle::CIndexedTriangle(int idx, CMesh &mesh) : CTriangle( idx, mesh )
{
	m_iA = mesh.faces()[idx].A();
	m_iB = mesh.faces()[idx].B();
	m_iC = mesh.faces()[idx].C();

	m_idx = idx;
}


