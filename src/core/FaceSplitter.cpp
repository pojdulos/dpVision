#include "FaceSplitter.h"

#include "Mesh.h"

bool CFaceSplitter::splitOneFace(size_t i1, EDGE &edge)
{
	CVertex newVertex = (m->vertices()[edge.first] + m->vertices()[edge.second]) / 2;

	CFace &f1 = m->faces()[i1];
	CFace f1n;

	int i = f1.hasVertex(edge.second);
	if (i != -1)
	{
		size_t vidx;
		auto vv = newVertices.find(newVertex);
		if (vv != newVertices.end())
		{
			vidx = vv->second;
		}
		else
		{
			m->vertices().push_back(newVertex);
			vidx = m->vertices().size() - 1;
			newVertices[newVertex] = vidx;
		}

		switch (i)
		{
		case 0:
			f1n = CFace(f1.A(), f1.B(), vidx);
			f1.A(vidx);
			break;
		case 1:
			f1n = CFace(vidx, f1.B(), f1.C());
			f1.B(vidx);
			break;
		case 2:
			f1n = CFace(f1.A(), vidx, f1.C());
			f1.C(vidx);
			break;
		}

		m->faces().push_back(f1n);

		return true;
	}

	return false;
}


double CFaceSplitter::longestEdge(size_t fidx, EDGE &e)
{
	CFace &f = m->faces()[fidx];

	double dAB = CVector3d(m->vertices()[f.A()], m->vertices()[f.B()]).length();
	double dBC = CVector3d(m->vertices()[f.B()], m->vertices()[f.C()]).length();
	double dCA = CVector3d(m->vertices()[f.C()], m->vertices()[f.A()]).length();

	double l = dAB;
	EDGE edge(f.A(), f.B());

	if (dBC > l)
	{
		l = dBC;
		edge = EDGE(f.B(), f.C());
	}

	if (dCA > l)
	{
		l = dCA;
		edge = EDGE(f.C(), f.A());
	}

	e = edge;
	return l;
}

void CFaceSplitter::splitFaces(double EPS)
{
	bigFaces2.clear();
	newVertices.clear();

	for (int fi = 0; fi < m->faces().size(); fi++)
	{
		EDGE e;
		double ll = longestEdge(fi, e);

		if (ll > EPS)
		{
			bigFaces2[DI(ll, fi)] = e;
		}
	}

	double maxD = 0.0;
	int k = 0;

	do
	{
		auto maxFace2 = prev(bigFaces2.rbegin().base());

		maxD = maxFace2->first.first;
		int maxIdx = maxFace2->first.second;
		EDGE maxEdge = maxFace2->second;

		if ( splitOneFace(maxIdx, maxEdge) )
		{
			// nowo utworzona œcianka zosta³a dodana na koñcu (push_back)
			size_t newIdx = m->faces().size() - 1;

			double tmpD;
			EDGE tmpE;

			bigFaces2.erase(maxFace2);

			tmpD = longestEdge(maxIdx, tmpE);
			if (tmpD > EPS)
			{
				bigFaces2[DI(tmpD, maxIdx)] = tmpE;
			}

			tmpD = longestEdge(newIdx, tmpE);
			if (tmpD > EPS)
			{
				bigFaces2[DI(tmpD, newIdx)] = tmpE;
			}
		}

		UI::STATUSBAR::printfTimed(500, "loop: %d, maxLL: %lf, bigFaces: %d", k++, maxD, bigFaces2.size());

	} while ((bigFaces2.size() > 0) && (maxD > EPS));

	UI::STATUSBAR::setText("DONE !");
}
