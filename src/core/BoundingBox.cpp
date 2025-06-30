#include "BoundingBox.h"
#include "stdafx.h"
#include "BoundingBox.h"

#include "BoundVector.h"

CBoundingBox::CBoundingBox(InitialValues v)
{
	reset(v);
}

CBoundingBox::CBoundingBox(const CBoundingBox &bb)
{
	m_min = bb.m_min;
	m_max = bb.m_max;
}

CBoundingBox::CBoundingBox(CPoint3d min, CPoint3d max)
{
	m_min = min;
	m_max = max;
}

CBoundingBox::CBoundingBox( CPoint3d a, CPoint3d b, CPoint3d c )
{
	m_min = a;
	m_max = a;

	expand( b );
	expand( c );
}



CBoundingBox::~CBoundingBox()
{
}

#include "GLViewer.h"

void CBoundingBox::draw( Style style, bool checked)
{
	if ( isInvalid() || (style == DontRender) ) return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glLineWidth(1.0);

	if ( style == NotSelected )
		glColor3f(0.2f, 0.2f, 0.2f);
	else
		if ( style == Locked )
			glColor3f(1.0f, 0.0f, 0.0f);
		else
			glColor3f(0.0f, 0.5f, 0.0f);

	drawBox();

	if ( checked )
	{
		glLineWidth(3.0);
		glColor4f(0.5f, 0.5f, 0.0f, 0.5f);
		drawBox(true);
	}

	glDisable(GL_COLOR_MATERIAL);

	glPopAttrib();
}

void CBoundingBox::drawBox(bool dashed)
{
	if (dashed)
	{
		glLineStipple(3, 0x00FF);
		glEnable(GL_LINE_STIPPLE);
	}

	glPushAttrib(GL_ENABLE_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUAD_STRIP);
	glVertex3d(m_max.X(), m_min.Y(), m_min.Z());
	glVertex3d(m_max.X(), m_min.Y(), m_max.Z());

	glVertex3d(m_min.X(), m_min.Y(), m_min.Z());
	glVertex3d(m_min.X(), m_min.Y(), m_max.Z());

	glVertex3d(m_min.X(), m_max.Y(), m_min.Z());
	glVertex3d(m_min.X(), m_max.Y(), m_max.Z());

	glVertex3d(m_max.X(), m_max.Y(), m_min.Z());
	glVertex3d(m_max.X(), m_max.Y(), m_max.Z());

	glVertex3d(m_max.X(), m_min.Y(), m_min.Z());
	glVertex3d(m_max.X(), m_min.Y(), m_max.Z());
	glEnd();

	glPopAttrib();
}

QVector<CPoint3d> CBoundingBox::getCorners()
{
	return { m_min,
		CPoint3d(m_min.x, m_min.y, m_max.z),
		CPoint3d(m_min.x, m_max.y, m_max.z),
		CPoint3d(m_min.x, m_max.y, m_min.z),
		CPoint3d(m_max.x, m_max.y, m_min.z),
		CPoint3d(m_max.x, m_min.y, m_min.z),
		CPoint3d(m_max.x, m_min.y, m_max.z),
		m_max };
}

void CBoundingBox::reset(InitialValues v)
{
	switch (v)
	{
		case NullBB:
			m_min = CPoint3d(0, 0, 0);
			m_max = CPoint3d(0, 0, 0);
			break;
		case InvalidBB:
		default:
			m_min = CPoint3d(DBL_MAX, DBL_MAX, DBL_MAX);
			m_max = CPoint3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
			break;
	}
}

bool CBoundingBox::intersects(CBoundVector ray)
{
	double tmin = ( m_min.X() - ray.origin().X() ) / ray.direction().X();
	double tmax = ( m_max.X() - ray.origin().X() ) / ray.direction().X();

	if ( tmin > tmax ) std::swap(tmin, tmax);

	double tymin = ( m_min.Y() - ray.origin().Y() ) / ray.direction().Y();
	double tymax = ( m_max.Y() - ray.origin().Y() ) / ray.direction().Y();

	if ( tymin > tymax ) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	double tzmin = ( m_min.Z() - ray.origin().Z() ) / ray.direction().Z();
	double tzmax = ( m_max.Z() - ray.origin().Z() ) / ray.direction().Z();

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

bool CBoundingBox::intersects(CBoundingBox b)
{
	//if (isInvalid() || b.isInvalid())
	//{
	//	return false;
	//}
	//else
		if ((m_max.X() < b.m_min.X()) || (m_max.Y() < b.m_min.Y()) || (m_max.Z() < b.m_min.Z()))
		{
			return false;
		}
		else
			if ((b.m_max.X() < m_min.X()) || (b.m_max.Y() < m_min.Y()) || (b.m_max.Z() < m_min.Z()))
			{
				return false;
			}
	return true;
}

CBoundingBox CBoundingBox::intersection(CBoundingBox a, CBoundingBox b)
{
	if (a.intersects(b))
	{
		CBoundingBox c;

		c.m_min = a.m_min;
		c.m_min.SetIfBigger(b.m_min);

		c.m_max = a.m_max;
		c.m_max.SetIfSmaller(b.m_max);

		return c;
	}
	else
	{
		return CBoundingBox(InvalidBB);
	}
}

void CBoundingBox::expand(const CBoundingBox b)
{
	m_min.SetIfSmaller(b.m_min);
	m_max.SetIfBigger(b.m_max);
}

void CBoundingBox::expand(const CPoint3d p)
{
	m_min.SetIfSmaller(p);
	m_max.SetIfBigger(p);
}

void CBoundingBox::expand(double val)
{
	CVector3d v(val, val, val);
	
	m_min = m_min - v;
	m_max = m_max + v;
}

bool CBoundingBox::contains(const CPoint3d & p)
{
	if ( (p.X() < m_min.X()) || (p.Y() < m_min.Y()) || (p.Z() < m_min.Z()) )
		return false;

	if ( (p.X() > m_max.X()) || (p.Y() > m_max.Y()) || (p.Z() > m_max.Z()) )
		return false;

	return true;
}



#define NUMDIM	3
#define RIGHT	0
#define LEFT	1
#define MIDDLE	2

bool CBoundingBox::hit(CPoint3d origin, CVector3d dir, CPoint3d & coord)
{
	bool inside = true;
	char quadrant[NUMDIM];
	register int i;
	int whichPlane;
	double maxT[NUMDIM];
	double candidatePlane[NUMDIM];

	/* Find candidate planes; this loop can be avoided if
	rays cast all from the eye(assume perpsective view) */
	for (i = 0; i < NUMDIM; i++)
		if (origin[i] < m_min[i]) {
			quadrant[i] = LEFT;
			candidatePlane[i] = m_min[i];
			inside = false;
		}
		else if (origin[i] > m_max[i]) {
			quadrant[i] = RIGHT;
			candidatePlane[i] = m_max[i];
			inside = false;
		}
		else {
			quadrant[i] = MIDDLE;
		}

	/* Ray origin inside bounding box */
	if (inside) {
		coord = origin;
		return (true);
	}

	/* Calculate T distances to candidate planes */
	for (i = 0; i < NUMDIM; i++)
		if (quadrant[i] != MIDDLE && dir[i] != 0.)
			maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < NUMDIM; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) return (false);
	for (i = 0; i < NUMDIM; i++)
		if (whichPlane != i) {
			coord[i] = origin[i] + maxT[whichPlane] * dir[i];
			if (coord[i] < m_min[i] || coord[i] > m_max[i])
				return (false);
		}
		else {
			coord[i] = candidatePlane[i];
		}
	return (true);				/* ray hits box */
}

int CBoundingBox::longest_axis()
{
	CVector3d m_whd(m_min, m_max);

	if (m_whd.X() > m_whd.Y())
	{
		if (m_whd.Y() > m_whd.Z())
		{
			return 0;
		}

		if (m_whd.Z() > m_whd.X() )
		{
			return 2;
		}
	}
	else if (m_whd.Z() > m_whd.Y())
	{
		return 2;
	}

	return 1;
}

