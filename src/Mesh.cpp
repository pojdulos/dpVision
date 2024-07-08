#include "Mesh.h"
#include "Model3D.h"
#include "UI.h"

CMesh::CMesh(CModel3D *p) :CPointCloud(p)
{
	setLabel("mesh");

	renderWithTexture(false);
	renderFacesAsPoints(false);
	renderFacesAsEdges(false);
	renderSmoothVertex(false);

	m_poly.setParent(this);
}


CMesh::CMesh( CMesh &m ) : CPointCloud(m)
{
	m_poly = m.m_poly;
	m_poly.setParent(this);

	vsplits = std::vector<SVsplit>(m.vsplits);

	orgsize = m.orgsize;

	version = m.version;

	renderFacesAsPoints( m.renderFacesAsPoints() );
	renderFacesAsEdges( m.renderFacesAsEdges() );

	renderSmoothVertex( hasVertexNormals() );

	renderWithTexture( hasTexture() && getMaterial().hasTextureCoords() && getMaterial().hasTextureIndexes());
}

CMesh::CMesh(CPointCloud & cloud) : CPointCloud(cloud)
{
	renderWithTexture(false);
	renderFacesAsPoints(false);
	renderFacesAsEdges(false);
	renderSmoothVertex(false);

	m_poly.setParent(this);
}

CMesh::~CMesh(void)
{
}

CMesh * CMesh::fromPointCloud(CPointCloud &cloud)
{
	return new CMesh( cloud );
}


void CMesh::renderSelf()
{
	if ((m_poly.size() == 0) )//|| renderFacesAsPoints())
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		CPointCloud::renderSelf();
		glPopAttrib();
	}
	else
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		m_poly.renderSelf();
		glPopAttrib();
	}
}


int CMesh::rayTriangleIntersect3D(CPoint3f pA, CPoint3f pB, CPoint3f pC, CVector3f vN, CVector3f vRay, CPoint3f pP0, CPoint3f &pIntersectionPoint, double &pDistance)
{
	CPoint3d iP(pIntersectionPoint);
	int res = rayTriangleIntersect3D((CPoint3d)pA, (CPoint3d)pB, (CPoint3d)pC, (CVector3d)vN, (CVector3d)vRay, (CPoint3d)pP0, iP, pDistance);
	pIntersectionPoint = iP;
	return res;
}

int CMesh::rayTriangleIntersect3D(CPoint3d pA, CPoint3d pB, CPoint3d pC, CVector3d vN, CVector3d vRay, CPoint3d pP0, CPoint3d &pIntersectionPoint, double &pDistance)
{
	//	Return:		-1 = trójkąt jest nieprawidłowy ( segment lub punkt )
	//				-2 =  promień leży na płaszczyźnie trójkąta
	//				0 =  trójkąt i promień są rozłaczne ( brak punktu przecięcia )
	//				1 =  OK - znaleziono punkt przecięcia
	//				2 =  OK - ale promień trafia w trójkąt "od tyłu"

	// liczba możliwie bliska zeru, żeby uniknąc błędu przy dzieleniu
	double prawieZero = 0.00000001;

	CVector3d u = CVector3d(pA, pB);
	CVector3d v = CVector3d(pA, pC);

	CVector3d vNorm(vN);
	
	if (0 == vN.length())
	{
		vNorm = u.crossProduct( v ); // iloczyn wektorowy !
	}

	if (0 == vNorm.length())
	{
		return -1; // trójkąt nie jest trójkątem - co najmniej jedna krawędź ma długość zerową
	}

	// wektor od wierzchołka trójkąta do punktu na promieniu
	CVector3d w0 = CVector3d(pA, pP0);

	// iloczyn skalarny -> zero jeśli wektory prostopadłe
	double a = -vNorm.dotProduct(w0); // 0 -> w0 prostopadły do vNorm -> punkt pP0 leży na płaszczyźnie trójkąta

	double b = vNorm.dotProduct(vRay);	// b == 0 -> vRay prostopadły do vNorm -> vRay jest równoległy do trójkąta
								// b < 0  -> vRay wpada od przodu -> OK
								// b > 0  -> vRay wpada od tyłu -> NIE OK

	//if ( fabs(b) < prawieZero ) // to jest chyba niepotrzebne o ile sie nie pojawią błędy
	if (b == 0)
	{ // vRay jest równoległy do płaszczyzny trójkąta
		if (a != 0)
		{ // vRay nie ma punktu wspólnego z płaszczyzną trójkąta
			return 0;
		}
		else
		{ // vRay leży na płaszczyźnie trójkąta
		  //
		  // teoretycznie można by tu zaznaczać pierwszy trafiony punkt (na krawędzi trójkąta)
		  //
			return -2;
		}
	}

	double r = a / b;
	//if (r < 0.0)
	//{ // promień oddala się od trójkąta -> brak przecięcia
	//	return 0;    
	//}
	// Może się przydać, ale tu nie moge tego brać pod uwagę,
	// bo mój "wyjściowy" punkt na promieniu może równie dobrze leżeć
	// nad płaszczyzną trójkąta (r>0), pod nią (r<0) jak i na niej (r=0).


	// Wyznaczam punkt przecięcia promienia z płaszczyzna ściany
	CVector3d vec = vRay*r;

	pDistance = abs( r );
	pIntersectionPoint = pP0 + vec;

	// --------------------------------------------------------------
	// Teraz sprawdzam czy pIntersectionPoint lezy w trójkącie tFace
	// --------------------------------------------------------------
	// to jest wyznaczane ze współrzędnych barycentrycznych

	double uu = u.dotProduct(u);
	double uv = u.dotProduct(v);
	double vv = v.dotProduct(v);

	CVector3d w = CVector3d(pA, pIntersectionPoint);

	double wu = w.dotProduct(u);
	double wv = w.dotProduct(v);

	double D = uv * uv - uu * vv;

	// get and test parametric coords
	double s = (uv * wv - vv * wu) / D;

	if (s < 0.0 || s > 1.0)         // pIntersectionPoint leży poza trójkątem
		return 0;

	double t = (uv * wu - uu * wv) / D;

	if (t < 0.0 || (s + t) > 1.0)  // pIntersectionPoint leży poza trójkątem
		return 0;

	// Tu juz wiem, że punkt przecięcia leży w trójkącie,
	// ale jeszcze sprawdzam od której strony promień wchodzi w ściankę
	if (b > 0.0)
	{ // vRay trafia w trójkąt "od tyłu"
		return 2;
	}

	return 1; // pIntersectionPoint leży na trójkącie tFace
}


int CMesh::rayTriangleIntersect3D( INDEX_TYPE j, CVector3f vRay, CPoint3f pP0, CPoint3f &pIntersectionPoint, double &pDistance )
{
	CFace tFace = m_poly[j];
	CPoint3f pA = m_vertices[tFace.A()];
	CPoint3f pB = m_vertices[tFace.B()];
	CPoint3f pC = m_vertices[tFace.C()];

	CVector3f vN;
	if (m_poly.normals().size() > j) vN = m_poly.normals()[j];


	return CMesh::rayTriangleIntersect3D( pA, pB, pC, vN, vRay, pP0, pIntersectionPoint, pDistance );
}


int CMesh::rayTriangleIntersect3D(INDEX_TYPE j, CVector3d vRay, CPoint3d pP0, CPoint3d &pIntersectionPoint, double &pDistance)
{
	CFace tFace = m_poly[j];
	CPoint3d pA = m_vertices[tFace.A()];
	CPoint3d pB = m_vertices[tFace.B()];
	CPoint3d pC = m_vertices[tFace.C()];

	CVector3d vN;
	if (m_poly.normals().size() > j) vN = m_poly.normals()[j];


	return CMesh::rayTriangleIntersect3D(pA, pB, pC, vN, vRay, pP0, pIntersectionPoint, pDistance);
}



bool CMesh::getClosestFace( CPoint3f pkt0, CVector3f vRay, CPoint3f &IntersectionPoint, INDEX_TYPE &indx, bool onlyFront)
{
	INDEX_TYPE id = 0;
	double dist = DBL_MAX;
	CPoint3f tmpP;
	double r;
	bool foundAny = false;

	for ( INDEX_TYPE j=0; j<static_cast<INDEX_TYPE>(m_poly.size()); j++ )
	{
		int result = rayTriangleIntersect3D( j, vRay, pkt0, tmpP, r );
		
		// result == -2 ---> promień leży na trójkącie (ma wiecej punktów wspólnych niż 1)
		// result == -1 ---> trójkąt jest zdegenerowany (co najmniej jedna krawędź ma zerową długość)
		// result == 0 ---> promień nie trafił w trójkąt
		// result == 1 ---> trójkąt trafiony od przodu (punkt przecięcia zwracany w tmpP)
		// result == 2 ---> trójkąt trafiony od tyłu (punkt przecięcia zwracany w tmpP)

		if ( result > 0 )
		{
			double tmp = CVector3f( tmpP, pkt0 ).length();
		
			if ( tmp < dist )
			{
				dist = tmp;
				IntersectionPoint = tmpP;
				indx = j;

				if ( result == 1 )
				{
					foundAny = true;
				}
				else if ( result == 2 )
				{
					if (onlyFront)
						foundAny = false;				 // jeśli najbliżej jest ściana odwrócona tyłem, to nie powinno mi zwrócić punktu
					else
						foundAny = true;				// chyba że dokłądnie tego chcę
				}
			}
		}

		/* UWAGA
		W przypadku gdy result == -2 możemy mieć do czynienia z sytuacjami:
		a) ściana jest ustawiona bokiem, ale ma sąsiadów więc promień powinien w któregoś z nich trafić i temu sąsiadowi zostanie przypisane trafienie
		b) j.w., ale wszyscy trafieni sąsiedzi sa odwróceni "tyłem" to zn. że promień prześliznął sie po wewn. stronie siatki - można punkt pominąć.
		c) ściana nie ma żadnego sąsiada - nieprawidłowa siatka ??? - mozna punkt pominąć
		d) ściana jest na brzegu siatki - jedynie tu należałoby jakoś wyznaczyć punkt przecięcia, ale to jest chyba przypadek marginalny
		*/

	}
	
	return foundAny;
}

bool CMesh::getClosestFace(CPoint3f pkt0, CVector3f vRay, std::set<INDEX_TYPE> podzbior, CPoint3f & IntersectionPoint, INDEX_TYPE & indx, bool onlyFront)
{
	INDEX_TYPE id = 0;
	double dist = DBL_MAX;
	CPoint3f tmpP;
	double r;
	bool foundAny = false;

	for (std::set<INDEX_TYPE>::iterator si = podzbior.begin(); si != podzbior.end(); si++)
	{
		INDEX_TYPE j = *si;

		int result = rayTriangleIntersect3D(j, vRay, pkt0, tmpP, r);

		// result == -2 ---> promień leży na trójkącie (ma wiecej punktów wspólnych niż 1)
		// result == -1 ---> trójkąt jest zdegenerowany (co najmniej jedna krawędź ma zerową długość)
		// result == 0 ---> promień nie trafił w trójkąt
		// result == 1 ---> trójkąt trafiony od przodu (punkt przecięcia zwracany w tmpP)
		// result == 2 ---> trójkąt trafiony od tyłu (punkt przecięcia zwracany w tmpP)

		if (result > 0)
		{
			double tmp = CVector3f(tmpP, pkt0).length();

			if (tmp < dist)
			{
				dist = tmp;
				IntersectionPoint = tmpP;
				indx = j;

				if (result == 1)
				{
					foundAny = true;
				}
				else if (result == 2)
				{
					if (onlyFront)
						foundAny = false;				 // jeśli najbliżej jest ściana odwrócona tyłem, to nie powinno mi zwrócić punktu
					else
						foundAny = true;				// chyba że dokłądnie tego chcę
				}
			}
		}

		/* UWAGA
		W przypadku gdy result == -2 możemy mieć do czynienia z sytuacjami:
		a) ściana jest ustawiona bokiem, ale ma sąsiadów więc promień powinien w któregoś z nich trafić i temu sąsiadowi zostanie przypisane trafienie
		b) j.w., ale wszyscy trafieni sąsiedzi sa odwróceni "tyłem" to zn. że promień prześliznął sie po wewn. stronie siatki - można punkt pominąć.
		c) ściana nie ma żadnego sąsiada - nieprawidłowa siatka ??? - mozna punkt pominąć
		d) ściana jest na brzegu siatki - jedynie tu należałoby jakoś wyznaczyć punkt przecięcia, ale to jest chyba przypadek marginalny
		*/

	}

	return foundAny;
}


int CMesh::getListOfIntersections(CPoint3f pkt0, CVector3f vRay, std::vector<float> &in, std::vector<float> &out )
{
	int id = 0;
	CPoint3f tmpP;
	double tmpR;

	for (INDEX_TYPE j = 0; j < static_cast<INDEX_TYPE>(m_poly.size()); j++)
	{
		int result = rayTriangleIntersect3D(j, vRay, pkt0, tmpP, tmpR);

		// result == -2 ---> promień leży na trójkącie (ma wiecej punktów wspólnych niż 1)
		// result == -1 ---> trójkąt jest zdegenerowany (co najmniej jedna krawędź ma zerową długość)
		// result == 0 ---> promień nie trafił w trójkąt
		// result == 1 ---> trójkąt trafiony od przodu (punkt przecięcia zwracany w tmpP)
		// result == 2 ---> trójkąt trafiony od tyłu (punkt przecięcia zwracany w tmpP)

		if (result > 0)
		{
			//double tmp = CVector3f( tmpP, pkt0 ).length();

			if (result == 1)
			{
				in.push_back(tmpR);
				id++;
			}
			else if (result == 2)
			{
				out.push_back(tmpR);
				id++;
			}
		}
	}

	return id;
}


int CMesh::getListOfIntersections2( CPoint3f pkt0, CVector3f vRay, std::map<INDEX_TYPE, std::pair<float, int>> &in, int ignoreVertex )
{
	int id = 0;
	CPoint3f tmpP;
	double tmpR;

	for ( INDEX_TYPE j=0; j<static_cast<INDEX_TYPE>(m_poly.size()); j++ )
	{
		if ( (ignoreVertex!=-1) && (m_poly.faces()[j].hasVertex(ignoreVertex)>-1) )
		{
			continue;
		}

		int result = rayTriangleIntersect3D( j, vRay, pkt0, tmpP, tmpR );
		
		// result == -2 ---> promień leży na trójkącie (ma wiecej punktów wspólnych niż 1)
		// result == -1 ---> trójkąt jest zdegenerowany (co najmniej jedna krawędź ma zerową długość)
		// result == 0 ---> promień nie trafił w trójkąt
		// result == 1 ---> trójkąt trafiony od przodu (punkt przecięcia zwracany w tmpP)
		// result == 2 ---> trójkąt trafiony od tyłu (punkt przecięcia zwracany w tmpP)

		if ( result > 0 )
		{
			//double tmp = CVector3f( tmpP, pkt0 ).length();

			if ( result > 0 )
			{
				in[j] = std::pair<float, int>(tmpR,result);
				id++;
			}
		}
	}
	
	return id;
}

#include <algorithm>

float CMesh::getInteriorLength( CPoint3f pkt0, CVector3f vRay, float &maxLen )
{
	std::vector<float> in;
	std::vector<float> out;

	int lb = getListOfIntersections( pkt0, vRay, in, out );

	float result = 0.0f;

	sort( in.begin(), in.end() );
	sort( out.begin(), out.end() );

	maxLen = (*out.rbegin()) - (*in.begin());

	std::vector<float>::iterator in_ptr = in.begin();
	std::vector<float>::iterator out_ptr = out.begin();

	while ( ( in_ptr != in.end() ) && ( out_ptr != out.end() ) ) {
		while ( out_ptr!=out.end() && ( (*in_ptr) > (*out_ptr) ) ) {
			out_ptr++;
		}

		if ( out_ptr!=out.end() ) {

			//---------------------------------------
			result += ( (*out_ptr) - (*in_ptr) );
			//---------------------------------------

			while ( in_ptr!=in.end() && ( (*in_ptr) <= (*out_ptr) ) ) {
				in_ptr++;
			}

			out_ptr++;
		}
	}

	return result;
}

std::vector<CVector3f> &CMesh::getVectors( CPoint3f pkt0 ) 
{
	static std::vector<CVector3f> ret;

	ret.clear();
	for ( INDEX_TYPE j=0; j<static_cast<INDEX_TYPE>(m_vertices.size()); j++ )
	{
		ret.push_back( CVector3f( pkt0, m_vertices[j] ) );
		UI::STATUSBAR::printf( "Generating wektors [%d]", j );
	}

	return ret;
}

void CMesh::correctNormals()
{
	if (m_poly.normals().empty())
	{
		m_poly.normals().resize(m_poly.size());
	}

	//if (!m_poly.normals().empty())
	{
		for (int i = m_poly.size() - 1; i >= 0; i--)
		{
			UI::STATUSBAR::printfTimed(1000, L"(CMesh) Recomputing normal vectors of faces. %d more to be done", i);
			m_poly.normals()[i] = m_poly[i].getNormal(m_vertices);
		}
	}
}


bool CMesh::calcVN(bool weighted)
{
	CPointCloud::Normals tmpN(m_vertices.size(), CVector3d(0, 0, 0));

	for (CFace& f : m_poly.faces())
	{
		CVertex vA = m_vertices[f.A()];
		CVertex vB = m_vertices[f.B()];
		CVertex vC = m_vertices[f.C()];

		CVector3d vn = CFace::triangleNormal(vA, vB, vC);

		if (weighted)
			vn *= CFace::triangleArea(vA, vB, vC);

		tmpN[f.A()] += vn;
		tmpN[f.B()] += vn;
		tmpN[f.C()] += vn;
	}

	for (auto& vec : tmpN)
	{
		vec.normalize();
	}

	m_vnormals = tmpN;

	return (m_vertices.size() == m_vnormals.size());
}


bool CMesh::calcFN()
{
	try { m_poly.normals().resize(m_poly.size()); }
	catch (std::bad_alloc) { return false; }

	for (INDEX_TYPE j = 0; j < static_cast<INDEX_TYPE>(m_poly.size()); j++)
	{
		m_poly.normals()[j] = m_poly[j].getNormal(m_vertices);
	}

	return (0 < m_poly.normals().size());
}



void CMesh::invertNormals()
{
	if (!m_vnormals.empty())
	{
		for (int i = m_vnormals.size() - 1; i >= 0; i--)
		{
			UI::STATUSBAR::printfTimed(1000, L"(CMesh) Reversing normal vectors of vertices. %d more to be done", i);
			m_vnormals[i].invert();
		}
	}

	if (!m_poly.normals().empty())
	{
		for (int i = m_poly.normals().size() - 1; i >= 0; i--)
		{
			UI::STATUSBAR::printfTimed(1000, L"(CMesh) Reversing normal vectors of faces. %d more to be done", i);
			m_poly.normals()[i].invert();
		}
	}

	for (int i = m_poly.faces().size() - 1; i >= 0; i--)
	{
		UI::STATUSBAR::printfTimed(1000, L"(CMesh) Reversing normal vectors of faces. %d more to be done", i);
		m_poly.faces()[i].invert();
	}
}


void CMesh::removeUnusedVertices()
{
	std::map<INDEX_TYPE, INDEX_TYPE> vIndexMap;
	std::map<INDEX_TYPE, INDEX_TYPE> tcIndexMap;

	CPointCloud::Vertices newVertices;
	CPointCloud::Colors newColors;
	CPointCloud::Normals newNormals;

	CMaterial::TextureCoordinates newTC;

	bool modColors = ( m_vcolors.size() == m_vertices.size() );
	bool modNormals = (m_vnormals.size() == m_vertices.size());

	bool modTC = (m_materials[0]->texcoord.size() == m_vertices.size());

	this->resetBoundingBox();

	INDEX_TYPE i = static_cast<INDEX_TYPE>(m_poly.faces().size());

	for (INDEX_TYPE j = 0; j < i; j++)
	{
		UI::STATUSBAR::printfTimed(1000, L"(CMesh) Removing unused vertices. %d more to be done", i-j);

		CFace *f = &this->m_poly.faces()[j];

		INDEX_TYPE a = f->A();
		if (vIndexMap.end() == vIndexMap.find(a))
		{
			vIndexMap[a] = newVertices.size();
			newVertices.push_back(m_vertices[a] );
			if (modColors) newColors.push_back(m_vcolors[a]);
			if (modNormals) newNormals.push_back(m_vnormals[a]);

			expand(m_vertices[a]);
		}
		f->A( vIndexMap.at(a) );

		INDEX_TYPE b = f->B();
		if (vIndexMap.end() == vIndexMap.find(b))
		{
			vIndexMap[b] = newVertices.size();
			newVertices.push_back(m_vertices[b] );
			if (modColors) newColors.push_back(m_vcolors[b]);
			if (modNormals) newNormals.push_back(m_vnormals[b]);

			expand(m_vertices[b]);
		}
		f->B( vIndexMap.at(b) );

		INDEX_TYPE c = f->C();
		if (vIndexMap.end() == vIndexMap.find(c))
		{
			vIndexMap[c] = newVertices.size();
			newVertices.push_back(m_vertices[c] );
			if (modColors) newColors.push_back(m_vcolors[c]);
			if (modNormals) newNormals.push_back(m_vnormals[c]);

			expand(m_vertices[c]);
		}
		f->C( vIndexMap.at(c) );

		//====================================================

		if (modTC && (this->getMaterial(0).texindex.size()>j))
		{
			CTIndex* ti = &this->getMaterial(0).texindex[j];

			INDEX_TYPE tA = ti->a;
			if (tcIndexMap.end() == tcIndexMap.find(tA))
			{
				tcIndexMap[tA] = newTC.size();
				newTC.push_back(this->getMaterial(0).texcoord[tA]);
			}
			ti->a = tcIndexMap.at(tA);

			INDEX_TYPE tB = ti->b;
			if (tcIndexMap.end() == tcIndexMap.find(tB))
			{
				tcIndexMap[tB] = newTC.size();
				newTC.push_back(this->getMaterial(0).texcoord[tB]);
			}
			ti->b = tcIndexMap.at(tB);

			INDEX_TYPE tC = ti->c;
			if (tcIndexMap.end() == tcIndexMap.find(tC))
			{
				tcIndexMap[tC] = newTC.size();
				newTC.push_back(this->getMaterial(0).texcoord[tC]);
			}
			ti->c = tcIndexMap.at(tC);
		}
	}
	vIndexMap.clear();
	tcIndexMap.clear();

	m_vertices = newVertices;
	if (modColors) m_vcolors = newColors;
	if (modNormals) m_vnormals = newNormals;

	if (modTC) m_materials[0]->texcoord = newTC;

	newVertices.clear();
	newColors.clear();
	newNormals.clear();
	newTC.clear();
}

void CMesh::applyTransformation( CTransform &from,  CTransform &to )
{
	CPointCloud::applyTransformation(from, to);

	//correctNormals();
	calcVN();
	setModified(true);
}

void CMesh::removeFace(INDEX_TYPE idx)
{
	m_poly.removeFace(idx);
	setModified(true);
}


void CMesh::removeFace(CMesh::Faces::iterator itf)
{
	m_poly.removeFace(itf);
	setModified(true);
}


void CMesh::removeFace(CMesh::Faces::reverse_iterator itfr)
{
	m_poly.removeFace(itfr);
	setModified(true);
}

INDEX_TYPE CMesh::addFace(CFace f)
{
	setModified(true);
	return m_poly.addFace(f);
}

INDEX_TYPE CMesh::addFace(CFace f, CRGBA fColor)
{
	setModified(true);
	return m_poly.addFace(f,fColor);
}

INDEX_TYPE CMesh::addFaceX(std::vector<INDEX_TYPE> dane, CRGBA col)
{
	INDEX_TYPE a = dane[0], b = dane[1], c = dane[2];

	INDEX_TYPE lastIDX = addFace(CFace(a, b, c), col);
	
	for (int i = 3; i < dane.size(); ++i) {
		b = c;
		c = dane[i];

		lastIDX = addFace(CFace(a, b, c), col);
	}

	return lastIDX;
}


void CMesh::afterLoad(CModel3D *p)
{
	renderSmoothVertex( hasVertexNormals() );

	bool t = false;// attachTexture(p->path());

	UI::STATUSBAR::printf(t?L"jest tekstura": L"nie ma tekstury");

	renderWithTexture( hasTexture() && getMaterial().hasTextureCoords() && getMaterial().hasTextureIndexes() );
}


bool CMesh::attachTexture(CFileInfo plikSiatki)
{
	if (getMaterial().attachTexture( plikSiatki.absoluteFilePath() ))
	{
		prepare();
		return true;
	}
	return false;
}



CMesh* CMesh::createPrimitivePlane(double sizeX, double sizeY, int divX, int divY)
{
	CMesh* plane = new CMesh();

	double X2 = (sizeX + 1) / 2;
	double Y2 = (sizeY + 1) / 2;

	double minX = -X2;
	double maxX = minX+sizeX;

	double minY = -Y2;
	double maxY = minY + sizeY;

	plane->addVertex(CVertex(minX, minY, 0.0));
	plane->addVertex(CVertex(minX, maxY, 0.0));
	plane->addVertex(CVertex(maxX, maxY, 0.0));
	plane->addVertex(CVertex(maxX, minY, 0.0));

	plane->faces().push_back(CFace(0, 1, 2));
	plane->faces().push_back(CFace(0, 2, 3));

	plane->fnormals().push_back(CFace(0, 1, 2).getNormal(plane->vertices()));
	plane->fnormals().push_back(CFace(2, 3, 0).getNormal(plane->vertices()));

	plane->getMaterial().FrontColor.ambient.SetFloat(0.0f, 1.0f, 1.0f, 0.8f);
	plane->getMaterial().FrontColor.diffuse.SetFloat(0.0f, 1.0f, 1.0f, 0.8f);

	plane->getMaterial().BackColor.ambient.SetFloat(1.0f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.diffuse.SetFloat(1.0f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.specular.SetFloat(0.5f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.emission.SetFloat(0.5f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.shininess = 1.0f;


	return plane;
}


std::wstring CMesh::infoRow()
{
	std::wstring napis;
	
	napis  = CPointCloud::infoRow();
	napis += L"\nf:" + std::to_wstring((long long)m_poly.size());
	napis += L"\nfn:" + std::to_wstring((long long)m_poly.normals().size());
	napis += L"\nfc:" + std::to_wstring((long long)m_poly.colors().size());
	//napis += L", vs:" + std::to_wstring((long long)vsplits.size());
	//napis += L", tc:" + std::to_wstring((long long)texcoord.size());
	//napis += L", ti:" + std::to_wstring((long long)texindex.size());
	return napis;
}

void CMesh::info(std::wstring i[4])
{
	i[0] = L"data child";
	i[1] = L"???";
	i[2] = L"mesh";
	i[3] = infoRow();
}

void CMesh::createV2Fmap( CMesh::V2Fmap &map )
{
	map.clear();

	for (int i = 0; i<m_poly.size(); i++)
	{
		CFace *f = &m_poly[i];
		map[f->A()].insert(i);
		map[f->B()].insert(i);
		map[f->C()].insert(i);
	}
}

void CMesh::createV2Emap(CMesh::V2Emap& v2e)
{
	v2e.clear();

	for (int i = 0; i < m_poly.size(); i++)
	{
		CFace* f = &m_poly[i];

		v2e[f->A()].first.insert(f->B());
		v2e[f->A()].second.insert(f->C());

		v2e[f->B()].first.insert(f->C());
		v2e[f->B()].second.insert(f->A());

		v2e[f->C()].first.insert(f->A());
		v2e[f->C()].second.insert(f->B());
	}
}


void CMesh::createMaps(CMesh::V2Fmap& v2f, CMesh::V2Emap& v2e)
{
	v2f.clear();
	v2e.clear();

	for (int i = 0; i < m_poly.size(); i++)
	{
		CFace* f = &m_poly[i];
		v2f[f->A()].insert(i);
		v2f[f->B()].insert(i);
		v2f[f->C()].insert(i);

		v2e[f->A()].first.insert(f->B());
		v2e[f->A()].second.insert(f->C());

		v2e[f->B()].first.insert(f->C());
		v2e[f->B()].second.insert(f->A());

		v2e[f->C()].first.insert(f->A());
		v2e[f->C()].second.insert(f->B());
	}
}


void CMesh::findNeighborsOfFace( INDEX_TYPE idxF, CMesh::V2Fmap &map, CMesh::Neighbors *sasiadki, int limit)
{
	CFace *f = &m_poly[idxF];

	sasiadki->clear();

	std::map<INDEX_TYPE, INDEX_TYPE> scianki;

	CMesh::V2Fmap::iterator f_rng;

	f_rng = map.find(f->A());
	if (f_rng != map.end())
	{
		std::set<INDEX_TYPE>::iterator fi = f_rng->second.find(idxF);

		for (std::set<INDEX_TYPE>::iterator fi = f_rng->second.begin(); fi != f_rng->second.end(); fi++)
		{
			if (idxF != *fi)
			{
				if (scianki.find(*fi) == scianki.end())
				{
					scianki[*fi] = 1;
				}
				else
				{
					scianki[*fi]++;
				}
			}
		}
	}

	f_rng = map.find(f->B());
	if (f_rng != map.end())
	{
		std::set<INDEX_TYPE>::iterator fi = f_rng->second.find(idxF);

		for (std::set<INDEX_TYPE>::iterator fi = f_rng->second.begin(); fi != f_rng->second.end(); fi++)
		{
			if (idxF != *fi)
			{
				if (scianki.find(*fi) == scianki.end())
				{
					scianki[*fi] = 1;
				}
				else
				{
					scianki[*fi]++;
				}
			}
		}
	}

	f_rng = map.find(f->C());
	if (f_rng != map.end())
	{
		std::set<INDEX_TYPE>::iterator fi = f_rng->second.find(idxF);

		for (std::set<INDEX_TYPE>::iterator fi = f_rng->second.begin(); fi != f_rng->second.end(); fi++)
		{
			if (idxF != *fi)
			{
				if (scianki.find(*fi) == scianki.end())
				{
					scianki[*fi] = 1;
				}
				else
				{
					scianki[*fi]++;
				}
			}
		}
	}


	for (std::map<INDEX_TYPE, INDEX_TYPE>::iterator im = scianki.begin(); im != scianki.end(); im++)
	{
		if (im->second > limit)	   //scianki, które mają co najmniej (czytaj: dokładnie) dwa wierzchołki wspólne z bieżącą
		{
			sasiadki->insert(im->first);
		}
	}
}

void CMesh::findBoundaryFaces(std::set<INDEX_TYPE>& brzegowe, CMesh::V2Fmap & map)
{
	for (int faceIndex = 0; faceIndex < m_poly.size(); faceIndex++)
	{
		UI::STATUSBAR::printfTimed(500, L"szukam ścianek brzegowych, zostało:%d", m_poly.size() - faceIndex);

		CMesh::Neighbors neighbors;
		findNeighborsOfFace(faceIndex, map, &neighbors, 1); // sasiadki "krawedziowe"	- może ich być max. 3

		if (neighbors.size() < 3) //gdy mam mniej niż 3 sąsiadki do plotek, to jestem na krawędzi!!!
		{
			brzegowe.insert( faceIndex );
		}
	}

	UI::STATUSBAR::printf(L"gotowe, znalazłem:%d ścianek brzegowych", brzegowe.size());
}


void CMesh::findBoundaryFaces( CMesh::Neighborhood &brzegowe, CMesh::V2Fmap &map, std::set< INDEX_TYPE > &scianki )
{
	for (std::set< INDEX_TYPE >::iterator i = scianki.begin(); i != scianki.end(); i++)
	{
		UI::STATUSBAR::printfTimed(500, L"szukam ścianek brzegowych, znaleziono:%d", brzegowe.size() );

		CMesh::Neighbors sasiadki;
		findNeighborsOfFace( (*i), map, &sasiadki, 1 ); // sasiadki "krawedziowe"	- może ich być max. 3

		if (sasiadki.size() < 3) //gdy mam mniej niż 3 sąsiadki do plotek, to jestem na krawędzi!!!
		{
			brzegowe.insert( CMesh::Neighborhood::value_type((*i), sasiadki) );
		}
	}

	UI::STATUSBAR::printf(L"gotowe, znalazłem:%d ścianek brzegowych", brzegowe.size());
}

void CMesh::findBoundaryFaces( CMesh::Neighborhood &brzegowe, CMesh::V2Fmap &map )
{
	for ( int faceIndex = 0; faceIndex < m_poly.size(); faceIndex++ )
	{
		UI::STATUSBAR::printfTimed(500, L"szukam ścianek brzegowych, zostało:%d", m_poly.size() - faceIndex );

		CMesh::Neighbors neighbors;
		findNeighborsOfFace( faceIndex, map, &neighbors, 1 ); // sasiadki "krawedziowe"	- może ich być max. 3

		if (neighbors.size() < 3) //gdy mam mniej niż 3 sąsiadki do plotek, to jestem na krawędzi!!!
		{
			brzegowe.insert(CMesh::Neighborhood::value_type( faceIndex, neighbors ));
		}
	}

	UI::STATUSBAR::printf(L"gotowe, znalazłem:%d ścianek brzegowych", brzegowe.size());
}

void CMesh::findBoundaryFaces(std::set<INDEX_TYPE>& brzegowe, CMesh::V2Fmap & map, std::set<INDEX_TYPE>& scianki)
{
	for (std::set< INDEX_TYPE >::iterator i = scianki.begin(); i != scianki.end(); i++)
	{
		UI::STATUSBAR::printfTimed(500, L"szukam ścianek brzegowych, znaleziono:%d", brzegowe.size());

		CMesh::Neighbors sasiadki;
		findNeighborsOfFace((*i), map, &sasiadki, 1); // sasiadki "krawedziowe"	- może ich być max. 3

		if (sasiadki.size() < 3) //gdy mam mniej niż 3 sąsiadki do plotek, to jestem na krawędzi!!!
		{
			brzegowe.insert( *i );
		}
	}

	UI::STATUSBAR::printf(L"gotowe, znalazłem:%d ścianek brzegowych", brzegowe.size());
}

void CMesh::findBoundaryEdges( CMesh::Edges &edges, CMesh::Neighborhood sciankiBrzegowe )
{
	for (CMesh::Neighborhood::iterator bi = sciankiBrzegowe.begin(); bi != sciankiBrzegowe.end(); bi++)
	{
		INDEX_TYPE myIndex = bi->first;
		std::set<std::pair<INDEX_TYPE, INDEX_TYPE>> myEdges;

		myEdges.insert(std::pair<INDEX_TYPE, INDEX_TYPE>(m_poly[myIndex].A(), m_poly[myIndex].B()));
		myEdges.insert(std::pair<INDEX_TYPE, INDEX_TYPE>(m_poly[myIndex].B(), m_poly[myIndex].C()));
		myEdges.insert(std::pair<INDEX_TYPE, INDEX_TYPE>(m_poly[myIndex].C(), m_poly[myIndex].A()));

		for (std::set<INDEX_TYPE>::iterator ni = bi->second.begin(); ni != bi->second.end(); ni++)
		{
			INDEX_TYPE nIndex = *ni;
			CFace f = m_poly[nIndex];

			if (myEdges.find(std::pair<INDEX_TYPE, INDEX_TYPE>( f.B(), f.A() )) != myEdges.end())
				myEdges.erase(std::pair<INDEX_TYPE, INDEX_TYPE>( f.B(), f.A()));

			if (myEdges.find(std::pair<INDEX_TYPE, INDEX_TYPE>( f.C(), f.B())) != myEdges.end())
				myEdges.erase(std::pair<INDEX_TYPE, INDEX_TYPE>( f.C(), f.B()));

			if (myEdges.find(std::pair<INDEX_TYPE, INDEX_TYPE>( f.A(), f.C())) != myEdges.end())
				myEdges.erase(std::pair<INDEX_TYPE, INDEX_TYPE>( f.A(), f.C()));
		}

		for (std::set<std::pair<INDEX_TYPE, INDEX_TYPE>>::iterator ei = myEdges.begin(); ei != myEdges.end(); ei++)
		{
			edges.insert(CEdge(ei->first, ei->second, myIndex));
		}
	}
}

void CMesh::getEdges(CMesh::Edges &edges)
{
	edges.clear();
	for (int myIndex = 0; myIndex < m_poly.size(); myIndex++)
	{
		CFace f = m_poly[myIndex];
		edges.insert(CEdge( f.A(), f.B(), myIndex));
		edges.insert(CEdge( f.B(), f.C(), myIndex));
		edges.insert(CEdge( f.C(), f.A(), myIndex));
	}
}

//void CMesh::findBoundaryEdges( CMesh::Edges &edges )
//{
//	getEdges(edges);
//
//	CMesh::Edges::iterator it = edges.begin();
//	while (it != edges.end())
//	{
//		CMesh::Edges::iterator ti = edges.find(CEdge(it->second, it->first, 0));
//		if (ti != edges.end())
//		{
//			edges.erase(ti);
//			it = edges.erase(it);
//		}
//		else
//		{
//			it++;
//		}
//	}
//}

void CMesh::findBoundaryEdges(CMesh::Edges& edges)
{
	CMesh::Edges tmp_edges;

	getEdges(tmp_edges);

	edges.clear();

	CMesh::Edges::iterator it = tmp_edges.begin();
	while (it != tmp_edges.end())
	{
		INDEX_TYPE f = (*it).first;
		INDEX_TYPE s = (*it).second;
		INDEX_TYPE t = (*it).face;

		int licz = 0;

		it = tmp_edges.erase(it); // kasuje i przesuwa wskaźnik na kolejny element

		CMesh::Edges::iterator ti3 = tmp_edges.find(CEdge( f, s, 0));

		while (ti3 != tmp_edges.end())
		{
			if (ti3 == it)
				it = tmp_edges.erase(ti3);
			else
				tmp_edges.erase(ti3);

			licz++;
			ti3 = tmp_edges.find(CEdge(f, s, 0));
		}

		CMesh::Edges::iterator ti4 = tmp_edges.find(CEdge( s, f, 0));

		while (ti4 != tmp_edges.end())
		{
			if (ti4 == it)
				it = tmp_edges.erase(ti4);
			else
				tmp_edges.erase(ti4);

			licz++;
			ti4 = tmp_edges.find(CEdge(s, f, 0));
		}

		if ( licz == 0 )
		{
			edges.insert(CEdge(f, s, t));
		}
	}
}


void CMesh::getEdges(CMesh::Edges &edges, std::set< INDEX_TYPE > &scianki)
{
	edges.clear();
	for (std::set< INDEX_TYPE >::iterator i = scianki.begin(); i != scianki.end(); i++)
	{
		INDEX_TYPE myIndex = (*i);

		CFace f = m_poly[myIndex];

		edges.insert(CEdge( f.A(), f.B(), myIndex));
		edges.insert(CEdge( f.B(), f.C(), myIndex));
		edges.insert(CEdge( f.C(), f.A(), myIndex));
	}
}

void CMesh::findBoundaryEdges(CMesh::Edges &edges, std::set< INDEX_TYPE > &scianki)
{
	getEdges(edges, scianki);

	CMesh::Edges::iterator it = edges.begin();
	while ( it != edges.end() )
	{
		CMesh::Edges::iterator ti = edges.find(CEdge(it->second, it->first, 0));
		if ( ti != edges.end())
		{
			edges.erase(ti);
			it = edges.erase(it);
		}
		else
		{
			it++;
		}
	}
}




//void CMesh::removeDuplicateVertices(double eps)
//{
//	CMesh::V2Fmap v2map;
//	this->createV2Fmap(v2map);
//
//	this->removeDuplicateVertices(v2map);
//
//	v2map.clear();
//}


void CMesh::removeDuplicateVertices(double eps)
{
	this->createKDtree();
	std::map<INDEX_TYPE, INDEX_TYPE> vertexMap;
	
	for (int i = 0; i < m_vertices.size(); i++)
	{
		if (vertexMap.find(i) == vertexMap.end())
		{
			vertexMap[i] = i;

			double eps2 = 0.0001;
			if (eps != 0) eps2 = eps + 0.0001 * eps;

			std::vector<INDEX_TYPE> v = m_kdtree->find_all_in_distance_to_pt(eps2, m_vertices[i]);

			for (INDEX_TYPE idx : v)
			{
				if ((idx != i) && (CVector3f(m_vertices[i], m_vertices[idx]).squaredLength() <= eps))
				{
					vertexMap[idx] = i;
				}
			}
		}
	}

	for (int i = m_poly.faces().size() - 1; i >= 0; i--) {
		auto& f = m_poly.faces()[i];

		f.Set(vertexMap[f.first], vertexMap[f.second], vertexMap[f.third]);
		if ((f.A() == f.B()) || (f.A() == f.C()) || (f.C() == f.B()))
		{
			m_poly.removeFace(i);
		}
	}



	this->removeUnusedVertices();
}


void CMesh::removeDuplicateVertices( CMesh::V2Fmap& map, double eps )
{
	this->createKDtree();
	std::set<INDEX_TYPE> used;
	for (int i = 0; i < m_vertices.size(); i++)
	{
		if (used.find(i) == used.end())
		{
			double eps2 = 0.001;
			if (eps != 0) eps2 = eps + 0.001 * eps;

			std::vector<INDEX_TYPE> v = m_kdtree->find_all_in_distance_to_pt(eps2, m_vertices[i]);

			for (INDEX_TYPE idx : v)
			{
				if ((idx != i) && (CVector3f(m_vertices[i], m_vertices[idx]).squaredLength() <= eps))
				{
					for (INDEX_TYPE n : map[idx])
					{
						CFace* f = &faces()[n];

						int k = f->hasVertex(idx);

						if (-1 != k) f->setAt(k, i);
					}
					map[idx].clear();
					used.insert(idx);
				}
			}
		}
	}

	this->removeUnusedVertices();
}


std::map<int, CMesh::Neighbors> CMesh::multiNeighbourhood(CMesh::Neighborhood& map, INDEX_TYPE idx, int dist = 1, double maxDist = 0.0)
{
	std::map<int, CMesh::Neighbors> result;

	result[0].insert(idx); // poziom "zero"

	if (dist < 1)
	{
		if (maxDist == 0.0)
		{
			return result;
		}

		dist = INT_MAX;
	}

	if (maxDist == 0.0)
	{
		result[1] = map[idx];
	}
	else
	{
		for (auto& i : map[idx]) {
			if (CVector3d(m_vertices[idx], m_vertices[i]).length() <= maxDist)
				result[1].insert(i);
		}
	}

	if (dist == 1) return result;

	for (int i = 2; i <= dist; i++)
	{
		for (const auto& i1 : result[i - 1])
		{
			if (maxDist == 0.0)
			{
				result[i].insert(map[i1].begin(), map[i1].end());
			}
			else
			{
				for (auto& i2 : map[i1]) {
					if (CVector3d(m_vertices[idx], m_vertices[i2]).length() <= maxDist)
						result[i].insert(i2);
				}
			}
		}

		if (result[i].empty())
		{
			result.erase(i);
			return result;
		}

		for (int j = 0; j < i; j++)
		{
			for (const auto& j1 : result[j])
			{
				if (result[i].find(j1) != result[i].end())
				{
					result[i].erase(j1);
				}
			}
		}
	}

	return result;
}


void CMesh::createNeighborhoodMapOfVertices(CMesh::Neighborhood &map, bool bidirectional )
{
	map.clear();
	for (int myIndex = 0; myIndex < m_poly.size(); myIndex++)
	{
		UI::STATUSBAR::printfTimed(500, L"generating neghborhood map for all vertices, %d remaining", m_poly.size() - myIndex);
		CFace f = m_poly[myIndex];

		map[f.A()].insert(f.B());
		map[f.B()].insert(f.C());
		map[f.C()].insert(f.A());

		if (bidirectional)
		{
			map[f.A()].insert(f.C());
			map[f.B()].insert(f.A());
			map[f.C()].insert(f.B());
		}
	}
}

void CMesh::createNeighborhoodMapOfVertices(CMesh::Neighborhood &map, std::set< INDEX_TYPE > &scianki, bool bidirectional )
{
	map.clear();
	int n = scianki.size();
	for (std::set< INDEX_TYPE >::iterator i = scianki.begin(); i != scianki.end(); i++)
	{
		INDEX_TYPE myIndex = (*i);

		UI::STATUSBAR::printfTimed(500, L"generating neghborhood map for all vertices, %d remaining", n--);
		CFace f = m_poly[myIndex];

		map[f.A()].insert(f.B());
		map[f.B()].insert(f.C());
		map[f.C()].insert(f.A());

		if (bidirectional)
		{
			map[f.A()].insert(f.C());
			map[f.B()].insert(f.A());
			map[f.C()].insert(f.B());
		}
	}
}

//
//CPoint3f CMesh::getCenterOfWeight()
//{
//	double x = 0.0;
//	double y = 0.0;
//	double z = 0.0;
//
//	for (CMesh::Vertices::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
//	{
//		x += it->X();
//		y += it->Y();
//		z += it->Z();
//	}
//	
//	x /= m_vertices.size();
//	y /= m_vertices.size();
//	z /= m_vertices.size();
//
//	return CPoint3f(x, y, z);
//}

void CMesh::transformByMatrixF(float matrix[16])
{
	//for (CMesh::Vertices::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
	//{
	//	*it = (*it).transformByMatrixF(matrix);
	//}
	CPointCloud::transformByMatrixF(matrix);
	correctNormals();
	if (hasVertexNormals()) calcVN();
}

void CMesh::transformByMatrixD(double matrix[16])
{
//	for (CMesh::Vertices::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
//	{
//		CPoint3d v(*it);
//		*it = v.transformByMatrixD(matrix);
//	}
	CPointCloud::transformByMatrixD(matrix);
	correctNormals();
	if (hasVertexNormals()) calcVN();
}




void CMesh::pushThreeNewFaces(INDEX_TYPE idxTop/*A*/, INDEX_TYPE idxLeft/*B*/, INDEX_TYPE idxRight/*C*/, INDEX_TYPE* idxF/*F*/, INDEX_TYPE* idxE/*E*/, std::map<INDEX_TYPE, INDEX_TYPE>* idxMap, CMesh::Faces* tmpf)
{
	if (idxMap[0].find(idxTop) != idxMap[0].end())
	{
		tmpf[0].push_back(CFace(idxMap[0][idxTop], idxF[0], idxE[0]));

		tmpf[1].push_back(CFace(idxF[1], idxMap[1][idxLeft], idxMap[1][idxRight]));
		tmpf[1].push_back(CFace(idxF[1], idxMap[1][idxRight], idxE[1]));
	}
	else
	{
		tmpf[1].push_back(CFace(idxMap[1][idxTop], idxF[1], idxE[1]));

		tmpf[0].push_back(CFace(idxF[0], idxMap[0][idxLeft], idxMap[0][idxRight]));
		tmpf[0].push_back(CFace(idxF[0], idxMap[0][idxRight], idxE[0]));
	}

	tmpf[2].push_back(CFace(idxMap[2][idxTop], idxF[2], idxE[2]));
	tmpf[2].push_back(CFace(idxF[2], idxMap[2][idxLeft], idxMap[2][idxRight]));
	tmpf[2].push_back(CFace(idxF[2], idxMap[2][idxRight], idxE[2]));
}

void CMesh::pushTwoNewFaces(INDEX_TYPE idxTop, INDEX_TYPE idxLeft, INDEX_TYPE idxRight, INDEX_TYPE* idxNewArr, std::map<INDEX_TYPE, INDEX_TYPE>* idxMap, CMesh::Faces* tmpf)
{
	if (idxMap[0].find(idxRight) != idxMap[0].end())
	{
		tmpf[0].push_back(CFace(idxMap[0][idxTop], idxNewArr[0], idxMap[0][idxRight]));
		tmpf[1].push_back(CFace(idxMap[1][idxTop], idxMap[1][idxLeft], idxNewArr[1]));
	}
	else
	{
		tmpf[1].push_back(CFace(idxMap[1][idxTop], idxNewArr[1], idxMap[1][idxRight]));
		tmpf[0].push_back(CFace(idxMap[0][idxTop], idxMap[0][idxLeft], idxNewArr[0]));
	}

	tmpf[2].push_back(CFace(idxMap[2][idxTop], idxNewArr[2], idxMap[2][idxRight]));
	tmpf[2].push_back(CFace(idxMap[2][idxTop], idxMap[2][idxLeft], idxNewArr[2]));
}


void CMesh::pushNewVertex(INDEX_TYPE idx1, INDEX_TYPE idx2, CVertex v, CPointCloud::Vertices* tmpv, CPointCloud::Colors* tmpc, INDEX_TYPE* ret)
{
	if (hasVertexColors())
	{
		CRGBA k1 = m_vcolors[idx1];
		CRGBA k2 = m_vcolors[idx2];
		CRGBA nowyKolor((k1.fR() + k2.fR()) / 2, (k1.fG() + k2.fG()) / 2, (k1.fB() + k2.fB()) / 2, (k1.fA() + k2.fA()) / 2);
		tmpc[0].push_back(nowyKolor);
		tmpc[1].push_back(nowyKolor);
		tmpc[2].push_back(nowyKolor);
	}

	tmpv[0].push_back(v);
	ret[0] = tmpv[0].size() - 1;

	tmpv[1].push_back(v);
	ret[1] = tmpv[1].size() - 1;

	tmpv[2].push_back(v);
	ret[2] = tmpv[2].size() - 1;
}

#include "Plane.h"
#include "AnnotationPoints.h"

void CMesh::cutPlane(CPlane m_cutPlane, CMesh* reszta, CMesh* krawedz, CAnnotationPoints* pts)
{
	//CMesh* mesh = (CMesh*)obj->getChild();
	if ( hasFaces() )
	{
		CPointCloud::Vertices tmpv[3];
		CPointCloud::Colors tmpc[3];

		std::map<INDEX_TYPE, INDEX_TYPE> idxMap[3];

		for (int iV = 0; iV < m_vertices.size(); iV++)
		{
			CVertex& v = m_vertices[iV];

			int test = m_cutPlane.testPointPosition(v);

			int idx = (test > 0) ? 1 : (test < 0) ? 0 : 2;

			tmpv[idx].push_back(v);
			if ( hasVertexColors() ) tmpc[idx].push_back(m_vcolors[iV]);
			idxMap[idx][iV] = tmpv[idx].size() - 1;

			if (idx == 2) // punkt leży na płaszczyźnie - wspólny dla obu części
			{
				tmpv[0].push_back(v);
				if ( hasVertexColors() ) tmpc[0].push_back(m_vcolors[iV]);
				idxMap[0][iV] = tmpv[0].size() - 1;

				tmpv[1].push_back(v);
				if (hasVertexColors()) tmpc[1].push_back(m_vcolors[iV]);
				idxMap[1][iV] = tmpv[1].size() - 1;
			}
		}

		CMesh::Faces tmpf[3];

		CModel3D* obj3 = new CModel3D();

		for (int iF = 0; iF < faces().size(); iF++)
		{
			CFace& f = faces()[iF];

			if ((idxMap[0].find(f.A()) != idxMap[0].end()) && (idxMap[0].find(f.B()) != idxMap[0].end()) && (idxMap[0].find(f.C()) != idxMap[0].end()))
			{
				tmpf[0].push_back(CFace(idxMap[0][f.A()], idxMap[0][f.B()], idxMap[0][f.C()]));
			}
			else if ((idxMap[1].find(f.A()) != idxMap[1].end()) && (idxMap[1].find(f.B()) != idxMap[1].end()) && (idxMap[1].find(f.C()) != idxMap[1].end()))
			{
				tmpf[1].push_back(CFace(idxMap[1][f.A()], idxMap[1][f.B()], idxMap[1][f.C()]));
			}
			else
			{
				CVertex& vA = m_vertices[f.A()];
				CVertex& vB = m_vertices[f.B()];
				CVertex& vC = m_vertices[f.C()];

				if (idxMap[2].find(f.A()) == idxMap[2].end())
				{
					tmpv[2].push_back(vA);
					if (hasVertexColors()) tmpc[2].push_back(m_vcolors[f.A()]);
					idxMap[2][f.A()] = tmpv[2].size() - 1;
				}

				if (idxMap[2].find(f.B()) == idxMap[2].end())
				{
					tmpv[2].push_back(vB);
					if (hasVertexColors()) tmpc[2].push_back(m_vcolors[f.B()]);
					idxMap[2][f.B()] = tmpv[2].size() - 1;
				}

				if (idxMap[2].find(f.C()) == idxMap[2].end())
				{
					tmpv[2].push_back(vC);
					if (hasVertexColors()) tmpc[2].push_back(m_vcolors[f.C()]);
					idxMap[2][f.C()] = tmpv[2].size() - 1;
				}

				//tmpf[2].push_back(CFace(idxMap[2][f.A()], idxMap[2][f.B()], idxMap[2][f.C()]));

				CPoint3d* vD = NULL;
				CPoint3d* vE = NULL;
				CPoint3d* vF = NULL;

				CPoint3d result;
				if (m_cutPlane.lineSegmentIntersection(vA, vB, result))
				{
					if (pts != nullptr) pts->addPoint(result);
					vF = new CPoint3d(result); // F=C'
				}

				if (m_cutPlane.lineSegmentIntersection(vB, vC, result))
				{
					if (pts != nullptr) pts->addPoint(result);
					vD = new CPoint3d(result); // D=A'
				}

				if (m_cutPlane.lineSegmentIntersection(vC, vA, result))
				{
					if (pts != nullptr) pts->addPoint(result);
					vE = new CPoint3d(result); // E=B'
				}

				// kolejność: A F B D C E

				int scenariusz = 0;
				if (vD != NULL) scenariusz++;
				if (vE != NULL) scenariusz++;
				if (vF != NULL) scenariusz++;

				INDEX_TYPE idxD[3], idxE[3], idxF[3];

				switch (scenariusz)
				{
				case 3:
					// trzy trafienia
					// - przecina bok i wierzchołek - podzielić ściankę na 2 częsci,
					// - ścianka leży na płaszczyżnie (???) - została już wcześniej dodana do jednej z części

					if (vD == vE) // == C
					{
						// trójkąty: CFB + CAF

						pushNewVertex( f.A(), f.B(), *vF, tmpv, tmpc, idxF);

						pushTwoNewFaces(f.C(), f.A(), f.B(), idxF, idxMap, tmpf);
					}
					else if (vE == vF) // ==A
					{
						// trójkąty: ADC + ABD
						pushNewVertex( f.B(), f.C(), *vD, tmpv, tmpc, idxD);

						pushTwoNewFaces(f.A(), f.B(), f.C(), idxD, idxMap, tmpf);
					}
					if (vF == vD) // ==B
					{
						// trójkąty: BEC + BAE

						pushNewVertex( f.A(), f.C(), *vE, tmpv, tmpc, idxE);

						pushTwoNewFaces(f.B(), f.C(), f.A(), idxE, idxMap, tmpf);
					}

					break;
				case 2:
					// dwa trafienia
					// - przecina dwa boki (najczęściej tak będzie) - podzielić ściankę na 3 częsci,
					// - bok leży na płaszczyżnie (???)
					if (NULL == vD) // Brak A'
					{
						// trójkąty: AFE + FBC + FCE

						pushNewVertex( f.A(), f.B(), *vF, tmpv, tmpc, idxF);
						pushNewVertex( f.A(), f.C(), *vE, tmpv, tmpc, idxE);

						pushThreeNewFaces(f.A(), f.B(), f.C(), idxF, idxE, idxMap, tmpf);
					}
					else if (NULL == vE)
					{
						// trójkąty: BDF + DCA + DAF

						pushNewVertex( f.B(), f.C(), *vD, tmpv, tmpc, idxD);
						pushNewVertex( f.B(), f.A(), *vF, tmpv, tmpc, idxF);

						pushThreeNewFaces(f.B(), f.C(), f.A(), idxD, idxF, idxMap, tmpf);
					}
					else if (NULL == vF)
					{
						// trójkąty: CED + EAB + EBD

						pushNewVertex( f.C(), f.A(), *vE, tmpv, tmpc, idxE);
						pushNewVertex( f.C(), f.B(), *vD, tmpv, tmpc, idxD);

						pushThreeNewFaces(f.C(), f.A(), f.B(), idxE, idxD, idxMap, tmpf);
					}

					break;
				case 1:
					// jedno trafienie - przecina tylko jeden wierzchołek
					// - ścianka w całości nalezy do jednej z brył
					// to zostalo już obsłużone automatycznie wcześniej
					break;
				default:
					//zero trafień, to nie powinno wystąpić
					break;
				}

				if (vD != NULL) { delete vD; vD = NULL; }
				if (vE != NULL) { delete vE; vE = NULL; }
				if (vF != NULL) { delete vF; vF = NULL; }
			}
		}

		m_vnormals.clear();
		fnormals().clear();
		fcolors().clear();

		m_vertices = tmpv[1];
		m_vcolors = tmpc[1];
		faces() = tmpf[1];

		resetBoundingBox();
		for (auto& v : m_vertices)
		{
			expandBoundingBox(v);
		}

		if (krawedz != nullptr)
		{
			krawedz->vertices() = tmpv[2];
			krawedz->vcolors() = tmpc[2];
			krawedz->faces() = tmpf[2];

			krawedz->resetBoundingBox();
			for (auto& v : krawedz->m_vertices)
			{
				krawedz->expandBoundingBox(v);
			}
		}

		if (reszta != nullptr)
		{
			reszta->vertices() = tmpv[0];
			reszta->vcolors() = tmpc[0];
			reszta->faces() = tmpf[0];

			reszta->resetBoundingBox();
			for (auto& v : reszta->m_vertices)
			{
				reszta->expandBoundingBox(v);
			}
		}

	}
	//else
	//{
	//	CPointCloud* r = nullptr;
	//	CPointCloud::cutPlane(&m_cutPlane, r);

	//	if ( r != nullptr )
	//		reszta = new CMesh( *r );
	//}
}


void CMesh::merge(CMesh* mesh)
{
	INDEX_TYPE vOffset = this->m_vertices.size();
	INDEX_TYPE fOffset = this->m_poly.faces().size();

	this->setSelfVisibility(false);

	for (int i = 0; i < mesh->vertices().size(); i++)
	{
		if (mesh->hasVertexColors())
		{
			if (mesh->hasVertexNormals())
			{
				this->addVertex(mesh->vertices()[i], mesh->vcolors()[i], mesh->vnormals()[i]);
			}
			else
			{
				this->addVertex(mesh->vertices()[i], mesh->vcolors()[i]);
			}
		}
		else
		{
			if (mesh->hasVertexNormals())
			{
				this->addVertex(mesh->vertices()[i], mesh->vnormals()[i]);
			}
			else
			{
				this->addVertex(mesh->vertices()[i]);
			}
		}
	}

	for (int i = 0; i < mesh->faces().size(); i++)
	{
		INDEX_TYPE a = mesh->faces()[i].A() + vOffset;
		INDEX_TYPE b = mesh->faces()[i].B() + vOffset;
		INDEX_TYPE c = mesh->faces()[i].C() + vOffset;

		CFace f(a, b, c);

		if (mesh->hasFaceColors())
		{
			CRGBA col = mesh->fcolors()[i];

			this->addFace(f, col);
		}
		else
		{
			this->addFace(f);
		}
	}

	this->setSelfVisibility(true);
}
