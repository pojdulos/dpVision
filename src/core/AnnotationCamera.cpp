#include "Global.h"
#include "AnnotationCamera.h"
#include "Model3D.h"

#include "AP.h"
#include "UI.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <regex>

CAnnotationCamera* CAnnotationCamera::create(Sensor s, double matrix[16], std::string label)
{
	CAnnotationCamera* ad = new CAnnotationCamera();

	ad->m_sensor = s;
	
	ad->setMatrix(matrix);

	ad->setLabel(UI::str2wstr(label));

	//ad->setColor(CRGBA::fromHexRGBA("#007bff66"));

	return ad;
}

CAnnotationCamera* CAnnotationCamera::create(Sensor s, std::string matrixSTR, std::string label)
{
	CAnnotationCamera* ad = new CAnnotationCamera();

	ad->m_sensor = s;

	ad->setMatrix(matrixSTR);

	ad->setLabel(UI::str2wstr(label));

	ad->setColor(CRGBA::fromHexRGBA("#007bff66"));
	ad->setSelColor(CRGBA::fromHexRGBA("#ff000066"));

	return ad;
}


std::vector<std::string> resplit(const std::string& s, std::string rgx_str = "\\s+")
{
	std::vector<std::string> elems;

	std::regex rgx(rgx_str);

	std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
	std::sregex_token_iterator end;

	while (iter != end) {
		//std::cout << "S43:" << *iter << std::endl;
		elems.push_back(*iter);
		++iter;
	}

	return elems;

}

inline void CAnnotationCamera::setMatrix(std::string matrixSTR)
{
	std::regex rgx("\\s+");

	std::sregex_token_iterator iter(matrixSTR.begin(), matrixSTR.end(), rgx, -1);
	std::sregex_token_iterator end;

	int row = 0;
	int col = 0;
	while (iter != end) {
		matrix(col, row) = std::stod(*iter);
		++iter;

		col++;
		if (col > 3)
		{
			row++;
			col = 0;
		}
	}
}


inline void CAnnotationCamera::setMatrix(double matrix[16])
{
	m_matrix[0] = matrix[0];
	m_matrix[1] = matrix[4];
	m_matrix[2] = matrix[8];
	m_matrix[3] = matrix[12];
	m_matrix[4] = matrix[1];
	m_matrix[5] = matrix[5];
	m_matrix[6] = matrix[9];
	m_matrix[7] = matrix[13];
	m_matrix[8] = matrix[2];
	m_matrix[9] = matrix[6];
	m_matrix[10] = matrix[10];
	m_matrix[11] = matrix[14];
	m_matrix[12] = matrix[3];
	m_matrix[13] = matrix[7];
	m_matrix[14] = matrix[11];
	m_matrix[15] = matrix[15];

	//for (int i = 0; i < 16; i++)
	//{
	//	m_matrix[i] = matrix[i];
	//}
}

std::wstring CAnnotationCamera::getInfoRow()
{
	std::wstring info = L"Camera";

	return info;
}

void CAnnotationCamera::renderSelf()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	//glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	

	glMultMatrixd(m_matrix);

	if ( m_parent->isSelected() || m_selected )
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	double scale = 1.0;
	CBaseObject* parent = m_parent;
	if (parent != nullptr)
	{
		while (parent->getParent() != nullptr)
		{
			scale *= ((CAnnotation*)parent)->getTransform().scale().x;
			parent = parent->getParent();
		}
		scale *= ((CModel3D*)parent)->getTransform().scale().x;
	}

	double aspect = m_sensor.realWidth / m_sensor.realHeight;
	double sX = 0.40 / scale;
	double sY = sX / aspect;
	//double f = displayScale * m_sensor.mmFocalLength;

	glLineWidth(3);

	glBegin( GL_QUADS );
	/*glVertex3d(-sX, -sY, f);
	glVertex3d(sX, -sY, f);
	glVertex3d(sX, sY, f);
	glVertex3d(-sX, sY, f);
	*/glVertex3d(-sX, -sY, 0);
	glVertex3d(sX, -sY, 0);
	glVertex3d(sX, sY, 0);
	glVertex3d(-sX, sY, 0);
	glEnd();


	glColor4ub(0, 0, 0, 255);
	glBegin(GL_LINES);
	glVertex3d(-sX, -sY, 0); glVertex3d(sX, -sY, 0);
	glVertex3d(sX, -sY, 0); glVertex3d(sX, sY, 0);
	glVertex3d(sX, sY, 0); glVertex3d(-sX, sY, 0);
	glVertex3d(-sX, sY, 0); glVertex3d(-sX, -sY, 0);
	glEnd();


	//glColor4ub(255, 255, 0, 255);

	//glPolygonMode(GL_FRONT, GL_LINE);
	//glPolygonMode(GL_BACK, GL_LINE);


	//glBegin(GL_TRIANGLES);
	//glVertex3d(0.0, 0.0, 0.0);
	//glVertex3d(-sX, sY, f);
	//glVertex3d(sX, sY, f);

	//glVertex3d(0.0, 0.0, 0.0);
	//glVertex3d(sX, -sY, f);
	//glVertex3d(-sX, -sY, f);
	//glEnd();

	//glBegin(GL_LINES);
	//	glVertex3d(0.0, 0.0, 0.0);
	//	glVertex3d(-sX, sY, f);

	//	glVertex3d(0.0, 0.0, 0.0);
	//	glVertex3d(sX, sY, f);

	//	glVertex3d(0.0, 0.0, 0.0);
	//	glVertex3d(sX, -sY, f);

	//	glVertex3d(0.0, 0.0, 0.0);
	//	glVertex3d(-sX, -sY, f);
	//glEnd();

	glPopAttrib();
	glPopMatrix();
}
