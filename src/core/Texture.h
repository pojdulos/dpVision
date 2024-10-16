#ifndef TEXTURE_H
#define TEXTURE_H

#include "dll_global.h"



#include <QtWinExtras/QtWinExtras>
#include <QtCore/QString>

class CTexture : public QOpenGLTexture
{
public:
	QImage m_image;

	using QOpenGLTexture::QOpenGLTexture;
	CTexture(const CTexture& tex);
	CTexture(QString path);
	CTexture(QString path, const char* format);
	CTexture(std::string path);
	CTexture(std::wstring path);
};

#endif