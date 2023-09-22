#include "Texture.h"

CTexture::CTexture(const CTexture& tex) : QOpenGLTexture(QOpenGLTexture::Target2D)
{
	m_image = QImage(tex.m_image);

	setData(m_image.mirrored());
}

CTexture::CTexture(QString path) : QOpenGLTexture(QImage(path).mirrored())
{
	m_image = QImage(path);
}

CTexture::CTexture(QString path, const char* format) : QOpenGLTexture(QImage(path, format).mirrored())
{
	m_image = QImage(path, format);
}

CTexture::CTexture(std::string path) : QOpenGLTexture(QImage(QString::fromStdString(path)).mirrored())
{
	m_image = QImage(QString::fromStdString(path));
}

CTexture::CTexture(std::wstring path) : QOpenGLTexture(QImage(QString::fromStdWString(path)).mirrored())
{
	m_image = QImage(QString::fromStdWString(path));
}
