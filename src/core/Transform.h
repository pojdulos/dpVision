#pragma once

#include "Global.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "Quaternion.h"

#include<Eigen/Geometry>
//#include <QtGui/QMatrix4x4>
#include <memory>

class ITransformRenderer;

class  DPVISION_EXPORT CTransform
{
	std::shared_ptr<ITransformRenderer> renderer_ = nullptr;

	CVector3d m_tra; // translacja
	CQuaternion m_rot; // kwaternion obrotu
	CPoint3d m_sca; // skala

	bool m_bLocked; // czy zablokowa� mo�liwo�� zmiany pozycji

	bool m_show_screw;

	bool m_bRelocateCtr; // czy centrowa� obiekt na scenie podczas rysowania
	CPoint3d m_origin; // okre�la �rodek obrotu we wsp�rz�dnych obiektu - i jednocze�nie punkt 0,0 dla centrowania

public:
	typedef enum {
		ZERO,
		WEIGHT_CTR,
		BBOX_CTR
	} Origin;


	CTransform();
	CTransform(const CTransform &t);
	CTransform(const Eigen::Matrix4d& mat);
	~CTransform();

	void reset();

	// Obroty s� zwykle wykonywane wok� punktu 0,0, aby obiekt nie "lata�" po scenie podczas obracania
	// jego �rodek obrotu jest domy�lnie ustawiany w �rodku obiektu, wg wzoru: (max+min)/2
	CPoint3d &origin() { return m_origin; }
	
	// Deprecated, the same as origin() 
	CPoint3d &centerOfRotation() { return m_origin; }

	// mo�na to oczywi�cie zmieni� i ustawi� go praktycznie dowolnie:
	inline void setOrigin(CPoint3d c = CPoint3d(0, 0, 0)) { m_origin = c; }

	// Deprecated, the same as setOrigin(CPoint3f c)
	inline void setCenterOfRotation(CPoint3d c) { setOrigin(c); }

	// czasami istnieje potrzeba wycentrowania obiektu na scenie, bo np. jego wsp�rz�dne s� bardzo odleg�e od centrum uk�adu
	// i nie mie�ci�by sie na ekranie. �eby nie wprowadza� dodatkowych zmiennych, wykorzystujemy �rodek obrotu jako punkt
	// to�samy z pocz�tkiem "wirtualnego" uk�adu wsp�rz�dnych
	bool isTheOriginMovedToTheCenterOfRotation() { return m_bRelocateCtr; };
	void moveTheOriginToTheCenterOfRotation(bool b) { m_bRelocateCtr = b; };


	//void fromGLMatrixD(double matrix[16]);

	//Przyj��em, �e skala b�dzie jednakowa w ka�dej z osi
	CPoint3d &scale() { return m_sca; }
	
	void setScale(CPoint3d s) { m_sca = s; };
	void setScale(double s) { m_sca.x = m_sca.y = m_sca.z = s; };

	CVector3d &translation() { return m_tra; }
	const CVector3d& translate(CVector3d ruch);

	CQuaternion &rotation() { return m_rot; }
	
	void rotate(std::vector<std::pair<CVector3d, double>> axisAngle, bool internal = true);
	void rotate(CPoint3d origin, std::vector<std::pair<CVector3d, double>> axisAngle, bool internal = true);

	inline void rotateDeg(std::vector<std::pair<CVector3d, double>> axisAngle, bool internal = true)
	{
		for (auto& aa : axisAngle)
			aa.second = deg2rad(aa.second);
		rotate(axisAngle, internal);
	}

	inline void rotateDeg(CPoint3d origin, std::vector<std::pair<CVector3d, double>> axisAngle, bool internal = true)
	{
		for (auto& aa : axisAngle)
			aa.second = deg2rad(aa.second);
		rotate(origin, axisAngle, internal);
	}

	void showScrew(bool b) { m_show_screw = b; }
	bool isScrewVisible() { return m_show_screw; }

	void rotateAroundAxis(CVector3d axis, double angle, bool internal = true);
	void rotateAroundAxis(CPoint3d origin, CVector3d axis, double angle, bool internal = true);

	inline void rotateAroundAxisDeg(CVector3d axis, double angle, bool internal = true)
	{
		rotateAroundAxis(axis, deg2rad(angle), internal);
	}

	inline void rotateAroundAxisDeg(CPoint3d origin, CVector3d axis, double angle, bool internal = true)
	{
		rotateAroundAxis(origin, axis, deg2rad(angle), internal);
	}

	void rotateByMatrix3x3(double m[9]);

	bool isLocked() { return m_bLocked; }
	bool lock(bool b) { return m_bLocked = b; }

	void fromGLMatrixF(float* matrix);
	void fromGLMatrixD(double* matrix);
	void fromRowMatrixD(double* matrix);
	void fromEigenMatrix4d(Eigen::Matrix4d m);
	void fromQMatrix4x4(QMatrix4x4 m);


	void toGLMatrixF(float *matrix);
	void toGLMatrixD(double* matrix);
	void toRowMatrixD(double* matrix);
	Eigen::Matrix4d toEigenMatrix4d();
	QMatrix4x4 toQMatrix4x4();


	//bool invertMatrixD(const double m[16], double invOut[16]);
	//bool invertMatrixF(const float m[16], float invOut[16]);

	//void toInvertedGLMatrixF(float* matrix);
	//void toInvertedGLMatrixD(double* matrix);
	//void toInvertedRowMatrixD(double* matrix);

	static CTransform fromTo(CTransform from, CTransform to);


	template<typename _W> CVector3<_W> operator*(CVector3<_W> p) {
		return Eigen::Vector4d(this->toEigenMatrix4d() * p.toVector4());
	};

	template<typename _W> CPoint3<_W> operator*(CPoint3<_W> p) {
		return Eigen::Vector4d(this->toEigenMatrix4d() * p.toVector4());
	};

	template<typename _W> CVector3<_W> operator/(CVector3<_W> p) {
		return Eigen::Vector4d(this->toEigenMatrix4d() * p.toVector4());
	};

	template<typename _W> CPoint3<_W> operator/(CPoint3<_W> p) {
		return Eigen::Vector4d(this->toEigenMatrix4d().inverse() * p.toVector4());
	};



	//CPoint3d l2w_test(const CPoint3d pkt);
	//CPoint3d w2l_test(const CPoint3d pkt);

	CPoint3d l2w(const CPoint3d pkt1);
	CPoint3d w2l(const CPoint3d pkt1);

	inline CPoint3d local2world(const CPoint3d pkt1) { return l2w(pkt1); };
	inline CPoint3d world2local(const CPoint3d pkt1) { return w2l(pkt1); };


	std::wstring infoRow();

	QString toString(QString prefix = "", QString suffix = "", QString separator = " ", const char* format = "%.8f");

	void fromRowMatrix(QString text, QString separator= "(\\s+)");

	void renderScrew(float r, float g, float b);

	void render();
};


template<typename _W> inline const CPoint3<_W> operator/(CPoint3<_W> p, CTransform t)
{
	return Eigen::Matrix<_W, 4, 1>(t.toEigenMatrix4d().inverse() * p.toVector4());
}

template<typename _W> inline const CVector3<_W> operator/(CVector3<_W> p, CTransform t)
{
	return Eigen::Matrix<_W, 4, 1>(t.toEigenMatrix4d().inverse() * p.toVector4());
}


template<typename _W> inline const CPoint3<_W> operator*(Eigen::Matrix<_W, 4, 4> m, CPoint3<_W> p)
{
	return Eigen::Matrix<_W, 4, 1>(m * p.toVector4());
}

template<typename _W> inline const CVector3<_W> operator*(Eigen::Matrix<_W, 4, 4> m, CVector3<_W> v)
{
	return Eigen::Matrix<_W, 4, 1>(m * v.toVector4());
}

typedef CTransform* PtrTransform;
typedef CTransform& RefTransform;
