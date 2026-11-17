#include "Global.h"
#include "AnnotationPlane.h"
#include "Model3D.h"
#include "Workspace.h"


#include "../renderers/IAnnotationPlaneRenderer.h"

namespace {
CVector3d fallbackPlaneAxis(const CVector3d& normal)
{
	CVector3d ref = (std::abs(normal.Z()) < 0.9) ? CVector3d::ZAxis() : CVector3d::XAxis();
	CVector3d axis = ref.crossProduct(normal).getNormalized();
	if (axis.length() == 0.0)
		axis = CVector3d::YAxis();
	return axis;
}
}

CAnnotationPlane::CAnnotationPlane(std::shared_ptr<CBaseObject> m) :CAnnotation(m), CPlane()
{
	setLabel("plane");
	m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);

	m_size = 10;
	ensureAxisU();

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

CAnnotationPlane::CAnnotationPlane(int objId) :CAnnotation(objId), CPlane()
{
	setLabel("plane");
	m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
	m_size = 10;
	ensureAxisU();

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

CAnnotationPlane::CAnnotationPlane(CAnnotationPlane& t) :CAnnotation(t), CPlane(t)
{
	//setLabel("plane");
	//m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	//m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
	m_size = t.m_size;
	m_axisU = t.m_axisU;
	ensureAxisU();

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

CAnnotationPlane::CAnnotationPlane(CPlane p) :CAnnotation(-1), CPlane(p)
{
	setLabel("plane");
	m_normal.normalize();
	m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
	m_size = 10;
	ensureAxisU();

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

CAnnotationPlane::CAnnotationPlane(CPoint3d p, CVector3d n) :CAnnotation(-1), CPlane(p, n)
{
	setLabel("plane");
	m_normal.normalize();
	m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
	m_size = 10;
	ensureAxisU();

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

void CAnnotationPlane::ensureAxisU()
{
	if (m_normal.length() == 0.0)
	{
		m_axisU = CVector3d::XAxis();
		return;
	}

	if (m_axisU.length() == 0.0)
	{
		m_axisU = fallbackPlaneAxis(m_normal);
		return;
	}

	CVector3d projected = m_axisU - m_normal * m_axisU.dotProduct(m_normal);
	if (projected.length() == 0.0)
	{
		m_axisU = fallbackPlaneAxis(m_normal);
		return;
	}

	m_axisU = projected.getNormalized();
}

void CAnnotationPlane::setAxisU(const CVector3d& axis)
{
	m_axisU = axis;
	ensureAxisU();
}

const CVector3d& CAnnotationPlane::axisU()
{
	ensureAxisU();
	return m_axisU;
}

void CAnnotationPlane::setNormal(CVector3d n)
{
	CPlane::setNormal(n);
	ensureAxisU();
}

void CAnnotationPlane::applyTransformation(CTransform &prevT, CTransform &newT)
{
	Eigen::Matrix4d transformMatrix = newT.toEigenMatrix4d().inverse() * prevT.toEigenMatrix4d();
	const CVector3d oldNormal = m_normal;
	const CVector3d oldAxisU = m_axisU;

	Eigen::Vector4d center(m_center.X(), m_center.Y(), m_center.Z(), 1.0);
	center = transformMatrix * center;
	m_center = CPoint3d(center);

	if (oldNormal.length())
	{
		Eigen::Matrix3d linearPart = transformMatrix.block<3, 3>(0, 0);
		Eigen::Vector3d normal = linearPart.inverse().transpose() * oldNormal.toVector3();
		m_normal = CVector3d(normal).getNormalized();

		Eigen::Vector3d axis = linearPart * oldAxisU.toVector3();
		setAxisU(CVector3d(axis));
	}

	m_d = recalc_D();
}

bool CAnnotationPlane::applyParentTransform()
{
	auto parent = getParentPtr();
	auto parentModel = std::dynamic_pointer_cast<CModel3D>(parent);
	if (parentModel == nullptr)
		return false;

	CTransform nullTransform;
	applyTransformation(parentModel->transform(), nullTransform);
	transform().reset();

	auto self = shared_from_this();
	auto grandpa = parent->getParentPtr();

	CWorkspace::instance()->_objectRemove(self);
	CWorkspace::instance()->_objectAdd(self, grandpa);

	return true;
}


std::wstring CAnnotationPlane::getInfoRow()
{
	std::wstring info = L"Point: [ X=" + std::to_wstring((long double)m_center.X()) + L", Y=" + std::to_wstring((long double)m_center.Y()) + L", Z=" + std::to_wstring((long double)m_center.Z()) + L" ]";
	info.append( L", normal: [ nX=" + std::to_wstring((long double)m_normal.X()) + L", nY=" + std::to_wstring((long double)m_normal.Y()) + L", nZ=" + std::to_wstring((long double)m_normal.Z()) + L" ]" );

	return info;
}

