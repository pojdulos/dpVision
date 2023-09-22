#include "Global.h"
#include "PolygonGroup.h"
#include "GLViewer.h"
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGLExtensions/QtOpenGLExtensions>

CPolygonGroup::CPolygonGroup( CBaseObject* p ) //: CBaseObject( p )
{
	m_parent = p;
	m_matIdx = 0;

	initQGLShaderProgram();
}

CPolygonGroup::CPolygonGroup( CPolygonGroup &m ) //: CBaseObject( m )
{
	m_parent = m.m_parent;
	m_faces = CPolygonGroup::Faces(m.m_faces);
	m_colors = CPolygonGroup::Colors(m.m_colors);
	m_normals = CPolygonGroup::Normals(m.m_normals);

	m_matIdx = 0;

	initQGLShaderProgram();
}


CPolygonGroup::~CPolygonGroup()
{
	m_faces.clear();
	m_colors.clear();
	m_normals.clear();

	releaseQGLShaderProgram();
}

CPolygonGroup& CPolygonGroup::operator=(const CPolygonGroup& m)
{
	m_parent = m.m_parent;
	m_faces = CPolygonGroup::Faces(m.m_faces);
	m_colors = CPolygonGroup::Colors(m.m_colors);
	m_normals = CPolygonGroup::Normals(m.m_normals);

	m_matIdx = 0;

	return *this;
}

#include "Mesh.h"

INDEX_TYPE CPolygonGroup::addFace(CFace f)
{
	m_faces.push_back(f);
	if (!m_colors.empty() && m_colors.size() == m_faces.size() - 1)
		m_colors.push_back(CRGBA());
	return m_faces.size() - 1;
}

INDEX_TYPE CPolygonGroup::addFace(CFace f, CRGBA fColor)
{
	m_faces.push_back(f);
	if (m_colors.size() == m_faces.size() - 1)
		m_colors.push_back(fColor);
	else
	{
		m_colors.resize(m_faces.size(),CRGBA());
		m_colors[m_faces.size() - 1] = fColor;
	}
	return m_faces.size() - 1;
}

void CPolygonGroup::removeFace(INDEX_TYPE idx)
{
	m_faces.erase( m_faces.begin() + idx );
	
	if (hasColors())
	{
		m_colors.erase( m_colors.begin() + idx );
	}

	if (hasNormals())
	{
		m_normals.erase( m_normals.begin() + idx );
	}
}

void CPolygonGroup::removeFace(CPolygonGroup::Faces::iterator itf)
{
	size_t idx = itf - m_faces.begin();

	m_faces.erase( itf );

	if (hasColors())
	{
		m_colors.erase(m_colors.begin() + idx);
	}

	if (hasNormals())
	{
		m_normals.erase(m_normals.begin() + idx);
	}
}

void CPolygonGroup::removeFace(CPolygonGroup::Faces::reverse_iterator itfr)
{
	// &*(rit.base() - 1) == &*rit
	// https://en.cppreference.com/w/cpp/iterator/reverse_iterator/base

	removeFace( itfr.base() - 1 );
}


#include "PolygonGroupShaders.h"

void CPolygonGroup::initQGLShaderProgram()
{
}

void CPolygonGroup::releaseQGLShaderProgram()
{
	for (auto& program : programs)
	{
		delete program.second;
	}
	
	programs.clear();
}

void CPolygonGroup::renderWithShaders()
{
	//qDebug() << "renderWithShaders()" << endl;
	CMesh* mesh = (CMesh*)m_parent;
	CMaterial& material = mesh->getMaterial(m_matIdx);

	const QOpenGLContext* context = QOpenGLContext::currentContext();

	if (programs.find(context) == programs.end())
	{
		programs[context] = new QOpenGLShaderProgram();

		programs[context]->addShaderFromSourceCode(QOpenGLShader::Vertex, POLYGON_GROUP_VERTEX_SHADER);
		programs[context]->addShaderFromSourceCode(QOpenGLShader::Fragment, POLYGON_GROUP_FRAGMENT_SHADER);

		programs[context]->link();
	}

	QOpenGLShaderProgram *program = programs[context];

	program->bind();

	CMesh::Faces newFaces(mesh->faces().size());
	CMesh::Vertices newVertices(mesh->faces().size() * 3);
	CMesh::Colors newColors(mesh->faces().size() * 3);
	CMesh::Normals newNormals(mesh->faces().size() * 3);

	CMesh::Faces *faces = &m_faces;
	CVertex *vertexDataPtr = mesh->vertices().data();
	CRGBA *colorDataPtr = mesh->vcolors().data();
	CVector3f *normalDataPtr = mesh->vnormals().data();

	bool hVC = mesh->hasVertexColors();
	bool rSV = mesh->renderSmoothVertex();

	if (mesh->renderFacesColors() || !mesh->renderSmoothVertex())
	{
		for (size_t i = 0; i < m_faces.size(); ++i) {
			size_t idx = i * 3;

			CFace ff = m_faces[i];

			newVertices[idx] = mesh->vertices()[ff.A()];
			newVertices[idx + 1] = mesh->vertices()[ff.B()];
			newVertices[idx + 2] = mesh->vertices()[ff.C()];

			newFaces[i] = CFace(idx, idx + 1, idx + 2);

			if (mesh->renderFacesColors())
			{
				newColors[idx] = m_colors[i];
				newColors[idx + 1] = m_colors[i];
				newColors[idx + 2] = m_colors[i];
			}
			else if (mesh->hasVertexColors())
			{
				newColors[idx] = mesh->vcolors()[ff.A()];
				newColors[idx + 1] = mesh->vcolors()[ff.B()];
				newColors[idx + 2] = mesh->vcolors()[ff.C()];
			}

			if (!mesh->renderSmoothVertex())
			{
				if (mesh->hasFaceNormals())
				{
					newNormals[idx] = m_normals[i];
					newNormals[idx + 1] = m_normals[i];
					newNormals[idx + 2] = m_normals[i];
				}
				else
				{
					CVector3f vn = ff.getNormal(mesh->vertices());
					newNormals[idx] = vn;
					newNormals[idx + 1] = vn;
					newNormals[idx + 2] = vn;
				}
			}
			else
			{
				newNormals[idx] = mesh->vnormals()[ff.A()];
				newNormals[idx + 1] = mesh->vnormals()[ff.B()];
				newNormals[idx + 2] = mesh->vnormals()[ff.C()];
			}
		}

		faces = &newFaces;

		vertexDataPtr = newVertices.data();

		if (mesh->renderFacesColors())
		{
			colorDataPtr = newColors.data();
			hVC = true;
		}
		else if (mesh->hasVertexColors())
		{
			colorDataPtr = newColors.data();
			hVC = true;
		}

		normalDataPtr = newNormals.data();
		rSV = true;
	}



	//wierzcho³ki
	inPositionLoc = program->attributeLocation("inPosition");
	program->enableAttributeArray(inPositionLoc);
	program->setAttributeArray(inPositionLoc, (GLfloat*)vertexDataPtr, 3);

	// kolory
	QVector4D kolor(material.FrontColor.diffuse.fR(), material.FrontColor.diffuse.fG(), material.FrontColor.diffuse.fB(), material.FrontColor.diffuse.fA());
	program->setAttributeValue("alternativeColor", kolor);

	if (material.m_force)
	{
		program->setUniformValue("useAlternativeColor", true);
	}
	else
	{
		program->setUniformValue("useAlternativeColor", false);
	}

	inVColorsLoc = program->attributeLocation("inVColors");




	if (hVC)
	{
		program->enableAttributeArray(inVColorsLoc);
		program->setAttributeArray(inVColorsLoc, GL_UNSIGNED_BYTE, colorDataPtr, 4);
		program->setUniformValue("useVColors", true);
	}
	else
	{
		program->setUniformValue("useVColors", false);
	}

	//koordynaty tekstury
	inTexCoordLoc = program->attributeLocation("inTexCoord");
	textureUniform = program->uniformLocation("ourTexture");
	if (mesh->renderWithTexture())
	{
		context->functions()->glActiveTexture(GL_TEXTURE0);
		material.tekstura->bind();

		program->setUniformValue("useTexture", true);
		program->setUniformValue(textureUniform, 0);

		program->enableAttributeArray(inTexCoordLoc);
		program->setAttributeArray(inTexCoordLoc, GL_FLOAT, &(material.texcoord[0]), 2);
	}
	else
	{
		program->setUniformValue("useTexture", false);
	}

	// Zmienna steruj¹ca wektorami normalnymi
	useVNormalsLoc = program->uniformLocation("useVNormals");
	inNormalsLoc = program->attributeLocation("inNormals");
	if (rSV) {
		program->enableAttributeArray(inNormalsLoc);
		program->setAttributeArray(inNormalsLoc, GL_FLOAT, normalDataPtr, 3);
		program->setUniformValue(useVNormalsLoc, true);
	}
	else {
		program->setUniformValue(useVNormalsLoc, false);
	}


	// trójk¹ty
	context->functions()->glGenBuffers(1, &EBO);
	context->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	size_t idx = 0;

	while (idx < faces->size())
	{
		size_t size = std::min(PACKET_SIZE, faces->size() - idx);
		
		//rysowanie
		context->functions()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(INDEX_TYPE) * 3, &((*faces)[idx]), GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES, size * 3, GL_UNSIGNED_INT, 0);
		
		idx += PACKET_SIZE;
	}

	//czyszczenie
	context->functions()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	context->functions()->glDeleteBuffers(1, &EBO);

	if (hVC)
	{
		program->disableAttributeArray(inVColorsLoc);
	}

	if (mesh->renderWithTexture())
	{
		program->disableAttributeArray(inTexCoordLoc);
		material.tekstura->release();
	}

	if (rSV) {
		program->disableAttributeArray(inNormalsLoc);
	}

	program->disableAttributeArray(inPositionLoc);

	program->release();

}


void CPolygonGroup::renderSelf_NEW()
{
	float buf[5];
	CMesh* mesh = (CMesh*)m_parent;
	CMaterial& material = mesh->getMaterial(m_matIdx);

	if (mesh->renderFacesAsPoints())
	{
		glPolygonMode(GL_FRONT, GL_POINT);
		glPolygonMode(GL_BACK, GL_POINT);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(CPointCloud::m_pointSize);
	}
	else if (mesh->renderFacesAsEdges())
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_LINE);
	}

	renderWithShaders();
	return;

	if (mesh->renderSmoothVertex())// && !mesh->renderFacesColors() )
	{
		renderWithShaders();
		return;
	}
	else
	{
		CPolygonGroup::Colors::iterator color;

		if (!material.m_force)
		{
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

			color = mesh->fcolors().begin();
			
		}
		else
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT, material.FrontColor.ambient.fV(buf));
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material.FrontColor.diffuse.fV(buf));
			glMaterialfv(GL_FRONT, GL_SPECULAR, material.FrontColor.specular.fV(buf));
			glMaterialfv(GL_FRONT, GL_EMISSION, material.FrontColor.emission.fV(buf));
			glMaterialf(GL_FRONT, GL_SHININESS, material.FrontColor.shininess);

			glMaterialfv(GL_BACK, GL_AMBIENT, material.BackColor.ambient.fV(buf));
			glMaterialfv(GL_BACK, GL_DIFFUSE, material.BackColor.diffuse.fV(buf));
			glMaterialfv(GL_BACK, GL_SPECULAR, material.BackColor.specular.fV(buf));
			glMaterialfv(GL_BACK, GL_EMISSION, material.BackColor.emission.fV(buf));
			glMaterialf(GL_BACK, GL_SHININESS, material.BackColor.shininess);
		}


		glBegin(GL_TRIANGLES);

		CPolygonGroup::Faces::iterator face = m_faces.begin();
		CPolygonGroup::Normals::iterator vnface = m_normals.begin();

		bool hasVn = m_normals.size() >= m_faces.size();

		for (; face != m_faces.end(); face++)
		{
			if (!material.m_force)
			{
				if (mesh->renderFacesColors())
				{
					glColor4ubv(color->V());
					color++;
				}
				else if (mesh->hasVertexColors())
				{
					glColor4ubv(mesh->vcolors()[face->A()].V());
				}
			}

			if ( mesh->renderSmoothVertex() )
				glNormal3fv( mesh->vnormals()[face->A()].toVector() );
			else
			{
				if (hasVn)
				{
					glNormal3fv(vnface->toVector());
					vnface++;
				}
				else
				{
					glNormal3dv(face->getNormal(mesh->vertices()).toVector());
				}

			}

			glVertex3fv(mesh->vertices()[face->A()].toVector());

			if (!mesh->renderFacesColors() && mesh->hasVertexColors() && !material.m_force)
			{
				glColor4ubv(mesh->vcolors()[face->B()].V());
			}

			if (mesh->renderSmoothVertex())
				glNormal3fv( mesh->vnormals()[face->B()].toVector() );

			glVertex3fv(mesh->vertices()[face->B()].toVector());

			if (!mesh->renderFacesColors() && mesh->hasVertexColors() && !material.m_force)
			{
				glColor4ubv(mesh->vcolors()[face->C()].V());
			}

			if (mesh->renderSmoothVertex())
				glNormal3fv( mesh->vnormals()[face->C()].toVector() );

			glVertex3fv(mesh->vertices()[face->C()].toVector());
		}
		
		glEnd();
		glDisable(GL_COLOR_MATERIAL);
	}
};

void CPolygonGroup::renderSelf_OLD()
{
	float buf[5];
	CMesh* mesh = (CMesh*)m_parent;
	CMaterial& material = mesh->getMaterial(m_matIdx);

	if ((mesh->hasVertexColors() || hasColors()) && !material.m_force)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	}

	if ((!mesh->hasVertexColors() && !hasColors() && !mesh->renderWithTexture()) || material.m_force)
	{
		glMaterialfv(GL_FRONT, GL_AMBIENT, material.FrontColor.ambient.fV(buf));
		glMaterialfv(GL_FRONT, GL_DIFFUSE, material.FrontColor.diffuse.fV(buf));
		glMaterialfv(GL_FRONT, GL_SPECULAR, material.FrontColor.specular.fV(buf));
		glMaterialfv(GL_FRONT, GL_EMISSION, material.FrontColor.emission.fV(buf));
		glMaterialf(GL_FRONT, GL_SHININESS, material.FrontColor.shininess);

		glMaterialfv(GL_BACK, GL_AMBIENT, material.BackColor.ambient.fV(buf));
		glMaterialfv(GL_BACK, GL_DIFFUSE, material.BackColor.diffuse.fV(buf));
		glMaterialfv(GL_BACK, GL_SPECULAR, material.BackColor.specular.fV(buf));
		glMaterialfv(GL_BACK, GL_EMISSION, material.BackColor.emission.fV(buf));
		glMaterialf(GL_BACK, GL_SHININESS, material.BackColor.shininess);

		//glEnable(GL_COLOR_MATERIAL);

	}

	if (mesh->renderWithTexture())
	{
		glBindTexture(GL_TEXTURE_2D, material.tekstura->textureId());
		glEnable(GL_TEXTURE_2D);
	}

	if (mesh->renderFacesAsPoints())
	{
		glPolygonMode(GL_FRONT, GL_POINT);
		glPolygonMode(GL_BACK, GL_POINT);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(CPointCloud::m_pointSize);
	}
	else if (mesh->renderFacesAsEdges())
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_LINE);
	}


	CMaterial::TextureIndexes::iterator texIdx;
	if (mesh->renderWithTexture()) texIdx = material.texindex.begin();

	CPolygonGroup::Colors::iterator color;
	if ((mesh->hasVertexColors() || mesh->hasFaceColors()) && !material.m_force)
		color = mesh->fcolors().begin();

	glBegin(GL_TRIANGLES);

	CPolygonGroup::Faces::iterator face = m_faces.begin();
	CPolygonGroup::Normals::iterator vnface = m_normals.begin();

	bool hasVn = m_normals.size() >= m_faces.size();

	for (; face != m_faces.end(); face++)
	{
		if (mesh->hasVertexColors() && !material.m_force)
			glColor4ubv(mesh->vcolors()[face->A()].V());
		else if (hasColors() && !material.m_force)
		{
			glColor4ubv(color->V());
			color++;
		}

		if (mesh->renderSmoothVertex())
			glNormal3fv(mesh->vnormals()[face->A()].toVector());
		else
		{
			if (hasVn)
			{
				glNormal3fv(vnface->toVector());
				vnface++;
			}
			else
			{
				glNormal3dv(face->getNormal(mesh->vertices()).toVector());
			}

		}

		if (mesh->renderWithTexture())
			glTexCoord2f(material.texcoord[texIdx->a].s, material.texcoord[texIdx->a].t);

		glVertex3fv(mesh->vertices()[face->A()].toVector());

		if (mesh->hasVertexColors() && !material.m_force)
			glColor4ubv(mesh->vcolors()[face->B()].V());

		if (mesh->renderSmoothVertex())
			glNormal3fv(mesh->vnormals()[face->B()].toVector());

		if (mesh->renderWithTexture())
			glTexCoord2f(material.texcoord[texIdx->b].s, material.texcoord[texIdx->b].t);

		glVertex3fv(mesh->vertices()[face->B()].toVector());

		if (mesh->hasVertexColors() && !material.m_force)
			glColor4ubv(mesh->vcolors()[face->C()].V());

		if (mesh->renderSmoothVertex())
			glNormal3fv(mesh->vnormals()[face->C()].toVector());

		if (mesh->renderWithTexture())
			glTexCoord2f(material.texcoord[texIdx->c].s, material.texcoord[texIdx->c].t);

		glVertex3fv(mesh->vertices()[face->C()].toVector());

		if (mesh->renderWithTexture())
			texIdx++;
	}

	glEnd();


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
};

void CPolygonGroup::renderSelf()
{
	renderSelf_OLD();
}
