#pragma once

#include "BaseObject.h"
#include "Face.h"
#include "Material.h"
#include "Texture.h"
#include <QOpenGLShaderProgram>

class  DPVISION_EXPORT CPolygonGroup//	 : public CBaseObject
{
public:
	typedef std::vector<CFace> Faces;
	typedef std::vector<CRGBA> Colors;
	typedef std::vector<CVector3f> Normals;

private:
	CBaseObject *m_parent;
	Faces m_faces;
	Colors m_colors;
	Normals m_normals; //faces normals array is obligatory
	size_t m_matIdx;
	std::map<const QOpenGLContext*, QOpenGLShaderProgram*> programs;
	GLint inPositionLoc;
	GLint inVColorsLoc;
	GLint inTexCoordLoc;
	GLint textureUniform;
	GLint useVNormalsLoc;
	GLint inNormalsLoc;
	GLuint EBO;

public:
	CPolygonGroup( CBaseObject *p = nullptr);
	CPolygonGroup( CPolygonGroup &m );
	~CPolygonGroup();

	CPolygonGroup& operator=(const CPolygonGroup&);

	void setParent(CBaseObject *p) { m_parent = p; };

	inline Faces &faces() { return m_faces; };
	inline Normals &normals() { return m_normals; };
	inline Colors &colors() { return m_colors; };

	CFace & operator[](INDEX_TYPE index) { return m_faces[index]; }
	const CFace & operator[](INDEX_TYPE index) const { return m_faces[index]; }

	inline size_t size() { return m_faces.size(); };

	inline bool hasColors() { return !m_colors.empty() && (m_colors.size() >= m_faces.size()); };
	inline bool hasNormals() { return !m_normals.empty() && (m_normals.size() >= m_faces.size()); };

	INDEX_TYPE addFace(CFace f);
	INDEX_TYPE addFace(CFace f, CRGBA fColor);

	void removeFace(INDEX_TYPE idx);
	void removeFace(CPolygonGroup::Faces::iterator itf);
	void removeFace(CPolygonGroup::Faces::reverse_iterator itfr);

	void renderSelf();
	void renderSelf_OLD();
	void renderSelf_NEW();

private:
	void initQGLShaderProgram();
	void releaseQGLShaderProgram();
	void renderWithShaders();
};

