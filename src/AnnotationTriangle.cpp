#include "Global.h"
#include "AnnotationTriangle.h"

#include "AP.h"

#include <Windows.h>
#include <GL/GL.h>

void CAnnotationTriangle::applyTransform(CTransform prevT, CTransform newT)
{
	m_pC = newT.w2l(prevT.l2w(m_pC));
	m_pA = newT.w2l(prevT.l2w(m_pA));
	m_pB = newT.w2l(prevT.l2w(m_pB));
}

std::wstring CAnnotationTriangle::getInfoRow()
{
	std::wstring info = L"A: [" + std::to_wstring((double) m_pA.X()) + L"," + std::to_wstring((double)m_pA.Y()) + L"," + std::to_wstring((double)m_pA.Z()) + L"]\n";
	info.append(L"B: [" + std::to_wstring((double)m_pB.X()) + L"," + std::to_wstring((double)m_pB.Y()) + L"," + std::to_wstring((double)m_pB.Z()) + L"]\n");
	info.append(L"C: [" + std::to_wstring((double)m_pC.X()) + L"," + std::to_wstring((double)m_pC.Y()) + L"," + std::to_wstring((double)m_pC.Z()) + L"]\n");

	return info;
}

void CAnnotationTriangle::renderSelf()
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	if (m_selected)
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	//glLineWidth(5);

	glBegin(GL_TRIANGLES);
		glVertex3dv(m_pA.toVector());
		glVertex3dv(m_pB.toVector());
		glVertex3dv(m_pC.toVector());
	glEnd();


	glPointSize(5);

	if (m_selected)
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
		glVertex3dv(m_pA.toVector());
		glVertex3dv(m_pB.toVector());
		glVertex3dv(m_pC.toVector());
	glEnd();


	glPopAttrib();
	glPopMatrix();
}
