#include "stdafx.h"

//#include <Windows.h>
#include "Mesh.h"

#include <GL/gl.h>
#include "VolTK.h"
#include "RGBA.h"
//#include "Model3D.h"
#include "MainApplication.h"

#include "../renderers/IVolTKRenderer.h"
#include "StatusBarManager.h"

CVolTK::CVolTK(std::shared_ptr<CBaseObject> p, int imageWidth, int imageHeight, int numberOfImages, int depth):CObject(p)
{
	m_dataIntensityRange = { 0xFFFF, 0x0000 };
	m_currentIntensityRange = { 0xFFFF, 0x0000 };

	
	kostka.init(imageWidth, imageHeight, numberOfImages);

	m_Min.Set(0,0,0);
	m_Max.Set(imageWidth - 1,imageHeight - 1,numberOfImages - 1);

	//m_b.Set(0, 0, 0);
	//m_e.Set(imageWidth, imageHeight, numberOfImages);

	m_displayData.resize(0x10000);
	for (auto& dp : m_displayData) dp = new DisplayDataPart;

	m_alpha = 0.0f;

	m_imDepth = depth;

	m_div = 1;

	//m_voxelSize = CPoint3d(1.0, 1.0, 1.0);

	renderer_ = std::make_shared<IVolTKRenderer>();
}

/*

CVolTK::CVolTK(const CVolTK& src) :CObject(src)
{
	kostka = src.kostka;

	m_displayData.resize(0x10000);

	for (int i=0; i<0x10000; i++)
	{
		m_displayData[i] = new DisplayDataPart();
		std::copy(
			src.m_displayData[i]->begin(),
			src.m_displayData[i]->end(),
			back_inserter(*m_displayData[i]));
	}

	colorFilters = src.colorFilters;

	m_dataIntensityRange = src.m_dataIntensityRange;
	m_currentIntensityRange = src.m_currentIntensityRange;

	m_Min = src.m_Min;
	m_Max = src.m_Max;

	m_alpha = src.m_alpha;

	m_imDepth = src.m_imDepth;

	m_div = src.m_div;

	m_voxelSize = src.m_voxelSize;

	setGeometry();
}

CVolTK::CVolTK(const CVolTK& src, CPoint3s lowerBound, CPoint3s upperBound ) :CObject(src)
{
	kostka = src.kostka;

	m_displayData.resize(0x10000);

	for (int i = 0; i < 0x10000; i++)
	{
		m_displayData[i] = new DisplayDataPart();
		std::copy_if(
			src.m_displayData[i]->begin(),
			src.m_displayData[i]->end(),
			back_inserter(*m_displayData[i]),
			[lowerBound, upperBound](CPoint3s d) {
				return ((lowerBound.x <= d.x) && (lowerBound.y <= d.y) && (lowerBound.z <= d.z) && (d.x < upperBound.x) && (d.y < upperBound.y) && (d.z < upperBound.z));
			});
	}

	colorFilters = src.colorFilters;

	m_dataIntensityRange = src.m_dataIntensityRange;

	m_currentIntensityRange = src.m_currentIntensityRange;

	m_Min = lowerBound;
	m_Max = upperBound - CPoint3s(1, 1, 1);

	m_alpha = src.m_alpha;

	m_imDepth = src.m_imDepth;

	m_div = src.m_div;

	m_voxelSize = src.m_voxelSize;

	setGeometry();
}

CVolTK::CVolTK(const CVolTK& src, uint16_t lowerBound, uint16_t upperBound)
{
	kostka = src.kostka;

	m_displayData.resize(0x10000);
	for (auto& dp : m_displayData) dp = new DisplayDataPart;

	for (auto itz = lowerBound; itz < upperBound; itz++)
	{
		m_displayData[itz] = new DisplayDataPart;
		std::copy(
			src.m_displayData[itz]->begin(),
			src.m_displayData[itz]->end(),
			back_inserter(*m_displayData[itz]));
	}

	colorFilters = src.colorFilters;

	m_currentIntensityRange.upper = m_dataIntensityRange.upper = 0xffff;// m_data.crbegin()->first;
	m_currentIntensityRange.lower = m_dataIntensityRange.lower = 0;// m_data.cbegin()->first;


	m_Min = src.m_Min;
	m_Max = src.m_Max;

	m_alpha = src.m_alpha;

	m_imDepth = src.m_imDepth;

	m_div = src.m_div;

	m_voxelSize = src.m_voxelSize;

	setGeometry();
}
*/

CVolTK::CVolTK(const CVolTK& src, CPoint3s mB, CPoint3s mE, uint16_t lowerC, uint16_t upperC)
{
	kostka = src.kostka;

	m_displayData.resize(0x10000);
	for (auto& dp : m_displayData) dp = new DisplayDataPart;

	for (auto itz = lowerC; itz < upperC; itz++)
	{
		m_displayData[itz] = new DisplayDataPart;
		std::copy_if(
			src.m_displayData[itz]->begin(),
			src.m_displayData[itz]->end(),
			back_inserter(*m_displayData[itz]),
			[mB, mE](CPoint3s d) {
				return ((mB.x <= d.x) && (mB.y <= d.y) && (mB.z <= d.z) && (d.x < mE.x) && (d.y < mE.y) && (d.z < mE.z));
			});
	}

	colorFilters = src.colorFilters;

	m_currentIntensityRange.upper = m_dataIntensityRange.upper = 0xffff;// m_data.crbegin()->first;
	m_currentIntensityRange.lower = m_dataIntensityRange.lower = 0;// m_data.cbegin()->first;

	m_Min = mB;
	m_Max = mE - CPoint3s(1, 1, 1);

	m_alpha = src.m_alpha;

	m_imDepth = src.m_imDepth;

	m_div = src.m_div;

	//m_voxelSize = src.m_voxelSize;

	setLabel("copy_of_" + src.m_label);

	setGeometry();

	renderer_ = std::make_shared<IVolTKRenderer>();
}

CVolTK::~CVolTK(void)
{
	clear();
}



std::shared_ptr<CBaseObject> CVolTK::getCopy()
{
	return std::make_shared<CVolTK>(*this, m_b, m_e, m_currentIntensityRange.lower, m_currentIntensityRange.upper+1);
}

CVolTK* CVolTK::getPart(CPoint3s min, CPoint3s max)
{
	return nullptr;
}

CVolTK* CVolTK::getPart(uint16_t wMin, uint16_t wMax)
{
	return nullptr;
}

void CVolTK::clear()
{
	for (DisplayData::iterator itz = m_displayData.begin(); itz != m_displayData.end(); itz++)
	{
		delete (*itz);
	}

	m_dataIntensityRange.upper = 0x0000;
	m_dataIntensityRange.lower = 0xFFFF;
}

std::shared_ptr<CVolTK> CVolTK::create(int cols, int rows, int layers, int value, int depth)
{
	auto volTK = std::make_shared<CVolTK>(nullptr, cols, rows, layers, depth);

	for (int layer = 0; layer < layers; layer++)
		for (int row = 0; row < rows; row++)
			for (int col = 0; col < cols; col++)
				volTK->kostka.set(col, row, layer, value);
	
	volTK->createDisplayData();
	volTK->setGeometry();

	return volTK;
}


std::wstring CVolTK::infoRow()
{
	std::wstring info = L"Volumetric TK\n";
	info += L"data Min: " + std::to_wstring(m_dataIntensityRange.lower) + L"\n";
	info += L"data Max: " + std::to_wstring(m_dataIntensityRange.upper) + L"\n";
	info += L"window: " + std::to_wstring(m_currentIntensityRange.lower) + L" - " + std::to_wstring(m_currentIntensityRange.upper) + L"\n";

	info += L"range:\nx[" + std::to_wstring(m_Min.x) + L"-" + std::to_wstring(m_Max.x) + L"]\ny[" + std::to_wstring(m_Min.y) + L"-" + std::to_wstring(m_Max.y) + L"]\nz[" + std::to_wstring(m_Min.z) + L"-" + std::to_wstring(m_Max.z) + L"]\n\n";

		info += L"voxelSize: " + std::to_wstring(kostka.m_voxelSize.x) + L", " + std::to_wstring(kostka.m_voxelSize.y) + L", " + std::to_wstring(kostka.m_voxelSize.z) + L"\n";
	info += L"gantra: " + std::to_wstring(m_alpha) + L"\n";

	return info;
}

// #ifdef _WIN32
// #define PACKET_SIZE 100000ULL
// #else
// #define PACKET_SIZE 100000UL
// #endif

//#include <qglshaderprogram.h>
#include "AP.h"
//#include "MainWindow.h"
//#include "GLViewer.h"


// bool CVolTK::addShadersBAK(QOpenGLShaderProgram& program)
// {
// 	const char* defaultVertexShader = R"(
// #version 330 compatibility
// in vec3 vertex;
// uniform mediump vec4 color;
// uniform mediump vec3 b;
// uniform mediump vec3 e;
// uniform mediump vec3 vScale;
// uniform mediump float sinA;
// uniform mediump float cosA;

// out VS_OUT{
// 	vec4 c;
// 	bool b;
// } vout;

// void main(void)
// {
//   gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex.x, vertex.y, vertex.z, 1.0);
//   vout.c = color;
//   vout.b = ((vertex.x >= b.x)&&(vertex.x < e.x)&&(vertex.y >= b.y)&&(vertex.y < e.y)&&(vertex.z >= b.z)&&(vertex.z < e.z));
// }
// )";

// 	const char* defaultGeometryShader = R"(
// #version 330 core
// layout(points) in;
// layout(points, max_vertices = 1) out;

// in VS_OUT{
// 	vec4 c;
// 	bool b;
// } gs_in[];

// out vec4 fColor;

// void main(void)
// {
//   if (gs_in[0].b)
//   {
//     fColor = gs_in[0].c;
//     gl_Position = gl_in[0].gl_Position;
//     EmitVertex();
//     EndPrimitive();
//   }
// }
// )";

// 	const char* defaultFragmentShader = R"(
// #version 330
// in vec4 fColor;
// void main(void)
// {
//    gl_FragColor = fColor;
// }
// )";




// 		program.addShaderFromSourceCode(QOpenGLShader::Vertex, defaultVertexShader);
// 		program.addShaderFromSourceCode(QOpenGLShader::Geometry, defaultGeometryShader);
// 		program.addShaderFromSourceCode(QOpenGLShader::Fragment, defaultFragmentShader);

// 	return true;
// }


// bool CVolTK::addShaders(QOpenGLShaderProgram& program)
// {
// 	const char* defaultVertexShader = R"(
// #version 330 compatibility
// in vec3 vertex;
// uniform mediump vec4 color;
// uniform mediump vec3 b;
// uniform mediump vec3 e;
// uniform mediump vec3 vScale;
// uniform mediump float sinA;
// uniform mediump float cosA;

// out VS_OUT{
// 	vec4 c;
// 	bool b;
// } vout;

// void main(void)
// {
//   gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex.x*vScale.x, vertex.y*vScale.y - vertex.z*vScale.z*sinA/cosA, vertex.z*vScale.z, 1.0);
//   vout.c = color;
//   vout.b = ((vertex.x >= b.x)&&(vertex.x < e.x)&&(vertex.y >= b.y)&&(vertex.y < e.y)&&(vertex.z >= b.z)&&(vertex.z < e.z));
// }
// )";

// 	const char* defaultGeometryShader = R"(
// #version 330 core
// layout(points) in;
// layout(points, max_vertices = 1) out;

// in VS_OUT{
// 	vec4 c;
// 	bool b;
// } gs_in[];

// out vec4 fColor;

// void main(void)
// {
//   if (gs_in[0].b)
//   {
//     fColor = gs_in[0].c;
//     gl_Position = gl_in[0].gl_Position;
//     EmitVertex();
//     EndPrimitive();
//   }
// }
// )";

// 	const char* defaultFragmentShader = R"(
// #version 330
// in vec4 fColor;
// void main(void)
// {
//    gl_FragColor = fColor;
// }
// )";




// 	//	if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, AP::mainApp().appExecDir() + "/shaders/VolTK.vert"))
// 	//	{
// 	program.addShaderFromSourceCode(QOpenGLShader::Vertex, defaultVertexShader);
// 	//	}

// 	//	if (!program.addShaderFromSourceFile(QOpenGLShader::Geometry, AP::mainApp().appExecDir() + "/shaders/VolTK.gs"))
// 	//	{
// 	program.addShaderFromSourceCode(QOpenGLShader::Geometry, defaultGeometryShader);
// 	//	}

// 	//	if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, AP::mainApp().appExecDir() + "/shaders/VolTK.frag"))
// 	//	{
// 	program.addShaderFromSourceCode(QOpenGLShader::Fragment, defaultFragmentShader);
// 	//	}

// 	return true;
// }


float fixVal(float val)
{
	float k = 3.0;  // wsp�czynnik nachylenia krzywej
	float x_0 = 0.3;  // x dla kt�rego funkcja osi�ga 0.5

	float c = 1.0f / (1.0f + exp(-k * (val * 10.0f - x_0)));

	return c;
}

// void CVolTK::renderPart(QOpenGLShaderProgram& program, uint16_t min, uint16_t max, int fR, int fG, int fB)
// {
// 	for (int it = min; it < max; it++)
// 	{
// 		float c = float(it - min) / float(max - min);

// 		//float c = fixVal( ic );

// 		int r = c * fR;
// 		int g = c * fG;
// 		int b = c * fB;

// 		program.setUniformValue(colorLocation, QColor(r, g, b));

// 		DisplayDataPart& vertices = *m_displayData[it];

// 		if (!vertices.empty())
// 		{
// 			size_t idx = 0;

// 			functions->glVertexAttribPointer(vertexLocation, 3, GL_SHORT, GL_FALSE, 0, vertices.data());

// 			while (idx < vertices.size())
// 			{
// 				functions->glDrawArrays(GL_POINTS, idx, std::min(PACKET_SIZE, vertices.size() - idx));

// 				idx += PACKET_SIZE;
// 			}
// 		}
// 	}
// }

// void CVolTK::renderPart(QOpenGLShaderProgram &program, uint16_t min, uint16_t max, QColor col)
// {
// 	for (int it = min; it < max; it++)
// 	{
// 		float c = float(it - min) / float(max - min);

// 		int r = col.red() * c;
// 		int g = col.green() * c;
// 		int b = col.blue() * c;

// 		program.setUniformValue(colorLocation, QColor(r, g, b));

// 		DisplayDataPart& vertices = *m_displayData[it];

// 		if (!vertices.empty())
// 		{
// 			size_t idx = 0;

// 			functions->glVertexAttribPointer(vertexLocation, 3, GL_SHORT, GL_FALSE, 0, vertices.data());

// 			while (idx < vertices.size())
// 			{
// 				functions->glDrawArrays(GL_POINTS, idx, std::min(PACKET_SIZE, vertices.size() - idx));

// 				idx += PACKET_SIZE;
// 			}
// 		}
// 	}

// }

// void CVolTK::renderPart2(QOpenGLShaderProgram& program, uint16_t min, uint16_t max)
// {
// 	for (int cc = min; cc <= max; cc++)
// 	{
// 		float c = 1.0f;

// 		if (max>min)
// 			c = float(cc - min) / float(max - min);

// 		//c = fixVal(c);

// 		program.setUniformValue(colorLocation, QColor(c*255, c * 255, c * 255));

// 		DisplayDataPart& vertices = *m_displayData[cc];

// 		if (!vertices.empty())
// 		{
// 			size_t idx = 0;

// 			functions->glVertexAttribPointer(vertexLocation, 3, GL_SHORT, GL_FALSE, 0, vertices.data());

// 			while (idx < vertices.size())
// 			{
// 				functions->glDrawArrays(GL_POINTS, idx, std::min(PACKET_SIZE, vertices.size() - idx));

// 				idx += PACKET_SIZE;
// 			}
// 		}
// 	}
// }

void CVolTK::renderSelf()
{
// 	context = QOpenGLContext::currentContext();
// 	functions = context->functions();
	
// 	functions->glEnable(GL_PROGRAM_POINT_SIZE);

// 	//glPointSize(3);

// 	QOpenGLShaderProgram program;

// 	addShaders(program);

// 	program.link();
// 	program.bind();

// 	vertexLocation = program.attributeLocation("vertex");
// 	colorLocation = program.uniformLocation("color");

// 	int bLocation = program.uniformLocation("b");
// 	int eLocation = program.uniformLocation("e");
// 	int vScaleLocation = program.uniformLocation("vScale");
// 	int sinAlfaLocation = program.uniformLocation("sinA");
// 	int cosAlfaLocation = program.uniformLocation("cosA");

// 	program.enableAttributeArray(vertexLocation);

// 	uint16_t wB = m_currentIntensityRange.lower;
// 	uint16_t wE;

// 	program.setUniformValue(bLocation, m_b.x, m_b.y, m_b.z);
// 	program.setUniformValue(eLocation, m_e.x, m_e.y, m_e.z);
// 	program.setUniformValue(vScaleLocation, kostka.m_voxelSize.x, kostka.m_voxelSize.y, kostka.m_voxelSize.z);
// 	program.setUniformValue(sinAlfaLocation, sin((float)deg2rad(m_alpha)));
// 	program.setUniformValue(cosAlfaLocation, cos((float)deg2rad(m_alpha)));

// 	for (ColorFilter f : colorFilters)
// 	{
// 		wE = f.first;
// 		if (wE > wB)
// 		{
// 			if (f.second.first)
// 			{
// 				CRGBA c = f.second.second;
// 				renderPart(program, wB, wE, c.red(), c.green(), c.blue());
// 			}
// 			wB = wE;
// 		}
// 	}

// 	wE = m_currentIntensityRange.upper;
// 	if (wE >= wB)
// 	{
// 		renderPart2(program, wB, wE);
// 		//renderPart(program, wB, wE, QColor(255,255,255,128));
// 	}

// 	program.disableAttributeArray(vertexLocation);

// 	program.release();

// 	functions->glDisable(GL_PROGRAM_POINT_SIZE);
}


//void CVolTK::copyOneKostkaLayer(int16_t layer, int pixelRepresentation)
//{
//	for (size_t y = 0; y < kostka.m_rows; y++)
//	{
//		for (size_t x = 0; x < kostka.m_cols; x++)
//		{
//			if (m_imDepth == 16)
//			{
//				uint16_t pix = kostka.get(x+kostka.m_minC,y+kostka.m_minR, layer);
//				addVoxelW(x, y, layer, pix);
//
//			}
//			else if (m_imDepth == 8)
//			{
//				uint8_t pix = kostka.get(x + kostka.m_minC, y + kostka.m_minR, layer);
//				addVoxelB(x, y, layer, pix);
//			}
//
//		}
//	}
//}

void CVolTK::createDisplayData()
{
	for (int i = 0; i < 0x10000; i++)
	{
		m_displayData[i]->clear();
	}

	m_dataIntensityRange = { 0xFFFF, 0x0000 };
	m_currentIntensityRange = { 0xFFFF, 0x0000 };

	UI::PROGRESSBAR::init(0, kostka.m_lays, 0);
	UI::PROGRESSBAR::setText("Kopiuje dane: ");
	for (int z = 0; z < kostka.m_lays; z++)
	{
		UI::PROGRESSBAR::setValue(z);
		int zz = z;//  + kostka.m_minL;
		for (int y = 0; y < kostka.m_rows; y++)
		{
			int yy = y;// + kostka.m_minR;
			for (int x = 0; x < kostka.m_cols; x++)
			{
				int xx = x;// + kostka .m_minC;
				if (depth() == 16)
				{
					//uint16_t pix = kostka.getRescaled(xx, yy, zz);
					uint16_t pix = kostka.get(xx, yy, zz);
					addVoxelW(xx, yy, zz, pix);
				}
				else if (depth() == 8)
				{
					//uint8_t pix = kostka.getRescaled(xx, yy, zz);
					uint8_t pix = kostka.get(xx, yy, zz);
					addVoxelB(x, z, y, pix);
				}

			}
		}
	}
	UI::PROGRESSBAR::hide();

	m_currentIntensityRange = m_dataIntensityRange;
}


void CVolTK::addVoxelW(int16_t x, int16_t y, int16_t z, uint16_t c)
{
	m_displayData[c]->push_back(CPoint3s(x, y, z));

	if (c < m_dataIntensityRange.lower) m_dataIntensityRange.lower = c;

	if (c > m_dataIntensityRange.upper) m_dataIntensityRange.upper = c;


	if (m_Min.x > x) m_Min.x = x;
	if (m_Min.y > y) m_Min.y = y;
	if (m_Min.z > z) m_Min.z = z;

	if (m_Max.x < x) m_Max.x = x;
	if (m_Max.y < y) m_Max.y = y;
	if (m_Max.z < z) m_Max.z = z;
}

size_t CVolTK::addLayerW( int layerId, std::vector<WORD>* layer, uint16_t lowTreshold, uint16_t upTreshold)
{
	int lbv = 0;
	for (int y = 0; y < rows(); y++)
	{
		for (int x = 0; x < cols(); x++)
		{
			uint16_t mask = layer->at(x + y * cols());

			int xx = x - cols() / 2;
			int yy = y - rows() / 2;
			int zz = layerId - lays() / 2;

			if ((mask > lowTreshold) && (mask <= upTreshold))
			{
				addVoxelW(-xx, zz, -yy, mask);
				lbv++;
			}
		}
	}

	return lbv;
}



void CVolTK::addVoxelB(int16_t x, int16_t y, int16_t z, BYTE c)
{
	m_displayData[c]->push_back(CPoint3s(x, y, z));

	if (m_dataIntensityRange.lower > c) m_dataIntensityRange.lower = c;
	if (m_dataIntensityRange.upper < c) m_dataIntensityRange.upper = c;

	if (m_Min.x > x) m_Min.x = x;
	if (m_Min.y > y) m_Min.y = y;
	if (m_Min.z > z) m_Min.z = z;

	if (m_Max.x < x) m_Max.x = x;
	if (m_Max.y < y) m_Max.y = y;
	if (m_Max.z < z) m_Max.z = z;
}

size_t CVolTK::addLayerB(int layerId, std::vector<BYTE>* layer, BYTE lowTreshold, BYTE upTreshold)
{
	int lbv = 0;
	for (int y = 0; y < rows(); y++)
	{
		for (int x = 0; x < cols(); x++)
		{
			BYTE mask = layer->at(x + y * cols());

			int xx = x - cols() / 2;
			int yy = y - rows() / 2;
			int zz = layerId - lays() / 2;

			if ((mask > lowTreshold) && (mask <= upTreshold))
			{
				addVoxelB(-xx, zz, -yy, mask);
				lbv++;
			}
		}
	}

	return lbv;
}

typedef std::pair<INDEX_TYPE, INDEX_TYPE> Index;
typedef std::set<INDEX_TYPE> Faces;
typedef std::map<Index, Faces> E2Fmap;

void createE2Fmap(CMesh* mesh, E2Fmap& allEdges)
{
	for (unsigned int j = 0; j < mesh->faces().size(); j++)
	{
		CFace f = mesh->faces()[j];

		allEdges[Index(f.A(), f.B())].insert(j);
		allEdges[Index(f.B(), f.C())].insert(j);
		allEdges[Index(f.C(), f.A())].insert(j);
	}
}


void tesst(CMesh *mesh)
{
	E2Fmap allEdges;

	createE2Fmap(mesh, allEdges);

	Faces facesToRemove;

	for (const E2Fmap::value_type e : allEdges)
	{
		if (e.second.size() == 2)
		{
			Faces::iterator i0 = e.second.begin();
			CFace& f01 = mesh->faces()[*i0];
			i0++;
			CFace& f02 = mesh->faces()[*i0];

			E2Fmap::iterator it = allEdges.find(Index(e.first.second, e.first.first));
			
			if (it != allEdges.end())
			{
				Faces::iterator i1 = it->second.begin();
				CFace& f11 = mesh->faces()[*i1];
				i1++;
				CFace& f12 = mesh->faces()[*i1];

				INDEX_TYPE B = e.first.first;
				INDEX_TYPE C = e.first.second;

				INDEX_TYPE A;

				if ((f01.A() != B) && (f01.A() != C)) A = f01.A();
				else if ((f01.B() != B) && (f01.B() != C)) A = f01.B();
				else if ((f01.C() != B) && (f01.C() != C)) A = f01.C();

				//CVector3f v0 = CVector3f(mesh->vertices()[B], mesh->vertices()[A]).getNormalized();

				//if (CVector3f(mesh->vertices()[B], mesh->vertices()[C]).getNormalized().dotProduct(v0) != 0)
				//{
				//	v0 = CVector3f(mesh->vertices()[C], mesh->vertices()[A]).getNormalized();
				//}


				INDEX_TYPE D;

				if ((f11.A() != B) && (f11.A() != C)) D = f11.A();
				else if ((f11.B() != B) && (f11.B() != C)) D = f11.B();
				else if ((f11.C() != B) && (f11.C() != C)) D = f11.C();

				CVector3f v1 = CVector3f(mesh->vertices()[B], mesh->vertices()[D]).getNormalized();

				if (CVector3f(mesh->vertices()[C], mesh->vertices()[B]).getNormalized().dotProduct(v1) != 0)
				{
					v1 = CVector3f(mesh->vertices()[C], mesh->vertices()[D]).getNormalized();
				}


				//CVector3f vSum = ( v0 + v1 ).getNormalized();
				
				double dot0111 = f01.getNormal(mesh->vertices()).dotProduct(v1);


				if (dot0111 > 0.9)
				{
					INDEX_TYPE D;

					if ((f11.A() != B) && (f11.A() != C)) D = f11.A();
					else if ((f11.B() != B) && (f11.B() != C)) D = f11.B();
					else if ((f11.C() != B) && (f11.C() != C)) D = f11.C();

					f01.Set(A, B, D);
					f11.Set(A, D, C);

					//INDEX_TYPE E;

					//if ((f12.A() != B) && (f12.A() != C)) E = f12.A();
					//else if ((f12.B() != B) && (f12.B() != C)) E = f12.B();
					//else if ((f12.C() != B) && (f12.C() != C)) E = f12.C();

					//INDEX_TYPE F;

					//if ((f02.A() != B) && (f02.A() != C)) F = f02.A();
					//else if ((f02.B() != B) && (f02.B() != C)) F = f02.B();
					//else if ((f02.C() != B) && (f02.C() != C)) F = f02.C();

					//f12.Set(E, C, F);
					//f02.Set(E, F, B);
				}
				else
				{
					INDEX_TYPE D;

					if ((f12.A() != B) && (f12.A() != C)) D = f12.A();
					else if ((f12.B() != B) && (f12.B() != C)) D = f12.B();
					else if ((f12.C() != B) && (f12.C() != C)) D = f12.C();

					f01.Set(A, B, D);
					f12.Set(A, D, C);

					//INDEX_TYPE E;

					//if ((f11.A() != B) && (f11.A() != C)) E = f11.A();
					//else if ((f11.B() != B) && (f11.B() != C)) E = f11.B();
					//else if ((f11.C() != B) && (f11.C() != C)) E = f11.C();

					//INDEX_TYPE F;

					//if ((f02.A() != B) && (f02.A() != C)) F = f02.A();
					//else if ((f02.B() != B) && (f02.B() != C)) F = f02.B();
					//else if ((f02.C() != B) && (f02.C() != C)) F = f02.C();

					//f11.Set(E, C, F);
					//f02.Set(E, F, B);
				}
			}

		}
	}

}

void dodajKostke2(CMesh* mesh, CPoint3s voxel, CRGBA color, int16_t div_value, std::set<CPoint3s>& sasiedztwo)
{
	bool s[3][3][3];

	int16_t v[] = { static_cast<int16_t>(-div_value), 0, div_value };

	for (int16_t z = 0; z < 3; z++ )
		for (int16_t y = 0; y < 3; y++ )
			for (int16_t x = 0; x < 3; x++)
			{
				s[x][y][z] = sasiedztwo.find(voxel + CPoint3s(v[x], v[y], v[z])) != sasiedztwo.end();
			}


}

void dodajKostke(CMesh* mesh, CPoint3s voxel, CRGBA color, int16_t div, std::set<CPoint3s>& sasiedztwo)
{
	bool b000 = false;
	bool b010 = false;
	bool b110 = false;
	bool b100 = false;
	bool b001 = false;
	bool b011 = false;
	bool b111 = false;
	bool b101 = false;


	//============================= SCIANKI

	bool fmX = sasiedztwo.find(voxel + CPoint3s(-div, 0, 0)) == sasiedztwo.end();
	bool fmY = sasiedztwo.find(voxel + CPoint3s(0, -div, 0)) == sasiedztwo.end();
	bool fmZ = sasiedztwo.find(voxel + CPoint3s(0, 0, -div)) == sasiedztwo.end();

	bool fpX = sasiedztwo.find(voxel + CPoint3s(div, 0, 0)) == sasiedztwo.end();
	bool fpY = sasiedztwo.find(voxel + CPoint3s(0, div, 0)) == sasiedztwo.end();
	bool fpZ = sasiedztwo.find(voxel + CPoint3s(0, 0, div)) == sasiedztwo.end();

	if (fmZ)
	{
		b000 = b010 = b110 = b100 = true;
	}

	if (fpZ)
	{
		b001 = b111 = b011 = b101 = true;
	}

	if (fmY)
	{
		b000 = b001 = b101 = b100 = true;
	}

	if (fpY)
	{
		b010 = b111 = b011 = b110 = true;
	}

	if (fmX)
	{
		b000 = b001 = b011 = b010 = true;
	}

	if (fpX)
	{
		b100 = b111 = b101 = b110 = true;
	}


	float dMz = 0;
	float dPz = div;
	float dMy = 0;
	float dPy = div;
	float dMx = 0;
	float dPx = div;

	size_t p000 = b000 ? mesh->addVertex(voxel + CPoint3s(dMx, dMy, dMz), color) : 0;
	size_t p010 = b010 ? mesh->addVertex(voxel + CPoint3s(dMx, dPy, dMz), color) : 0;
	size_t p110 = b110 ? mesh->addVertex(voxel + CPoint3s(dPx, dPy, dMz), color) : 0;
	size_t p100 = b100 ? mesh->addVertex(voxel + CPoint3s(dPx, dMy, dMz), color) : 0;

	size_t p001 = b001 ? mesh->addVertex(voxel + CPoint3s(dMx, dMy, dPz), color) : 0;
	size_t p011 = b011 ? mesh->addVertex(voxel + CPoint3s(dMx, dPy, dPz), color) : 0;
	size_t p111 = b111 ? mesh->addVertex(voxel + CPoint3s(dPx, dPy, dPz), color) : 0;
	size_t p101 = b101 ? mesh->addVertex(voxel + CPoint3s(dPx, dMy, dPz), color) : 0;

	CMesh::Faces& faces = mesh->faces();

	if (fmZ)
	{
		faces.push_back(CFace(p000, p010, p110));
		faces.push_back(CFace(p000, p110, p100));
	}

	if (fpZ)
	{
		faces.push_back(CFace(p001, p111, p011));
		faces.push_back(CFace(p001, p101, p111));
	}

	if (fmY)
	{
		faces.push_back(CFace(p000, p001, p101));
		faces.push_back(CFace(p000, p101, p100));
	}

	if (fpY)
	{
		faces.push_back(CFace(p010, p111, p011));
		faces.push_back(CFace(p010, p110, p111));
	}

	if (fmX)
	{
		faces.push_back(CFace(p000, p001, p011));
		faces.push_back(CFace(p000, p011, p010));
	}

	if (fpX)
	{
		faces.push_back(CFace(p100, p111, p101));
		faces.push_back(CFace(p100, p110, p111));
	}
}


void dodajKostkeX(CMesh* mesh, CPoint3s voxel, CRGBA color, int16_t div, std::set<CPoint3s>& sasiedztwo)
{
	bool b000 = false;
	bool b010 = false;
	bool b110 = false;
	bool b100 = false;
	bool b001 = false;
	bool b011 = false;
	bool b111 = false;
	bool b101 = false;

	bool bb000 = false;
	bool bb010 = false;
	bool bb110 = false;
	bool bb100 = false;
	bool bb001 = false;
	bool bb011 = false;
	bool bb111 = false;
	bool bb101 = false;


	float ddMz = 0.25f * div;
	float ddPz = 0.75f * div;
	float ddMy = 0.25f * div;
	float ddPy = 0.75f * div;
	float ddMx = 0.25f * div;
	float ddPx = 0.75f * div;

	float dMz = 0.0f;
	float dPz = (float)div;
	float dMy = 0.0f;
	float dPy = (float)div;
	float dMx = 0.0f;
	float dPx = (float)div;


	//============================= SCIANKI

	bool bMx = sasiedztwo.find(voxel + CPoint3s(-div,    0,    0)) != sasiedztwo.end();
	bool bMy = sasiedztwo.find(voxel + CPoint3s(   0, -div,    0)) != sasiedztwo.end();
	bool bMz = sasiedztwo.find(voxel + CPoint3s(   0,    0, -div)) != sasiedztwo.end();

	bool bPx = sasiedztwo.find(voxel + CPoint3s( div,    0,    0)) != sasiedztwo.end();
	bool bPy = sasiedztwo.find(voxel + CPoint3s(   0,  div,    0)) != sasiedztwo.end();
	bool bPz = sasiedztwo.find(voxel + CPoint3s(   0,    0,  div)) != sasiedztwo.end();

	//============================= KRAWEDZIE

	//bool bMxMy = sasiedztwo.find(voxel + CPoint3s(-div, -div, 0)) != sasiedztwo.end();
	//bool bMxPy = sasiedztwo.find(voxel + CPoint3s(-div,  div, 0)) != sasiedztwo.end();
	//bool bPxPy = sasiedztwo.find(voxel + CPoint3s( div,  div, 0)) != sasiedztwo.end();
	//bool bPxMy = sasiedztwo.find(voxel + CPoint3s( div, -div, 0)) != sasiedztwo.end();

	//bool bMxMz = sasiedztwo.find(voxel + CPoint3s(-div, 0, -div)) != sasiedztwo.end();
	//bool bMxPz = sasiedztwo.find(voxel + CPoint3s(-div, 0,  div)) != sasiedztwo.end();
	//bool bPxPz = sasiedztwo.find(voxel + CPoint3s( div, 0,  div)) != sasiedztwo.end();
	//bool bPxMz = sasiedztwo.find(voxel + CPoint3s( div, 0, -div)) != sasiedztwo.end();

	//bool bMyMz = sasiedztwo.find(voxel + CPoint3s(0, -div, -div)) != sasiedztwo.end();
	//bool bMyPz = sasiedztwo.find(voxel + CPoint3s(0, -div,  div)) != sasiedztwo.end();
	//bool bPyPz = sasiedztwo.find(voxel + CPoint3s(0,  div,  div)) != sasiedztwo.end();
	//bool bPyMz = sasiedztwo.find(voxel + CPoint3s(0,  div, -div)) != sasiedztwo.end();

	//============================== NAROZNIKI

	//bool bMxMyMz = sasiedztwo.find(voxel + CPoint3s(-div, -div, -div)) != sasiedztwo.end();
	//bool bMxPyMz = sasiedztwo.find(voxel + CPoint3s(-div,  div, -div)) != sasiedztwo.end();
	//bool bPxPyMz = sasiedztwo.find(voxel + CPoint3s( div,  div, -div)) != sasiedztwo.end();
	//bool bPxMyMz = sasiedztwo.find(voxel + CPoint3s( div, -div, -div)) != sasiedztwo.end();

	//bool bMxMyPz = sasiedztwo.find(voxel + CPoint3s(-div, -div,  div)) != sasiedztwo.end();
	//bool bMxPyPz = sasiedztwo.find(voxel + CPoint3s(-div,  div,  div)) != sasiedztwo.end();
	//bool bPxPyPz = sasiedztwo.find(voxel + CPoint3s( div,  div,  div)) != sasiedztwo.end();
	//bool bPxMyPz = sasiedztwo.find(voxel + CPoint3s( div, -div,  div)) != sasiedztwo.end();

	//==============================


	if ( !bMz )
	{
		if (!bMx && !bMy)
			bb000 = true;
		else
			b000 = true;

		if (!bMx && !bPy)
			bb010 = true;
		else
			b010 = true;

		if (!bPx && !bPy)
			bb110 = true;
		else
			b110 = true;

		if (!bPx && !bMy)
			bb100 = true;
		else
			b100 = true;
	}


	if (!bPz)
	{
		if (!bMx && !bMy)
			bb001 = true;
		else
			b001 = true;

		if (!bMx && !bPy)
			bb011 = true;
		else
			b011 = true;

		if (!bPx && !bPy)
			bb111 = true;
		else
			b111 = true;

		if (!bPx && !bMy)
			bb101 = true;
		else
			b101 = true;

	}

	if (!bMy)
	{
		if (!bMx && !bPz)
			bb001 = true;
		else
			b001 = true;

		if (!bPz && !bPx)
			bb101 = true;
		else
			b101 = true;

		if (!bPx && !bMz)
			bb100 = true;
		else
			b100 = true;

		if (!bMx && !bMz)
			bb000 = true;
		else
			b000 = true;
	}

	if (!bPy)
	{
		if (!bMx && !bPz)
			bb011 = true;
		else
			b011 = true;

		if (!bPz && !bPx)
			bb111 = true;
		else
			b111 = true;

		if (!bPx && !bMz)
			bb110 = true;
		else
			b110 = true;

		if (!bMx && !bMz)
			bb010 = true;
		else
			b010 = true;
	}

	if (!bMx)
	{
		if (!bMy && !bMz)
			bb000 = true;
		else
			b000 = true;

		if (!bMy && !bPz)
			bb001 = true;
		else
			b001 = true;

		if (!bPy && !bPz)
			bb011 = true;
		else
			b011 = true;

		if (!bPy && !bMz)
			bb010 = true;
		else
			b010 = true;
	}

	if (!bPx)
	{
		if (!bMy && !bMz)
			bb100 = true;
		else
			b100 = true;

		if (!bMy && !bPz)
			bb101 = true;
		else
			b101 = true;

		if (!bPy && !bPz)
			bb111 = true;
		else
			b111 = true;

		if (!bPy && !bMz)
			bb110 = true;
		else
			b110 = true;
	}

	//==============================

	CPoint3f v(voxel);

	size_t p000 = bb000 ? mesh->addVertex(v + CPoint3f(ddMx, ddMy, ddMz), color) : b000 ? mesh->addVertex(v + CPoint3f(dMx, dMy, dMz), color) : 0;
	size_t p010 = bb010 ? mesh->addVertex(v + CPoint3f(ddMx, ddPy, ddMz), color) : b010 ? mesh->addVertex(v + CPoint3f(dMx, dPy, dMz), color) : 0;
	size_t p110 = bb110 ? mesh->addVertex(v + CPoint3f(ddPx, ddPy, ddMz), color) : b110 ? mesh->addVertex(v + CPoint3f(dPx, dPy, dMz), color) : 0;
	size_t p100 = bb100 ? mesh->addVertex(v + CPoint3f(ddPx, ddMy, ddMz), color) : b100 ? mesh->addVertex(v + CPoint3f(dPx, dMy, dMz), color) : 0;

	size_t p001 = bb001 ? mesh->addVertex(v + CPoint3f(ddMx, ddMy, ddPz), color) : b001 ? mesh->addVertex(v + CPoint3f(dMx, dMy, dPz), color) : 0;
	size_t p011 = bb011 ? mesh->addVertex(v + CPoint3f(ddMx, ddPy, ddPz), color) : b011 ? mesh->addVertex(v + CPoint3f(dMx, dPy, dPz), color) : 0;
	size_t p111 = bb111 ? mesh->addVertex(v + CPoint3f(ddPx, ddPy, ddPz), color) : b111 ? mesh->addVertex(v + CPoint3f(dPx, dPy, dPz), color) : 0;
	size_t p101 = bb101 ? mesh->addVertex(v + CPoint3f(ddPx, ddMy, ddPz), color) : b101 ? mesh->addVertex(v + CPoint3f(dPx, dMy, dPz), color) : 0;


	//==============================

	CMesh::Faces& faces = mesh->faces();

	if (!bMz)
	{
		faces.push_back(CFace(p100, p010, p110));
		faces.push_back(CFace(p100, p000, p010));
	}

	if (!bPz)
	{
		faces.push_back(CFace(p001, p111, p011));
		faces.push_back(CFace(p001, p101, p111));
	}

	if (!bMy)
	{
		faces.push_back(CFace(p001, p100, p101));
		faces.push_back(CFace(p001, p000, p100));
	}

	if (!bPy)
	{
		faces.push_back(CFace(p111, p010, p011));
		faces.push_back(CFace(p111, p110, p010));
	}

	if (!bMx)
	{
		faces.push_back(CFace(p000, p011, p010));
		faces.push_back(CFace(p000, p001, p011));
	}

	if (!bPx)
	{
		faces.push_back(CFace(p101, p110, p111));
		faces.push_back(CFace(p101, p100, p110));
	}
}


std::shared_ptr<CMesh> CVolTK::createMesh(int winMin, int winMax, CRGBA col)
{
	if (winMin >= winMax) return nullptr;

	if (winMin < m_currentIntensityRange.lower) winMin = m_currentIntensityRange.lower;
	if (winMax > m_currentIntensityRange.upper) winMax = m_currentIntensityRange.upper;

	//CVolTK::DisplayData::iterator wB = m_data.lower_bound(winMin);
	//CVolTK::DisplayData::iterator wE = m_data.upper_bound(winMax);

	std::set<CPoint3s> sasiedztwo;

	//for (itz = wB; itz != wE; itz++)
	for (auto itz = winMin; itz < winMax; itz++)
	{
		//WORD kol = (*itz).first;
		WORD kol = itz;
		StatusBarManager::printfTimed(500, QString("preparing data... (%1)").arg(kol));

		for (const CPoint3s voxel : *m_displayData[itz])
		{
			if ((voxel.X() >= m_b.x) && (voxel.X() < m_e.x) && (voxel.Y() >= m_b.y) && (voxel.Y() < m_e.y) && (voxel.Z() >= m_b.z) && (voxel.Z() < m_e.z))
			{
				sasiedztwo.insert(voxel);
			}
		}
	}

	std::shared_ptr<CMesh> mesh = std::make_shared<CMesh>();

	short div = m_div;

	mesh->vertices().reserve(sasiedztwo.size() * 8);
	mesh->faces().reserve(sasiedztwo.size() * 12);

	StatusBarManager::setText("Creating mesh...");

	for (auto itz = winMin; itz < winMax; itz++)
	{
		WORD kol = itz;
		StatusBarManager::printfTimed(500, QString("Creating mesh... (%1)").arg(kol));

		for (CPoint3s voxel : *m_displayData[itz])
		{
			if ((voxel.X() >= m_b.x) && (voxel.X() < m_e.x) && (voxel.Y() >= m_b.y) && (voxel.Y() < m_e.y) && (voxel.Z() >= m_b.z) && (voxel.Z() < m_e.z))
			{
				dodajKostkeX(mesh.get(), voxel, col, div, sasiedztwo);
			}
		}
	}

	sasiedztwo.clear();

	mesh->removeDuplicateVertices();

	tesst(mesh.get());

	mesh->resetBoundingBox();
	for (CVertex& v : mesh->vertices())
	{
		mesh->expandBoundingBox(v = realXYZ(v));
	}

	return mesh;
}

std::shared_ptr<CModel3D> CVolTK::toMesh()
{
	std::shared_ptr<CModel3D> newModel = std::make_shared<CModel3D>();

	uint16_t wB = m_currentIntensityRange.lower;
	uint16_t wE;

	UI::PROGRESSBAR::init(0, colorFilters.size(), 0);
	int i = 0;

	for (auto f : colorFilters)
	{
		wE = f.first;
		if (wE > wB)
		{
			if (f.second.first)
			{
				newModel->addChild(createMesh(wB, wE, f.second.second));
			}
			wB = wE;
		}
		UI::PROGRESSBAR::setValue(i++);
	}


	wE = m_currentIntensityRange.upper;
	if (wE > wB)
	{
		newModel->addChild(createMesh(wB, wE, CRGBA(1.0f, 1.0f, 1.0f, 0.5f)));
	}

	UI::PROGRESSBAR::hide();

	newModel->importChildrenGeometry();

	return newModel;
}


std::shared_ptr<CPointCloud> CVolTK::createCloud(uint16_t winMin, uint16_t winMax, CRGBA col)
{
	if (winMin >= winMax) return nullptr;

	if (winMin < m_currentIntensityRange.lower) winMin = m_currentIntensityRange.lower;
	if (winMax > m_currentIntensityRange.upper) winMax = m_currentIntensityRange.upper;

//	CVolTK::DisplayData::iterator wB = m_data.lower_bound(winMin);
//	CVolTK::DisplayData::iterator wE = m_data.upper_bound(winMax);

//	CVolTK::DisplayData::iterator itz;

	std::shared_ptr<CPointCloud> cloud = std::make_shared<CPointCloud>();

	int i = 0;
	for (auto itz = winMin; itz < winMax; itz++)
	{
		for (CVolTK::DisplayDataPart::iterator itv = m_displayData[itz]->begin(); itv != m_displayData[itz]->end(); itv++)
		{
			if (((*itv).X() >= m_b.x) && ((*itv).X() < m_e.x) && ((*itv).Y() >= m_b.y) && ((*itv).Y() < m_e.y) && ((*itv).Z() >= m_b.z) && ((*itv).Z() < m_e.z))
			{
				cloud->addVertex(realXYZ(*itv), col);
			}
		}
	}
	
	if (cloud->size() == 0)
	{
		//delete cloud;
		return nullptr;
	}

	return cloud;
}

std::shared_ptr<CModel3D> CVolTK::toCloud()
{
		std::shared_ptr<CModel3D> newModel = std::make_shared<CModel3D>();
		
		uint16_t wB = m_currentIntensityRange.lower;
		uint16_t wE;

		UI::PROGRESSBAR::init(0, colorFilters.size(), 0);
		int i = 0;

		for (auto f : colorFilters)
		{
			wE = f.first;
			if (wE > wB)
			{
				if (f.second.first)
				{
					newModel->addChild(createCloud(wB, wE, f.second.second));
				}
				wB = wE;
			}
			UI::PROGRESSBAR::setValue(i++);
		}


		wE = m_currentIntensityRange.upper;
		if (wE > wB)
		{
			newModel->addChild(createCloud(wB, wE, CRGBA(1.0f, 1.0f, 1.0f, 0.5f)));
		}

		UI::PROGRESSBAR::hide();

		newModel->importChildrenGeometry();
		return newModel;
}


CPoint3d CVolTK::realXYZ(CPoint3d point)
{
	//return CPoint3d(m_voxelSize.x * point.x, m_voxelSize.y * point.y - m_voxelSize.z * point.z * tan(deg2rad(m_alpha)), m_voxelSize.z * point.z);
	return kostka.getRealXYZ(point, m_alpha);
}

CPoint3d CVolTK::realXYZ(double x, double y, double z)
{
	//return CPoint3d(m_voxelSize.x * x, m_voxelSize.y * y - m_voxelSize.z * z * tan(deg2rad(m_alpha)), m_voxelSize.z * z);
	return kostka.getRealXYZ(x, y, z, m_alpha);
}

void CVolTK::setGeometry()
{
	m_b = m_Min;
	m_e = m_Max + CPoint3s(1,1,1);

	setMin(realXYZ(m_Min));
	setMax(realXYZ(m_Max));
}


QImage CVolTK::getLayerAsImage(int16_t nr, CVolTK::Layer layer, int dispBits)
{
	QImage image;

	QImage::Format imgFormat;
	WORD maxDisplVal;

	switch (dispBits)
	{
	case 8:
		imgFormat = QImage::Format::Format_Grayscale8;
		maxDisplVal = 0xff;
		break;
	case 12:
		imgFormat = QImage::Format::Format_Grayscale16;
		maxDisplVal = 0xfff;
		break;
	case 16:
	default:
		imgFormat = QImage::Format::Format_Grayscale16;
		maxDisplVal = 0xffff;
		break;
	}

	switch (layer)
	{
	case YZ:
		image = QImage(kostka.m_rows, kostka.m_lays, imgFormat);
		for (qint32 y = 0; y < kostka.m_lays; y++)
		{
			uchar* line = image.scanLine(y);

			for (qint32 x = 0; x < kostka.m_rows; x++)
			{
				if (dispBits==8)
					//line[x] = kostka.getRescaled(nr, x, y, 0, maxDisplVal);
					line[x] = kostka.get(nr, x, y);
				else
					//((uint16_t*)line)[x] = kostka.getRescaled(nr, x, y, 0, maxDisplVal);
					((uint16_t*)line)[x] = kostka.get(nr, x, y);
			}
		}
		return image;
		break;
	case ZX:
		image = QImage(kostka.m_lays, kostka.m_cols, imgFormat);
		for (qint32 y = 0; y < kostka.m_cols; y++)
		{
			uchar* line = image.scanLine(y);

			for (qint32 x = 0; x < kostka.m_lays; x++)
			{
				if (dispBits == 8)
					//line[x] = kostka.getRescaled(y, nr, x, 0, maxDisplVal);
					line[x] = kostka.get(y, nr, x);
				else
					//((uint16_t*)line)[x] = kostka.getRescaled(y, nr, x, 0, maxDisplVal);
					((uint16_t*)line)[x] = kostka.get(y, nr, x);
			}
		}
		return image;
		break;
	case XY:
	default:
		image = QImage(kostka.m_cols, kostka.m_rows, imgFormat);
		for (qint32 y = 0; y < kostka.m_rows; y++)
		{
			uchar* line = image.scanLine(y);

			for (qint32 x = 0; x < kostka.m_cols; x++)
			{
				if (dispBits == 8)
					//line[x] = kostka.getRescaled(x, y, nr, 0, maxDisplVal);
					line[x] = kostka.get(x, y, nr);
				else
					//((uint16_t*)line)[x] = kostka.getRescaled(x, y, nr, 0, maxDisplVal);
					((uint16_t*)line)[x] = kostka.get(x, y, nr);
			}
		}
		return image;
		break;
	}
}
