#pragma once
#include "Plugin.h"
#include "PluginDLL.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "Vertex.h"

#include "icpPointToPlane.h"
#include "icpPointToPoint.h"

#ifdef PLUGIN_NAME
    #undef PLUGIN_NAME
#endif 
#define PLUGIN_NAME L"(dp) Test ICP"

class CAnnotationPoint;
class CSymulatorZgryzu;
class CModel3D;
class CPointCloud;

class CPPlane
{
public:
	CPoint3d center;
	CVector3d normal;
};


// propozycja dp:
typedef std::vector< std::vector<float>* > pDistStruct;
// "zewnętrzny" wektor: N-1 elementów (od 0 do pts_cnt-1)
// "wewnętrzny" wektor: (N-1)-i elementów, gdzie i jest indeksem w "zewnętrznym" wektorze
// odwołanie: pDistStruct[A]->at(B) = odległość od punktu A do B, gdzie zawsze B > A


class PLUGINDLL_API ConcretePlugin : public Plugin
{
	bool m_picking;
	int ptCnt;
	bool bNegNormal;
	bool bRegrCalculated;
	int rotDir; //1 - X axis, 2 - Y axis, 3 - Z axis

	//std::vector<CPunkt3D> selPoints;  //point of floats
	std::vector<CPoint3d> selPoints;  //point of double
	std::vector<CVector3d> resNorm;  //vector of double

	void pdistDP(std::vector<CVertex> pts, pDistStruct& pd);

	void pdist(std::vector<CVertex> pts, std::vector<float> &pd);


	void plane_from_points(std::vector<CPoint3d> pts, CPPlane &fit_plane);

	double CP_dot(CVector3d p1, CVector3d p2)
	{
		return p1.X()* p2.X() + p1.Y()* p2.Y() + p1.Z()* p2.Z();
	}

	CVector3d CP_normalize(CVector3d vect)
	{
		double v_len = vect.X()*vect.X() + vect.Y()*vect.Y() + vect.Z()*vect.Z();
		if (v_len != 0.0)
			return vect / sqrt(v_len);
		else return vect;
	}

	CPPlane fitPlane;
	//double dRotMatrix[16];

	//void calc_rot_matrix(CPPlane fit_plane, double *rot_mat);
	void rotX90(double *rot_mat);
	void rotY90(double *rot_mat);
	void rotZ90(double *rot_mat);

	double fit(IcpPointToPoint& icp, CPointCloud* meshA, Matrix& R, Matrix& t);

	double fit(IcpPointToPlane& icp, CPointCloud* meshA, Matrix& R, Matrix& t);

public:
    ConcretePlugin(void);
    ~ConcretePlugin(void);

	/* ============= EVENT HANDLING BEGIN =========================*/

	// called on doubleclick on plugin name
	virtual void onRun() override;

	// called when plugin is loaded
	virtual void onLoad() override;

	// called before plugin unload
	virtual void onUnload() override;

	// called when you select plugin
	//virtual void onActivate() override {};

	// called when you select another plugin
	//virtual void onDeactivate() override {};

	CPointCloud* randDecimate(CPointCloud* d, int newsize=1000);

	void testICP_v1();

	CPointCloud* findNClosest(CPointCloud* ruchoma, CPointCloud* nieruchoma, int limit);

	// called if button on PluginPanel is pressed
	virtual void onButton(std::wstring name) override;

	// called if plugin is active and ctrl+MouseClick is done on any model
	virtual bool onMousePick(Plugin::PickEvent pickEvent) override;

	/* ============= EVENT HANDLING END =========================*/

};
