#ifndef TEXTURE_H
#define TEXTURE_H

//#include "gltools.h"


#include <QtWinExtras/QtWinExtras>
#include <QtCore/QString>

#ifdef DPVISION_PLUGIN_DLL
#define MYDECL __declspec(dllimport)
#else
#define MYDECL __declspec(dllexport)
#endif

class MYDECL CTexture : public QOpenGLTexture
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