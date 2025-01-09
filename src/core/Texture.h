#ifndef TEXTURE_H
#define TEXTURE_H

#include "dll_global.h"

#include <QtWinExtras/QtWinExtras>
#include <QtCore/QString>

class DPVISION_EXPORT CTexture : public QOpenGLTexture
{
public:
	//using QOpenGLTexture::QOpenGLTexture;
	CTexture(const QImage &img);
	//CTexture(const CTexture& tex);
	CTexture(QString path);
	CTexture(QString path, const char* format);
	CTexture(std::string path);
	CTexture(std::wstring path);
	
	const QImage toImage();
};

#endif