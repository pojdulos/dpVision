#include "Image.h"
#include "AP.h"
#include "MainWindow.h"
#include "MdiChild.h"

CImage::CImage() : CModel3D(), QImage()
{
	setLabel("image");
	fitToWindow = false;

	CModel3D::bDrawBB = false;
}

CImage::CImage(const QString& path, const char* format) : CModel3D(), QImage(path, format)
{
	m_label = QFileInfo(path).fileName();
	m_path = path;
	fitToWindow = false;

	img3d_half_width = (float)this->width() / 100.0f;
	img3d_half_height = (float)this->height() / 100.0f;

	setMin(CPoint3d(-img3d_half_width, -img3d_half_height, 0));
	setMax(CPoint3d(img3d_half_width, img3d_half_height, 0));

	CModel3D::bDrawBB = false;
}


CImage::CImage(const QImage& i) : CModel3D(), QImage(i)
{
	setLabel("image");
	fitToWindow = false;

	img3d_half_width = (float)this->width() / 100.0f;
	img3d_half_height = (float)this->height() / 100.0f;

	setMin(CPoint3d(-img3d_half_width, -img3d_half_height, 0));
	setMax(CPoint3d(img3d_half_width, img3d_half_height, 0));

	CModel3D::bDrawBB = false;
}

CImage::CImage(const CImage& i) : CModel3D( i ), QImage( i )
{
	setLabel("image");
	fitToWindow = false;

	img3d_half_width = (float)this->width() / 100.0f;
	img3d_half_height = (float)this->height() / 100.0f;

	setMin(CPoint3d(-img3d_half_width, -img3d_half_height, 0));
	setMax(CPoint3d(img3d_half_width, img3d_half_height, 0));

	CModel3D::bDrawBB = false;
}

CImage::CImage(const uchar* b, int ww, int hh, CImage::Format f) : CModel3D(), QImage(b, ww, hh, f)
{
	setLabel("image");
	fitToWindow = false;

	img3d_half_width = (float)this->width() / 100.0f;
	img3d_half_height = (float)this->height() / 100.0f;

	setMin(CPoint3d(-img3d_half_width, -img3d_half_height, 0));
	setMax(CPoint3d(img3d_half_width, img3d_half_height, 0));

	CModel3D::bDrawBB = false;
}

CImage::CImage(int ww, int hh, CImage::Format f) : CModel3D(), QImage(ww, hh, f)
{
	setLabel("image");
	fitToWindow = false;

	img3d_half_width = (float)this->width() / 100.0f;
	img3d_half_height = (float)this->height() / 100.0f;

	setMin(CPoint3d(-img3d_half_width, -img3d_half_height, 0));
	setMax(CPoint3d(img3d_half_width, img3d_half_height, 0));

	CModel3D::bDrawBB = false;
}

CImage::~CImage() {}

CImage* CImage::load(const char* path)
{
	QImage qimage(path);

	if (qimage.isNull())
		return nullptr;

	CImage* cimage = new CImage(qimage);
	cimage->setLabel(QFileInfo(path).fileName());
	cimage->setPath(QFileInfo(path).absoluteFilePath());

	return cimage;
}

CImage* CImage::load(const QString path)
{
	QImage qimage(path);

	if (qimage.isNull())
		return nullptr;

	CImage* cimage = new CImage(qimage);
	cimage->setLabel(QFileInfo(path).fileName());
	cimage->setPath(QFileInfo(path).absoluteFilePath());

	return cimage;
}

CImage* CImage::load(const std::string path)
{
	QImage qimage(QString::fromStdString(path));

	if (qimage.isNull())
		return nullptr;

	CImage* cimage = new CImage(qimage);
	cimage->setLabel(QFileInfo(QString::fromStdString(path)).fileName());
	cimage->setPath(QFileInfo(QString::fromStdString(path)).absoluteFilePath());

	return cimage;
}

CImage* CImage::load(const std::wstring path)
{
	QImage qimage(QString::fromStdWString(path));

	if (qimage.isNull())
		return nullptr;

	CImage* cimage = new CImage(qimage);
	cimage->setLabel(QFileInfo(QString::fromStdWString(path)).fileName());
	cimage->setPath(QFileInfo(QString::fromStdWString(path)).absoluteFilePath());

	return cimage;
}

void CImage::save(const char* path) { QImage::save(QString(path)); }
void CImage::save(const QString path) { QImage::save(path); }
void CImage::save(const std::string path) { QImage::save(QString::fromStdString(path)); }
void CImage::save(const std::wstring path) { QImage::save(QString::fromStdWString(path)); }

CImage* CImage::copy(int x, int y, int w, int h) const { return new CImage(QImage::copy(x, y, w, h)); }

uint64_t CImage::sizeInBytes() const { return QImage::sizeInBytes(); }

void CImage::fill(CImage::Color color) { QImage::fill(color); }

void CImage::setColor(int i, uint32_t argb) { QImage::setColor(i, argb); }
uint32_t CImage::pixel( int x, int y ) const { return QImage::pixel(x, y); }
void CImage::setPixel(int x, int y, uint32_t pixel) { QImage::setPixel(x, y, pixel); }
CImage::Format CImage::format() const { return QImage::format(); }

void CImage::renderSelf()
{
	if (m_showSelf)
	{
		QOpenGLTexture ogltx(*this);

		glLoadName(m_Id);
		glPushName(m_Id);

		glPushAttrib(GL_ALL_ATTRIB_BITS);


		glBindTexture(GL_TEXTURE_2D, ogltx.textureId());
		glEnable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);

		glBegin(GL_QUADS);

		glTexCoord2f(0.0, 0.0); 		glVertex2f(-img3d_half_width,  img3d_half_height);
		glTexCoord2f(1.0, 0.0); 		glVertex2f( img3d_half_width,  img3d_half_height);
		glTexCoord2f(1.0, 1.0); 		glVertex2f( img3d_half_width, -img3d_half_height);
		glTexCoord2f(0.0, 1.0); 		glVertex2f(-img3d_half_width, -img3d_half_height);

		glEnd();


		glDisable(GL_TEXTURE_2D);
		glDisable(GL_COLOR_MATERIAL);

		glPopAttrib();

		glPopName();
		glLoadName(0);

		if (bDrawBB) renderBoundingBox();
	}
}


//void CImage::info(std::wstring i[4])
//{
//	i[0] = m_label;
//	i[1] = std::to_wstring(m_Id);
//	i[2] = L"Model 3D";
//	i[3] = infoRow();
//}


QString formaty[] = { 
	"Invalid",
	"Mono",
	"MonoLSB",
	"Indexed8",
	"RGB32",
	"ARGB32",
	"ARGB32",
	"RGB16",
	"ARGB8565_Premultiplied",
	"RGB666",
	"ARGB6666_Premultiplied",
	"RGB555",
	"ARGB8555_Premultiplied",
	"RGB888",
	"RGB444",
	"ARGB4444_Premultiplied",
	"RGBX8888",
	"RGBA8888",
	"RGBA8888_Premultiplied",
	"BGR30",
	"A2BGR30_Premultiplied",
	"RGB30",
	"A2RGB30_Premultiplied",
	"Alpha8",
	"Grayscale8",
	"Grayscale16",
	"RGBX64",
	"RGBA64",
	"RGBA64_Premultiplied",
	"BGR888"
};

std::wstring CImage::infoRow()
{
	QString info = "Image2D\n";
		info.append("width  = " + QString::number(width()) + "\n");
		info.append("height = " + QString::number(height()) + "\n");
		info.append("depth  = " + QString::number(depth()) + "\n");
		info.append("format = " + formaty[format()] + "\n");

	
	return info.toStdWString();
}



std::pair<int, int> CImage::loadToBYTEvector(std::wstring fname, std::vector<BYTE>& bufor, int& depth)
{
	QImage image(QString::fromStdWString(fname));

	depth = image.depth();

	uchar* bits = image.bits(); // pozyskujê wskaŸnik na pocz¹tek tablicy bitów

	uchar* bitsEnd = bits + image.sizeInBytes(); // obliczam wskaŸnik na koniec tablicy

	bufor.clear();
	bufor.reserve(image.sizeInBytes());

	for (uchar* index = bits; index < bitsEnd; index++) {
		bufor.push_back(*index);
	}

	return std::pair<int, int>(image.width(), image.height());
}

std::pair<int, int> CImage::loadToRGBAvector(std::wstring fname, std::vector<CRGBA>& bufor, uchar alpha)
{
	QImage image(QString::fromStdWString(fname));

	int depth = image.depth();

	UI::STATUSBAR::printf(L"depth: %d", depth);
	uchar* bits = image.bits(); // pozyskujê wskaŸnik na pocz¹tek tablicy bitów

	uchar* bitsEnd = bits + image.sizeInBytes(); // obliczam wskaŸnik na koniec tablicy

	bufor.clear();
	bufor.reserve(image.sizeInBytes() / 4);

	if (depth == 16)
	{
		for (uchar* index = bits; index < bitsEnd; index += 2) {
			bufor.push_back(CRGBA(*index, *(index + 1), 0, alpha));
		}
	}
	else
	{
		for (uchar* index = bits; index < bitsEnd; index += 4) {
			CRGBA c = CRGBA::fromArgb32(index);
			c.setAlpha(alpha);
			bufor.push_back(c);
		}
	}

	return std::pair<int, int>(image.width(), image.height());
}

