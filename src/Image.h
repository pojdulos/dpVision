#pragma once
#include <QtGui/QImage>
#include <QtCore/QString>

#include "Model3D.h"

class  __declspec(dllexport) CImage : protected CModel3D, protected QImage
{
protected:
	CImage(const QImage& i);

public:
	typedef enum QImage::Format Format;
	typedef enum Qt::GlobalColor Color;

	bool fitToWindow;

	/*
	constructor creating an null-size CImage instance, so instance->isNull() returns true.
	*/
	CImage();
	
	/*
	This constructor always (!!!) returns valid CImage instance, but it is a null-size image if load error.
	So never check if NULL is returned by 'new' command, but check if newImage->isNull() returns true.
	You can also use static CImage* load(path) method. It returns NULL on load error and don't create invalid CImage instance.
	*/
	CImage(const QString& path, const char* format = nullptr);
	CImage(const char* path) { CImage(QString(path)); }
	CImage(const std::string path) { CImage(QString::fromStdString(path)); }
	CImage(const std::wstring path) { CImage(QString::fromStdWString(path)); }

	//copying constructor
	CImage(const CImage& i);

	CImage(const uchar* data, int w, int h, CImage::Format f);
	
	CImage(int w, int h, CImage::Format f);
	
	virtual ~CImage();

	static CImage* load(const char* path);
	static CImage* load(const QString path);
	static CImage* load(const std::string path);
	static CImage* load(const std::wstring path);

	void save(const char* path);
	void save(const QString path);
	void save(const std::string path);
	void save(const std::wstring path);

	using CModel3D::id;
	using CModel3D::setLabel;
	using CModel3D::getLabel;
	using CModel3D::setSelfVisibility;
	using CModel3D::path;
	using CModel3D::setPath;
	using CModel3D::getTransform;
	using CModel3D::render;

	virtual CImage* getCopy() override { return new CImage(*this); }

	CImage* copy(int x, int y, int w, int h) const;

	using QImage::isNull;
	using QImage::width;
	using QImage::height;
	using QImage::depth;

	using QImage::scanLine;
	using QImage::constScanLine;
	using QImage::bits;
	using QImage::constBits;
	using QImage::setColorCount;

	using QImage::mirrored;

	QImage toQImage() { return *(QImage*)this; };

	uint64_t sizeInBytes() const;

	void fill(CImage::Color color);

	//kolor w formacie: #AARRGGBB
	void setColor(int i, uint32_t argb);

	//kolor w formacie: #AARRGGBB
	uint32_t pixel(int x, int y) const;

	//kolor w formacie: #AARRGGBB
	void setPixel(int x, int y, uint32_t pixel);

	CImage::Format format() const;

	virtual void renderSelf() override;

	virtual inline int type() override { return CObject::Type::IMAGE; };
	//void info(std::wstring i[4]) override;
	virtual std::wstring infoRow() override;

	static std::pair<int, int> loadToBYTEvector(std::wstring fname, std::vector<BYTE>& bufor, int& depth);
	static std::pair<int, int> loadToRGBAvector(std::wstring fname, std::vector<CRGBA>& bufor, uchar alpha = 255);
};

