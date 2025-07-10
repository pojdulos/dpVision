#pragma once
#ifndef _VOLTK_H_BY_DP_INCLUDED_
#define _VOLTK_H_BY_DP_INCLUDED_

#include "Object.h"
//#include "Mesh.h"

#include <map>

typedef unsigned char BYTE;
typedef unsigned short WORD;


struct SVolTK {
	typedef struct {
		unsigned char bitLeftX:1;
		unsigned char bitRightX:1;
		unsigned char bitLeftY:1;
		unsigned char bitRightY:1;
		unsigned char bitLeftZ:1;
		unsigned char bitRightZ:1;
	} Neighbourhood;

	Neighbourhood n;

	bool isInside() { return 0 == n.bitLeftX+n.bitRightX+n.bitLeftY+n.bitRightY+n.bitLeftZ+n.bitRightZ; };
	
	WORD c;
};

class CModel3D;
class CMesh;
class CPointCloud;
class QOpenGLContext;
class QOpenGLFunctions;
class QOpenGLShaderProgram;

typedef WORD* WORDptr;

#include "Image.h"
#include "RAWdicomData.h"
#include "NowaKostka1.h"

typedef std::vector<CPoint3s> VolTKdisplayDataPart;
typedef std::vector<VolTKdisplayDataPart*> VolTKdisplayData;

class DPVISION_EXPORT CVolTK : public CObject
{
public:
	typedef VolTKdisplayDataPart DisplayDataPart;
	typedef VolTKdisplayData DisplayData;
	typedef std::pair<int,std::pair<bool,CRGBA>> ColorFilter;
	typedef std::map<int, std::pair<bool, CRGBA>> ColorFilterMap;
	typedef enum { XY, YZ, ZX } Layer;

	CVolTK(std::shared_ptr<CBaseObject> p, int imageWidth, int imageHeight, int numberOfImages, int depth );

	// konstruktory kopiuj¹ce
//	CVolTK(const CVolTK& src);
//	CVolTK(const CVolTK& src, CPoint3s mB, CPoint3s mE);  // wycinek: ( mB <= [x,y,z] < mE )
//	CVolTK(const CVolTK& src, uint16_t lowerBound, uint16_t upperBound);  // wybrane kolory: ( lowerBound <= [kolor] < upperBound )
	CVolTK(const CVolTK& src, CPoint3s mB, CPoint3s mE, uint16_t lowerBoundC, uint16_t upperBoundC);  // wycinek: ( mB <= [x,y,z] < mE )

	~CVolTK(void);

	virtual std::shared_ptr<CBaseObject> getCopy() override;

	CVolTK* getPart(CPoint3s min, CPoint3s max);
	CVolTK* getPart(uint16_t wMin, uint16_t wMax);


	virtual inline int type() override { return CObject::Type::VOLTK; };
	virtual std::wstring infoRow() override;

	virtual void renderSelf() override;

	size_t addLayerW( int layerId, std::vector<uint16_t>* layer, uint16_t lowTreshold = 0, uint16_t upTreshold = 0xffff );

	size_t addLayerB(int layerId, std::vector<BYTE>* layer, BYTE lowTreshold = 0, BYTE upTreshold = 0xff);

	std::shared_ptr<CMesh> createMesh(int winMin, int winMax, CRGBA col);
	std::shared_ptr<CModel3D> toMesh();

	std::shared_ptr<CPointCloud> createCloud(uint16_t winMin, uint16_t winMax, CRGBA col);
	std::shared_ptr<CModel3D> toCloud();

	//void copyOneKostkaLayer(int16_t layer, int pixelRepresentation);

	void createDisplayData();

	void addVoxelW(int16_t x, int16_t y, int16_t z, uint16_t c);
	void addVoxelB(int16_t x, int16_t y, int16_t z, BYTE c);

	NowaKostka1 kostka;


	DisplayData m_displayData;

	size_t size() { return m_displayData.size(); };
	void clear();

	static std::shared_ptr<CVolTK> create(int w, int h, int l, int v = 0, int d = 16);

	//inline int& cols() { return m_imW; }
	//inline int& rows() { return m_imH; }
	//inline int& lays() { return m_imNb; }
	inline int& depth() { return m_imDepth; }
	inline int cols() { return kostka.m_cols; }
	inline int rows() { return kostka.m_rows; }
	inline int lays() { return kostka.m_lays; }
	inline void setColsRows(int c, int r) { kostka.init(c, r, lays()); }
	CPoint3d getVoxelSize() { return kostka.m_voxelSize; }

	struct {
		uint16_t lower;
		uint16_t upper;
	} m_dataIntensityRange, m_currentIntensityRange;

	int16_t m_div;

	CPoint3f m_b, m_e;

	CPoint3s m_Min, m_Max;

	float m_alpha;

	ColorFilterMap colorFilters;

	CPoint3d realXYZ(CPoint3d point);
	CPoint3d realXYZ(double x, double y, double z);
	void setGeometry();

	QImage getLayerAsImage(int16_t nr, CVolTK::Layer layer, int dispBits=16);

private:
	int m_imDepth;

	QOpenGLContext* context;
	QOpenGLFunctions* functions;
	//QOpenGLShaderProgram program;
	int vertexLocation;
	int colorLocation;

	bool addShaders(QOpenGLShaderProgram& program);
	bool addShadersBAK(QOpenGLShaderProgram& program);

	void renderPart(QOpenGLShaderProgram& program, uint16_t min, uint16_t max, int fR, int fG, int fB);
	void renderPart(QOpenGLShaderProgram& program, uint16_t min, uint16_t max, QColor col);

	void renderPart2(QOpenGLShaderProgram& program, uint16_t min, uint16_t max);

protected:
	//_vof4 myFaces;
	//_vof4::iterator face4;

	//void volumAddBox( CMesh &mesh, float ix, float iy, float iz, float w, CRGBA color );
	//void volumReGenerate( CMesh &mesh );
	//void volumPrepare( CMesh &mesh, unsigned int options );
	//void volumRender1( CMesh &mesh, unsigned int options );
};


#endif /* _VOLTK_H_BY_DP_INCLUDED_ */