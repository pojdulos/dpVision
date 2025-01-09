#include "Global.h"
#include "PointCloud.h"
#include "GLViewer.h"

int CPointCloud::m_pointSize = 1;

CPointCloud::CPointCloud(CBaseObject *p) : CObject(p)
{
	setLabel("cloud");
	m_kdtree = nullptr;

	m_materials.push_back(new CMaterial);
}

CPointCloud::CPointCloud(CPointCloud& m) : CObject(m)
{
	m_vertices = m.m_vertices;
	m_vcolors = m.m_vcolors;
	m_vnormals = m.m_vnormals;

	for (CPointCloud::Materials::iterator it = m.m_materials.begin(); it != m.m_materials.end(); ++it)
	{
		m_materials.push_back(new CMaterial(**it));
	}

	m_kdtree = nullptr;
}

CPointCloud::~CPointCloud()
{
	m_vertices.clear();
	m_vcolors.clear();
	m_vnormals.clear();
	
	for (CPointCloud::Materials::iterator it = m_materials.begin(); it != m_materials.end(); ++it)
	{
		delete* it;
	}
	m_materials.clear();

	removeKDtree();
}

std::wstring CPointCloud::infoRow()
{
	std::wstring napis;

	napis = L"[id:" + std::to_wstring((long long)m_Id) + L"]";
	napis += L"\nv:" + std::to_wstring((long long)m_vertices.size());
	napis += L"\nvc:" + std::to_wstring((long long)m_vcolors.size());
	napis += L"\nvn:" + std::to_wstring((long long)m_vnormals.size());

	return napis;
}

#define PACKET_SIZE (size_t)1000000

void CPointCloud::renderSelf()
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if ( hasVertexColors() && ! m_materials[0]->m_force )
	{
		//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mesh.getMaterial().FrontColor.emission.fV(buf));

		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	}
	else
	{
		GLfloat kol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_materials[0]->FrontColor.ambient.fV(kol) );
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_materials[0]->FrontColor.diffuse.fV(kol) );
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_materials[0]->FrontColor.specular.fV(kol) );
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_materials[0]->FrontColor.emission.fV(kol) );
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_materials[0]->FrontColor.shininess);
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_POINT_SMOOTH);
	glPointSize( m_pointSize );

	size_t idx = 0;

	while (idx < m_vertices.size()) {
		glVertexPointer(3, GL_FLOAT, 0, &(m_vertices[idx]));


		if (hasVertexColors())
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, &(m_vcolors[idx]));
		}

		if (hasVertexNormals())
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, &(m_vnormals[idx]));
		}

		glDrawArrays(GL_POINTS, 0, std::min(PACKET_SIZE, m_vertices.size() - idx));

		if (hasVertexNormals())
		{
			glDisableClientState(GL_NORMAL_ARRAY);
		}

		if (hasVertexColors())
		{
			glDisableClientState(GL_COLOR_ARRAY);
		}

		idx += PACKET_SIZE;
	}

	glDisableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_COLOR_MATERIAL);

	glPopAttrib();
	glPopMatrix();

}

#include "AP.h"
#include "MainWindow.h"
#include "GLViewer.h"

#include "QtOpenGL/QGLShaderProgram"

#define highp
#define mediump
#define lowp


void CPointCloud::renderBACKUP()
{
	if (m_vertices.empty()) return;

	const QGLContext* context = QGLContext::currentContext();

	QGLShaderProgram program(context);

	program.addShaderFromSourceCode(QGLShader::Vertex,
		"attribute highp vec4 vertex;\n"
		"attribute vec4 color;\n"
		"varying lowp vec4 vertexColor;\n"
		"void main(void)\n"
		"{\n"
		"   gl_Position = gl_ModelViewProjectionMatrix * vertex;\n"
		"   vertexColor = color;\n"
		"}");
	
	program.addShaderFromSourceCode(QGLShader::Fragment,
		"varying lowp vec4 vertexColor;\n"
		"void main(void)\n"
		"{\n"
		"   gl_FragColor = vertexColor;\n"
		"}");
	
	program.link();
	program.bind();


	int vertexLocation = program.attributeLocation("vertex");
	program.enableAttributeArray(vertexLocation);
	program.setAttributeArray(vertexLocation, (GLfloat*) &(m_vertices[0]), 3);

	int colorLocation = program.attributeLocation("color");
	if (hasVertexColors())
	{
		program.enableAttributeArray(colorLocation);
		program.setAttributeArray(colorLocation, GL_UNSIGNED_BYTE, &(m_vcolors[0]), 4);
	}

	glDrawArrays(GL_POINTS, 0, m_vertices.size());

	if (hasVertexColors())
	{
		program.disableAttributeArray(colorLocation);
	}

	program.disableAttributeArray(vertexLocation);

	program.release();
}


size_t CPointCloud::addVertex(CVertex v, CRGBA c)
{
	size_t s = addVertex(v);
	if (m_vcolors.size() == s)
		m_vcolors.push_back(c);
	return s;
}

size_t CPointCloud::addVertex(CVertex v, CVector3d n)
{
	size_t s = addVertex(v);
	if (m_vnormals.size() == s)
		m_vnormals.push_back(n);
	return s;
}

size_t CPointCloud::addVertex(CVertex v, CRGBA c, CVector3d n)
{
	size_t s = addVertex(v, c);
	if (m_vnormals.size() == s)
		m_vnormals.push_back(n);
	return s;
}

bool CPointCloud::setVertex(size_t idx, CVertex v)
{
	if (m_vertices.size() > idx)
	{
		m_vertices[idx] = v;
		return m_modified = true;
	}
	return false;
}

bool CPointCloud::setVertexColor(size_t idx, CRGBA vColor)
{
	if (m_vertices.size() > idx)
	{
		if (m_vcolors.size() < m_vertices.size())
		{
			m_vcolors.resize(m_vertices.size(), CRGBA());
		}
		m_vcolors[idx] = vColor;

		return m_modified = true;
	}
	return false;
}

bool CPointCloud::setVertexNormal(size_t idx, CVector3f vNormal)
{
	if (m_vertices.size() > idx)
	{
		if (m_vnormals.size() < m_vertices.size())
		{
			m_vnormals.resize(m_vertices.size(), CVector3f());
		}
		m_vnormals[idx] = vNormal;

		return m_modified = true;
	}
	return false;
}

bool CPointCloud::removeVertex(size_t idx)
{
	if (m_vertices.size() > idx)
	{
		m_vertices.erase(m_vertices.begin() + idx);
		if (m_vcolors.size() > idx)
		{
			m_vcolors.erase(m_vcolors.begin() + idx);
		}
		if (m_vnormals.size() > idx)
		{
			m_vnormals.erase(m_vnormals.begin() + idx);
		}

		return m_modified = true;
	}
	return false;
}

void CPointCloud::rebuildBoundingBox()
{
	CBoundingBox::reset();
	for (CVertex& v : m_vertices)
	{
		CBoundingBox::expand(v);
	}
}


void CPointCloud::createKDtree()
{
	removeKDtree();
	UI::STATUSBAR::setText("I'm creating KDtree. Please wait...");
	m_kdtree = new CPointCloud::KDtree(m_vertices);
}

void CPointCloud::removeKDtree()
{
	if (nullptr != m_kdtree)
	{
		delete m_kdtree;
		m_kdtree = nullptr;
	}
}

CPointCloud::KDtree & CPointCloud::getKDtree(bool rebuild = CPointCloud::KDtree::PRESERVE)
{
	if ((nullptr == m_kdtree) || rebuild)
		createKDtree();

	return *m_kdtree;
}


CVertex CPointCloud::getClosestVertex(CPoint3f pkt, CVector3f vec)
{
	size_t id = 0;
	double dist = DBL_MAX;
	for (size_t j = 0; j < static_cast<size_t>(m_vertices.size()); j++) {
		//double tmp = CVector3f( vertices[j], pkt ).length();
		CVector3f tmpv = CVector3f(CPoint3f(m_vertices[j]), pkt);
		double tmp = tmpv.length();
		if (tmp < dist) {
			id = j;
			dist = tmp;
		}
	}
	return m_vertices[id];
}

CPoint3f CPointCloud::getCenterOfWeight()
{
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	if (!m_vertices.empty())
	{
		for (CPointCloud::Vertices::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
		{
			x += it->X();
			y += it->Y();
			z += it->Z();
		}

		x /= m_vertices.size();
		y /= m_vertices.size();
		z /= m_vertices.size();
	}

	return CPoint3f(x, y, z);
}

void CPointCloud::applyTransformation(CTransform& from, CTransform& to)
{
	CObject::applyTransformation(from, to);

	resetBoundingBox();
	for (size_t j = 0; j < static_cast<size_t>(m_vertices.size()); j++)
	{
		CTriple<double> t = to.w2l(from.l2w(m_vertices[j]));
		m_vertices[j] = t;
		expandBoundingBox(t);
	}
}

void CPointCloud::transformByMatrixF(float matrix[16])
{
	this->resetBoundingBox();
	for (CPointCloud::Vertices::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
	{
		*it = (*it).transformByMatrixF(matrix);
		this->expandBoundingBox(*it);
	}
	//		correctNormals();
	//		if (hasVertexNormals()) calcVN();
}

void CPointCloud::transformByMatrixD(double matrix[16])
{
	this->resetBoundingBox();
	for (CPointCloud::Vertices::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
	{
		CPoint3d v(*it);
		*it = v.transformByMatrixD(matrix);
		this->expandBoundingBox(*it);
	}
	//		correctNormals();
	//		if (hasVertexNormals()) calcVN();
}




#include "Plane.h"

void CPointCloud::cutPlane(CPlane* m_cutPlane, CPointCloud* &rest)
{
	rest = this->getCopy();

	if (rest == nullptr) return;

	CPointCloud::Vertices tmpv;
	CPointCloud::Colors tmpc;

	CPointCloud::Vertices tmpv2;
	CPointCloud::Colors tmpc2;

	for (int i = 0; i < m_vertices.size(); i++)
	{
		CVertex& v = m_vertices[i];
		if (0 >= m_cutPlane->testPointPosition(v))
		{
			tmpv2.push_back(v);
			if ( hasVertexColors() )
				tmpc2.push_back( m_vcolors[i] );
		}
		else
		{
			tmpv.push_back(v);
			if ( hasVertexColors() )
				tmpc.push_back( m_vcolors[i] );
		}
	}

	m_vertices = tmpv;
	m_vcolors = tmpc;

	rest->m_vertices = tmpv2;
	rest->m_vcolors = tmpc2;
}



CPointCloud* CPointCloud::findNClosest(CPointCloud* ruchoma, CPointCloud* nieruchoma, int limit)
{
	nieruchoma->createKDtree();

	std::multimap<double, int> dobre;

	for (int i = 0; i < ruchoma->size(); i++)
	{
		auto p = ruchoma->vertices()[i];

		int idx = nieruchoma->getKDtree(CPointCloud::KDtree::PRESERVE).closest_to_pt(p);
		double dist = CVector3d(p, nieruchoma->vertices()[idx]).length();

		dobre.insert({ dist,i });
	}

	CPointCloud* result = new CPointCloud;
	int size = (limit > dobre.size()) ? dobre.size() : limit;
	for (int i = 0; i < size; i++)
	{
		int idx = dobre.begin()->second;
		result->addVertex(ruchoma->vertices()[idx]);
		dobre.erase(dobre.begin());
	}

	nieruchoma->removeKDtree();
	return result;
}




// KDTREE


std::vector<INDEX_TYPE> CPointCloud::KDtree::find_k_closest_to_pt(int k, CPoint3f p, float maxdist) const
{
	std::vector<const float *> knn;
	trimesh::KDtree::find_k_closest_to_pt(knn, k, p.toVector(), maxdist*maxdist, NULL);

	std::vector<INDEX_TYPE> wynik;

	for (std::vector<const float *>::iterator it = knn.begin(); it != knn.end(); it++)
	{
		wynik.push_back((const CVertex *)*it - &m_vertices[0]);
	}

	return wynik;
}

std::vector<INDEX_TYPE> CPointCloud::KDtree::find_k_closest_to_ray(int k, CPoint3f p, CVector3f dir, float maxdist) const
{
	std::vector<const float *> knn;
	trimesh::KDtree::find_k_closest_to_ray(knn, k, p.toVector(), dir.toVector(), maxdist*maxdist, NULL);

	std::vector<INDEX_TYPE> wynik;

	for (std::vector<const float *>::iterator it = knn.begin(); it != knn.end(); it++)
	{
		wynik.push_back((const CVertex *)*it - &m_vertices[0]);
	}

	return wynik;
}

bool CPointCloud::KDtree::is_any_in_distance_to_pt(float distance, CPoint3f p) const
{
	return trimesh::KDtree::is_any_in_distance_to_pt(distance, p.toVector());
}

std::vector<INDEX_TYPE> CPointCloud::KDtree::find_all_in_distance_to_pt(float distance, CPoint3f p) const
{
	std::vector<const float *> knn;
	trimesh::KDtree::find_all_in_distance_to_pt(knn, distance, p.toVector());

	std::vector<INDEX_TYPE> wynik;

	for (std::vector<const float *>::iterator it = knn.begin(); it != knn.end(); it++)
	{
		wynik.push_back((const CVertex *)*it - &m_vertices[0]);
	}

	return wynik;
}


std::vector<INDEX_TYPE> CPointCloud::KDtree::find_all_in_distance_to_ray(float distance, CPoint3f p, CVector3f dir) const
{
	std::vector<const float *> knn;
	trimesh::KDtree::find_all_in_distance_to_ray(knn, distance, p.toVector(), dir.toVector());

	std::vector<INDEX_TYPE> wynik;
	wynik.reserve(knn.size());

	for (std::vector<const float *>::iterator it = knn.begin(); it != knn.end(); it++)
	{
		wynik.push_back((const CVertex *)*it - &m_vertices[0]);
	}

	return wynik;
}


std::vector<std::pair<INDEX_TYPE, float>> CPointCloud::KDtree::find_all_in_distance_to_ray_2(float distance, CPoint3f p, CVector3f dir) const
{
	std::vector< std::pair<float, const float *> > knn;
	trimesh::KDtree::find_all_in_distance_to_ray_2(knn, distance, p.toVector(), dir.toVector());

	std::vector<std::pair<INDEX_TYPE, float>> wynik;

	wynik.reserve(knn.size());

	for (std::vector< std::pair<float, const float *> >::iterator it = knn.begin(); it != knn.end(); it++)
	{
		wynik.push_back(std::pair<INDEX_TYPE, float>((const CVertex *)it->second - &m_vertices[0], it->first));
	}

	return wynik;
}

void CPointCloud::KDtree::clear()
{
	while (!kontener.empty())
	{
		delete kontener.back();
		kontener.pop_back();
	}
	storage = NULL;
	root = NULL;
}

void CPointCloud::KDtree::rebuild()
{
	clear();
	trimesh::KDtree::build((const float *)&m_vertices[0], m_vertices.size());
}


