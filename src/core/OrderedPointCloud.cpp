#include "Global.h"
#include "OrderedPointCloud.h"
#include "GLViewer.h"

#include "../renderers/IPointCloudRenderer.h"

COrderedPointCloud::COrderedPointCloud(std::shared_ptr<CBaseObject> p) : CPointCloud(p)
{
	setLabel("o.cloud");
	m_cols = 0;
	m_rows = 0;

	renderer_ = std::make_shared<IPointCloudRenderer>();
}

COrderedPointCloud::COrderedPointCloud(COrderedPointCloud& m) : CPointCloud(m)
{
	setLabel("o.cloud");
	m_dist = m.m_dist;
	m_cols = m.m_cols;
	m_rows = m.m_rows;

	renderer_ = std::make_shared<IPointCloudRenderer>();
}

COrderedPointCloud::~COrderedPointCloud() 
{
}

std::wstring COrderedPointCloud::infoRow()
{
	std::wstring napis;

	napis = CPointCloud::infoRow(); // L"[id:" + std::to_wstring((long long)m_Id) + L"]";
	napis += L"\ncols:" + std::to_wstring(m_cols);
	napis += L"\nrows:" + std::to_wstring(m_rows);
	//napis += L", vn:" + std::to_wstring((long long)m_vnormals.size());

	return napis;
}

//void COrderedPointCloud::renderSelf()
//{
	//if (m_height > 1)
	//{
	//	renderAsMesh();
	//}
	//else
	//{
//		CPointCloud::renderSelf();
	//}
//}

#include "Face.h"

void COrderedPointCloud::renderAsMesh()
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	int step = 10;

	for (int iy = 0; iy < (m_rows - step); iy+=step)
	{
		for (int ix = 0; ix < (m_cols - step); ix+=step)
		{
			CVertex &v1(m_vertices[iy*m_cols + ix]);
			CVertex &v2(m_vertices[(iy+step)*m_cols + ix]);
			CVertex &v3(m_vertices[(iy+step)*m_cols + ix+step]);
			CVertex &v4(m_vertices[iy*m_cols + ix+step]);

			if ((v1 != CVertex(0, 0, 0)) && (v2 != CVertex(0, 0, 0)) && (v3 != CVertex(0, 0, 0)) && (v4 != CVertex(0, 0, 0)))
			{
				CVector3d v01 = CVector3d(CPoint3d(0, 0, 0), v1);
				CVector3d v02 = CVector3d(CPoint3d(0, 0, 0), v2);
				CVector3d v03 = CVector3d(CPoint3d(0, 0, 0), v3);
				CVector3d v04 = CVector3d(CPoint3d(0, 0, 0), v4);

				CVector3d v12 = CVector3d(v1, v2);
				CVector3d v13 = CVector3d(v1, v3);
				CVector3d v14 = CVector3d(v1, v4);

				double d1 = v01.length();
				double d2 = v02.length();
				double d3 = v02.length();
				double d4 = v04.length();

				double eps = d1 / 100.0;

				CVector3d n1 = CFace::triangleNormal(v1, v2, v3);
				CVector3d n2 = CFace::triangleNormal(v4, v1, v3);

				double a = n1.dotProduct(n2);

				double b = abs( v01.dotProduct(v12) );
				double c = abs( v01.dotProduct(v14) );

				//if (a > 0.5)
				if ( ( d2 < (d1+eps) ) && (d2 > (d1 - eps)) && (d3 < (d1 + eps)) && (d3 > (d1 - eps)) && (d4 < (d1 + eps)) && (d4 > (d1 - eps)))
				//if ( ( b < 0.1 ) && ( c < 0.1 ) )
				{

					glBegin(GL_TRIANGLES);

					if (hasVertexColors())
					{
						CRGBA &c1(m_vcolors[iy*m_cols + ix]);
						CRGBA &c2(m_vcolors[(iy + step)*m_cols + ix]);
						CRGBA &c3(m_vcolors[(iy + step)*m_cols + ix + step]);
						CRGBA &c4(m_vcolors[iy*m_cols + ix + step]);

						glNormal3dv(n1.toVector());

						glColor4ubv(c1.V());
						glVertex3fv(v1.toVector());
						glColor4ubv(c2.V());
						glVertex3fv(v2.toVector());
						glColor4ubv(c3.V());
						glVertex3fv(v3.toVector());

						glNormal3dv(n2.toVector());

						glColor4ubv(c4.V());
						glVertex3fv(v4.toVector());
						glColor4ubv(c1.V());
						glVertex3fv(v1.toVector());
						glColor4ubv(c3.V());
						glVertex3fv(v3.toVector());
					}
					else
					{
						glNormal3dv(n1.toVector());

						glVertex3fv(v1.toVector());
						glVertex3fv(v2.toVector());
						glVertex3fv(v3.toVector());

						glNormal3dv(n2.toVector());

						glVertex3fv(v4.toVector());
						glVertex3fv(v1.toVector());
						glVertex3fv(v3.toVector());
					}
					glEnd();
				}
			}
		}
	}
	glPopAttrib();
	glPopMatrix();
}

