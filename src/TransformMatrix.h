#pragma once

#include <string>
#include "Vector3.h"

class __declspec(dllexport) CTransformMatrix
{
	double m_data[16]; // OpenGL style matrix

public:
	CTransformMatrix();
	CTransformMatrix(CTransformMatrix&);
	CTransformMatrix(double d[16]);
	~CTransformMatrix();

	double* data() { return m_data; }
	double& at(int col, int row);

	std::string toString(bool transpose = true);
	void fromString(std::string str, bool transpose = true);

	void setIdentity();

	CVector3d translation() {
		return CVector3d(m_data[12], m_data[13], m_data[14]);
	};
	
	double scaleX() {
		return CVector3d(m_data[0], m_data[1], m_data[2]).length();
	};

	double scaleY() {
		return CVector3d(m_data[4], m_data[5], m_data[6]).length();
	};

	double scaleZ() {
		return CVector3d(m_data[8], m_data[9], m_data[10]).length();
	};


};

