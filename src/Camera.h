#pragma once

#include "Wektor3D.h"

struct quaternion
{
	double x, y, z, w;

	double length()
	{
		return sqrt( x * x + y * y + z * z + w * w);
	};

	quaternion &normalize()
	{
		double L = this->length();

		x /= L;
		y /= L;
		z /= L;
		w /= L;
		return *this;
	};

	quaternion &conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	};

	static quaternion mult(quaternion A, quaternion B)
	{
		quaternion C;

		C.x = A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y;
		C.y = A.w*B.y - A.x*B.z + A.y*B.w + A.z*B.x;
		C.z = A.w*B.z + A.x*B.y - A.y*B.x + A.z*B.w;
		C.w = A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z;
		return C;
	};
};


class __declspec(dllexport) CCamera
{
public:
	CVector3<double> m_pos;
	CVector3<double> m_dir;
	CVector3<double> m_up;

	CCamera();
	~CCamera();

	void reset();

	void rotateCamera(double Angle, double x, double y, double z);

	void rotY(double a) {   }

};

