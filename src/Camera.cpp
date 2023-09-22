#include "Camera.h"



CCamera::CCamera()
{
	reset();
}


CCamera::~CCamera()
{
}

void CCamera::reset()
{
	m_pos.Set(0.0, 0.0, 200.0);
	m_dir.Set(0.0, 0.0, -1.0);
	m_up.Set(0.0, 1.0, 0.0);
}

	void CCamera::rotateCamera(double Angle, double x, double y, double z)
	{
		quaternion temp, quat_view, result;

		temp.x = x * sin(Angle / 2);
		temp.y = y * sin(Angle / 2);
		temp.z = z * sin(Angle / 2);
		temp.w = cos(Angle / 2);

		quat_view.x = m_dir.X();
		quat_view.y = m_dir.Y();
		quat_view.z = m_dir.Z();
		quat_view.w = 0;

		result = quaternion::mult( quaternion::mult(temp, quat_view), temp.conjugate() );

		m_dir.X(m_dir.X()+result.x );
		m_dir.Y(m_dir.Y() + result.y );
		m_dir.Z(m_dir.Z() + result.z );
	}
