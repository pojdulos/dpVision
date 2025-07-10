#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

#include <list>




class DPVISION_EXPORT CAnnotationCamera : public CAnnotation
{
public:
	struct DPVISION_EXPORT Sensor {
		int xResolution;
		int yResolution;
		double realWidth;
		double realHeight;
		double mmPixelWidth;
		double mmPixelHeight;

		double pxFocalLenght;
		double mmFocalLength;

		Sensor() : xResolution(1800), yResolution(1200), mmFocalLength(50.0), realWidth(36.0), realHeight(24.0)
		{
			mmPixelWidth = realWidth / xResolution;
			mmPixelHeight = realHeight / yResolution;

			pxFocalLenght = mmFocalLength * 2.0 / (mmPixelWidth + mmPixelHeight);
		}

		Sensor(int xR, int yR, double fL=50.0) : xResolution(xR), yResolution(yR), mmFocalLength(fL), realWidth(36.0), realHeight(24.0)
		{
			mmPixelWidth = realWidth / xResolution;
			mmPixelHeight = realHeight / yResolution;

			pxFocalLenght = mmFocalLength * 2.0 / (mmPixelWidth + mmPixelHeight);
		}

		Sensor( Sensor &s )
		{
			xResolution = s.xResolution;
			yResolution = s.yResolution;
			mmFocalLength = s.mmFocalLength;
			realWidth = s.realWidth;
			realHeight = s.realHeight;
			mmPixelWidth = s.mmPixelWidth;
			mmPixelHeight = s.mmPixelHeight;
			pxFocalLenght = s.pxFocalLenght;
		}

		void setFocalLenght(double mm) { mmFocalLength = mm; }

		void setResolution(int xResolution, int yResolution)
		{
			xResolution = xResolution;
			yResolution = yResolution;

			realWidth = (double)xResolution * mmPixelWidth;
			realHeight = (double)yResolution * mmPixelHeight;
		}

		void setPixelSize(double pw, double ph)
		{
			mmPixelWidth = pw;
			mmPixelHeight = ph;

			realWidth = (double)xResolution * mmPixelWidth;
			realHeight = (double)yResolution * mmPixelHeight;
		}
	};

private:
	Sensor m_sensor;

	// OpenGL style matrix (translation components occupy the 13th, 14th, and 15th elements (indices: 12-14) of the 16-element matrix)
	double m_matrix[16] = { 1.0, 0.0, 0.0, 0.0,
		                    0.0, 1.0, 0.0, 0.0,
		                    0.0, 0.0, 1.0, 0.0,
		                    0.0, 0.0, 0.0, 1.0 };

public:
	CAnnotationCamera(std::shared_ptr<CBaseObject> m = nullptr) :CAnnotation(m)
	{
		setLabel("Camera");
	};

	CAnnotationCamera( CAnnotationCamera &p ) :CAnnotation(p)
	{
		setLabel("Camera");
	}

	~CAnnotationCamera() {};

	virtual int type() { return CAnnotation::CAMERA; }

	static CAnnotationCamera* create(Sensor s, double matrix[16], std::string label);
	static CAnnotationCamera* create( Sensor s, std::string matrixSTR, std::string label);

	
	Sensor& sensor() { return m_sensor; }

	double* matrix() { return m_matrix; }

	// Remember: "rows" and "cols" are in "math" notation here,
	// so I expect that translation components are in 3 column
	double &matrix( int col, int row )
	{
		// m_matrix is OpenGL style, so I need to invert it
		return m_matrix[4 * col + row];
	}

	void setMatrix(std::string matrixSTR);

	void setMatrix(double matrix[16]);


	virtual std::wstring getInfoRow();

	virtual void renderSelf() override;
};

