#include "Texture.h"

CTexture::CTexture(const QImage& img) : QOpenGLTexture(img.mirrored()) {}
//CTexture::CTexture(const CTexture& tex) : QOpenGLTexture(((QOpenGLTexture&)tex)) {}
CTexture::CTexture(QString path) : QOpenGLTexture(QImage(path).mirrored()) {}
CTexture::CTexture(QString path, const char* format) : QOpenGLTexture(QImage(path, format).mirrored()) {}
CTexture::CTexture(std::string path) : QOpenGLTexture(QImage(QString::fromStdString(path)).mirrored()) {}
CTexture::CTexture(std::wstring path) : QOpenGLTexture(QImage(QString::fromStdWString(path)).mirrored()) {}

const QImage CTexture::toImage()
{
	this->bind();
	int width = this->width();
	int height = this->height();

	// Przygotowanie bufora na dane pikseli
	QImage image(width, height, QImage::Format_RGBA8888);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

	image = image.mirrored();
	this->release();
	return image;
}