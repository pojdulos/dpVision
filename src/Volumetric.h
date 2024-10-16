#pragma once
#include "Object.h"
#include "Vector3.h"
#include <QtCore/QString>
#include <vector>

struct SliceMetadata
{
	float image_position_patient[3] = { 0.0f, 0.0f, 0.0f };
	float slice_location = 0.0f;
	float pixel_spacing[2] = { 1.0f, 1.0f };
	float slice_thickness = 1.0f;
	float slice_distance = 1.0f;
	float gantry_detector_tilt = 0.0f;
	float rescale_slope = 1.0f;
	float rescale_intercept = 0.0f;

	//QString toString() {
	//	txt = f"SliceMetadata"
	//		txt += f"( gantry_detector_tilt = {self.gantry_detector_tilt}"
	//		txt += f", slice_thickness = {self.slice_thickness}"
	//		txt += f", slice_location = {self.slice_location}"
	//		txt += f", pixel_spacing = {self.pixel_spacing}"
	//		txt += f", image_position_patient = {self.image_position_patient} )"
	//		return txt
	//}
};


class QOpenGLShaderProgram;
class QOpenGLFunctions;

class CPointCloud;
class CMesh;

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#define _VoxelTypeINT16
//#define _VoxelTypeFLOAT

#if defined(_VoxelTypeFLOAT)
	#define VoxelTypeMax float(32767)
	#define VoxelTypeMin float(-32768)
#elif defined(_VoxelTypeINT16)
	#define VoxelTypeMax int16_t(32767)
	#define VoxelTypeMin int16_t(-32768)
#endif


class DPVISION_EXPORT Volumetric : public CObject
{
public:
#if defined(_VoxelTypeFLOAT)
	using VoxelType = float;
#elif defined(_VoxelTypeINT16)
	using VoxelType = int16_t;
#endif

	class SliceType : public std::vector<VoxelType>
	{
		unsigned int m_rows;
		unsigned int m_columns;

	public:
		SliceType() :m_rows(0), m_columns(0) {}
		SliceType(unsigned int _rows, unsigned int _cols, VoxelType color = 0.0) :std::vector<VoxelType>(_rows* _cols), m_rows(_rows), m_columns(_cols) {}

		// konstruktor kopiuj�cy
		SliceType(const SliceType& b) : std::vector<VoxelType>(b), m_rows(b.m_rows), m_columns(b.m_columns) {}

		inline VoxelType& at(unsigned int row, unsigned int column) { return (*this)[row * m_columns + column]; }
		inline const VoxelType& at(unsigned int row, unsigned int column) const { return (*this)[row * m_columns + column]; }

		inline bool isValid() { return (m_rows > 0) && (m_columns > 0) && (m_rows*m_columns==size()); }

		void setSize(unsigned int _rows, unsigned int _cols)
		{
			resize(_rows * _cols);
			m_rows = _rows;
			m_columns = _cols;
		}

		inline unsigned int rows() { return m_rows; }
		inline unsigned int columns() { return m_columns; }
	};
	
	typedef std::vector<SliceType> VolumeType;
	typedef SliceMetadata SliceMetadata;
	
	typedef enum { XY, YZ, ZX, TEST } LayerPlane;

	int i = 0;
	GLuint vbo = 0;
	//QOpenGLVertexArrayObject vao;
	//QOpenGLShaderProgram* shader_program = nullptr;
	GLuint shader_program = 0;
	
	VoxelType m_minVal = 0.0f;
	VoxelType m_maxVal = 1.0f;
	VoxelType m_minDisplWin = 0.0f;
	VoxelType m_maxDisplWin = 1.0f;
	
	bool m_fastDraw = true;
	bool m_renderBoxes = false;
	std::vector<SliceMetadata> metadata;
	int m_minSlice = 0;
	int m_maxSlice = 0;
	int m_minRow = 0;
	int m_maxRow = 0;
	int m_minColumn = 0;
	int m_maxColumn = 0;
	float m_filters[7][3] = {
		{0, -100, 799},
		{0, VoxelTypeMin, VoxelTypeMax},
		{0, VoxelTypeMin, VoxelTypeMax},
		{0, -1000, 4000},
		{0, VoxelTypeMin, VoxelTypeMax},
		{0, VoxelTypeMin, VoxelTypeMax},
		{0, 800, 4095} };

	float m_fcolors[7][3] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 1.0f},
		{1.0f, 0.0f, 1.0f},
		{1.0f, 1.0f, 1.0f} };


	Volumetric(CBaseObject* p = nullptr):CObject(p) {};
	~Volumetric() {};

	virtual inline int type() override { return CObject::Type::VOLUMETRIC_NEW; };
	virtual std::wstring infoRow() override;
	virtual Volumetric* getCopy() override;

	//GLuint compile_shader(QOpenGLFunctions* functions, QString source, GLenum shader_type);

	void remove_shader_program();

	bool create_program(QOpenGLFunctions* f);
	
	virtual void renderSelf() override;

	CPointCloud* sift_cloud( int nfeatures = 0, int nOctaveLayers = 3, double contrastThreshold = 0.04, double edgeThreshold = 10, double sigma = 1.6, int factor = 1 );
	CMesh* marching_cube(int factor = 1);
	CMesh* marching_tetrahedron(int factor = 1);

	bool getSlice(int nr, Volumetric::LayerPlane layer, Volumetric::SliceType* slice);

	Volumetric* getRotatedVol(Volumetric::LayerPlane dir);

	QImage getLayerAsImage(int nr, Volumetric::LayerPlane layer, bool tresh=false);
	QImage getLayerAsArgbImage(int nr, Volumetric::LayerPlane layer, bool tresh=false);

	QImage getRTGasImage(Volumetric::LayerPlane plane, bool tresh=false);

	CPoint3d realXYZ(CPoint3d pt);

	CPoint3d realXYZ(int x, int y, int z);

	CPoint3d getVoxelSize();

	CPoint3d getOrigin();

	static Volumetric* create(int layers = 256, int rows = 256, int columns = 256);
	void adjustMinMax(bool calc_color = true);
	void adjustMinMaxColor(VoxelType color);
	void drawBox(CPoint3i origin, CPoint3i size, VoxelType color);

	void drawSphere(CPoint3i origin, int radius, VoxelType color);

	void drawCylinder(CPoint3i origin, int radius, int height, char axis, VoxelType color);

	void drawCylinder(CPoint3f p0, CPoint3f p1, float radius, VoxelType color);

	void drawCylinder(CPoint3i origin, CVector3i vector, int radius, VoxelType color);


	static Volumetric* scal1(Volumetric*, Volumetric*);

	float pointToPlaneDistance(const CPoint3f& point, const CPoint3f& normal, const CPoint3f& planePoint);

	float interpolateVoxel(const Volumetric::VolumeType& volume, const CPoint3f& point);

	QImage generateFreeViewSliceImage(const CVector3f& normal, const CPoint3f& planePoint, const CVector3f& free_up, int width, int height, float scale, bool tresh=false);

	QImage generateFreeViewRtgImage(const CVector3f& normal, const CPoint3f& planePoint, int width, int height, float scale, bool tresh);

	/**************************************************************************/

	inline const bool empty() const { return m_data.empty(); }
	inline const size_t size() const { return m_data.size(); }
	inline void push_back(const SliceType& slice) { m_data.push_back(slice); }

	inline void reverse() {
		std::reverse(m_data.begin(), m_data.end());
		std::reverse(metadata.begin(), metadata.end());
	}

	inline unsigned int& layers() { return m_layers; }
	inline unsigned int& rows() { return m_rows; }
	inline unsigned int& columns() { return m_columns; }

	inline VoxelType& at(unsigned int layer, unsigned int row, unsigned int column) { return m_data[layer][row*m_columns+column]; }
	inline const VoxelType& at(unsigned int layer, unsigned int row, unsigned int column) const { return m_data[layer][row*m_columns+column]; }

	inline SliceType& operator[](unsigned int layer) { return m_data[layer]; }
	inline const SliceType& operator[](unsigned int layer) const { return m_data[layer]; }

	inline SliceType& getLayer(unsigned int layer) { return m_data[layer]; }
	inline const SliceType& getLayer(unsigned int layer) const { return m_data[layer]; }

	VoxelType& altData(unsigned int idx, unsigned int layer, unsigned int row, unsigned int col) { return m_data[layer].at(row,col); };

private:
	VolumeType m_data;
	unsigned int m_layers = 0;
	unsigned int m_rows = 0;
	unsigned int m_columns = 0;

	template<typename T> T clamp(T value, T min, T max);
};

// ąęłńś