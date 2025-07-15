#include "Global.h"
#include "AnnotationBezier.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "StatusBarManager.h"

std::wstring CAnnotationBezier::getInfoRow()
{
	std::wstring info = L"bezier (id:" + std::to_wstring((long long)m_Id ) + L"), size: " + std::to_wstring((long long) m_list.size()) + L" points.";

	return info;
}

#include "../renderers/IAnnotationBezierRenderer.h"

inline CAnnotationBezier::CAnnotationBezier(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("bezier");
	m_type = 0;

	renderer_ = std::make_shared<IAnnotationBezierRenderer>();
}

CPoint3d CAnnotationBezier::deCastelJau(double u, int count, int startIndex)
{
	std::vector<CPoint3d> iTPoints;
	for ( int i = 0; i < count; i++ )
	{
		int idx = ( startIndex + i ) % m_list.size();
		iTPoints.push_back(m_list[idx]);
	}

	double uM1 = 1.0 - u;
	for (int j = count - 1; j > 0; --j)
	{
		for (int i = 0; i < j; ++i)
		{
			iTPoints[i] = ( iTPoints[i] * uM1 + iTPoints[i + 1] * u ); /*X,Y,Z*/
		}
	}
	return iTPoints[0];
}


std::vector<CPoint3d> CAnnotationBezier::krzywa()
{
	int n = 7;
	int c = 2;
	int g = 10;
	
	int div = g * n;
	int cut = g * c;

	std::vector< std::vector<CPoint3d> > wyliczone( m_list.size()-1 );
	std::vector<CPoint3d> ret;

	for (int idx = 0; idx < m_list.size(); idx++)	// kolejne segmenty
	{
		for (int i = cut; i <= div-cut; i++) // bior� �rodkowe wartosci w segmencie
		{
			CPoint3d p = deCastelJau( (GLdouble)i / div, n, idx );
			wyliczone[idx].push_back(p);
		}
	}

	for (int idx = 0; idx < m_list.size(); idx++)	// kolejne segmenty
	{
		for (int j = (div + 1) / 2; j > 0; j--)
		{
			CPoint3d p = wyliczone[idx][j];
		}
	}

	return ret;
}

#include "tinysplinecpp.h"

void CAnnotationBezier::createTinySpline(int deg, int den)
{
	int size = m_list.size();
	int step = 2*deg;
	int splajnsize = size + ( 2 * step );

	try {
		tinyspline::BSpline spline( splajnsize, 3, deg, TS_CLAMPED );

		// ---------------------------------------------------------------------------------------

		std::vector<tinyspline::real> ctrlp = spline.ctrlp();
		
		for (int j = 0; j < splajnsize; j++)	// kolejne segmenty
		{
			int idx = (size + j - step) % size;
			
			ctrlp[3 * j + 0] = m_list[idx].X();
			ctrlp[3 * j + 1] = m_list[idx].Y();
			ctrlp[3 * j + 2] = m_list[idx].Z();
		}

		spline.setCtrlp(ctrlp);

		// ---------------------------------------------------------------------------------------

		int div = splajnsize * den;
		
		int cutoff = ( den * step ) / 2;

		std::vector<CPoint3d> tmp_spline;
		
		for (int i = cutoff; i < div-cutoff; i++)
		{
			std::vector<tinyspline::real> result = spline.evaluate((GLdouble)i / div).result();

			CPoint3d p(result[0], result[1], result[2]);

			tmp_spline.push_back(p);
		}

		// ---------------------------------------------------------------------------------------

		double d = CVector3d(tmp_spline[0], tmp_spline[tmp_spline.size() - 1]).length();
		int n = 0;
		for (int i = 1; i < step * den; i++)
		{
			double d1 = CVector3d(tmp_spline[i], tmp_spline[tmp_spline.size() - (i+1)]).length();

			if (d1 < d)
			{
				d = d1;
				n = i;
			}
		}

		m_spline.clear();
		for (int i = n; i < tmp_spline.size() - n; i++)
		{
			m_spline.push_back( tmp_spline[i] );
		}

	}
	catch (std::exception e) {
		StatusBarManager::setText(QString(e.what()));
	}
}

void CAnnotationBezier::drawTinySpline()
{
	glLineWidth(2.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < m_spline.size(); i++)
	{
		glVertex3dv( m_spline[i].toVector() );
	}
	glEnd();

	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < m_spline.size(); i++)
	{
		glVertex3dv(m_spline[i].toVector());
	}
	glEnd();
}

void CAnnotationBezier::applyTransformation(CTransform &prevT, CTransform &newT)
{
	for (std::vector<CPoint3d>::iterator ip = this->m_list.begin(); ip != this->m_list.end(); ip++)
	{
		ip->Set(newT.w2l(prevT.l2w(*ip)));
	}

	for (std::vector<CPoint3d>::iterator ip = this->m_spline.begin(); ip != this->m_spline.end(); ip++)
	{
		ip->Set(newT.w2l(prevT.l2w(*ip)));
	}
}

void CAnnotationBezier::drawBeziers(int deg, int den, int cut )
{
	int div = deg * den;

	for (int idx = 0; idx < m_list.size(); idx ++)
	{
		glBegin(GL_LINE_STRIP);

		for (int i = cut; i <= div - cut; i++)
		{
			CPoint3d p = deCastelJau((GLdouble)i / div, deg, idx);
			glVertex3dv(p.toVector());
		}

		glEnd();
	}
}

void CAnnotationBezier::renderSelf()
{
	if (this->m_list.size() > 1)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glLineWidth(3.0);
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

		if ( m_type == 1 ) drawTinySpline();
		else drawBeziers(8, 10, 34);

		glPointSize(10.0);
		glBegin(GL_POINTS);
		//for (int i = 0; i < m_list.size(); i++) glVertex3fv(&m_list[i].toVector()[0]);
		glColor4ubv( CRGBA( 0.0f, 1.0f, 0.0f, 1.0f ).V() );
		glVertex3dv( m_spline[0].toVector() );
		glColor4ubv(CRGBA(1.0f, 0.0f, 0.0f, 1.0f).V());
		glVertex3dv( m_spline[m_spline.size()-1].toVector() );

		glEnd();



		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}
