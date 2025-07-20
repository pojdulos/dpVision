#include "PicViewer.h"

#include "ImageLabel.h"
#include "Image.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include "../api/AP.h"

PicViewer::PicViewer(QWidget* parent) : QWidget( parent )
{
	setupUi(this);

	m_images.clear();
	m_current = -1;
}

PicViewer::PicViewer(CImage *im, QWidget* parent) : QWidget(parent)
{
	setupUi(this);

	setImage(im);
}

void PicViewer::setImage(CImage* im)
{
	m_images.clear();
	if (im == nullptr)
	{
		m_current = -1;
	}
	else
	{
		m_images.push_back(im);
		m_current = 0;
	}
	reloadImage();
}

void PicViewer::addImage(CImage* im)
{
	if (im != nullptr)
	{
		m_images.push_back(im);
	}
	reloadImage();
}

void PicViewer::setImages(std::vector<CImage*> im)
{
	m_images.clear();
	if (im.empty())
	{
		m_current = -1;
	}
	else
	{
		m_images.assign(im.begin(), im.end());
		m_current = 0;
	}
	reloadImage();
}

void PicViewer::addImages(std::vector<CImage*> im)
{
	if ( ! im.empty() )
	{
		m_images.insert(m_images.end(), im.begin(), im.end());
	}
	reloadImage();
}

void PicViewer::removeImages()
{
	m_images.clear();
	m_current = -1;
	reloadImage();
}

void PicViewer::removeImage(int i)
{
	if (m_images.empty() || (m_images.size() <= i))
	{
		return;
	}

	std::vector<CImage*> tmp = m_images;

	m_images.clear();
	
	if (i>0)
		m_images.assign(tmp.begin(), tmp.begin()+i-1);
	
	if (i<(tmp.size()-1))
		m_images.insert(m_images.end(), tmp.begin()+i+1, tmp.end());
	
	if (m_current>=m_images.size())
		m_current = m_images.size()-1;

	tmp.clear();

	reloadImage();
}


void PicViewer::reloadImage()
{
	if (m_images.empty() || (m_images.size() <= m_current))
	{
		imageLabel->setPixmap( QPixmap(200,150) );
		imageLabel->m_scale = 1.0;
		return;
	}

	QPixmap pic = QPixmap::fromImage(*(QImage*)m_images[m_current]);

	int imgW = pic.width();
	int imgH = pic.height();

	if (m_images[m_current]->fitToWindow)
	{
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

		int w = scrollArea->width();
		int h = scrollArea->height();

		QPixmap p = pic.scaled(w, h, Qt::KeepAspectRatio);
		imageLabel->setPixmap(p);

		imageLabel->m_scale = (double)p.width() / pic.width();
	}
	else
	{
		scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
		scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

		imageLabel->setPixmap(pic);

		imageLabel->m_scale = 1.0;
	}
}

int PicViewer::id()
{
	if (m_images.empty() || (m_images.size() <= m_current))
		return -1;
	else if (m_images[m_current] != nullptr)
		return m_images[m_current]->id();
	else
		return -1;
}

void PicViewer::setupUi( PicViewer *picViewer )
{
	QVBoxLayout* mainLayout = new QVBoxLayout(picViewer);
	mainLayout->setSpacing(0);
	mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
	mainLayout->setContentsMargins(0, 0, 0, 0);


	scrollArea = new QScrollArea(picViewer);
	scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
	scrollArea->setMinimumSize(QSize(0, 0));
	scrollArea->setMaximumSize(QSize(16777215, 16777215));
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setLineWidth(0);
	scrollArea->setWidgetResizable(true);

	QWidget* scrollAreaWidgetContents = new QWidget();
	scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
	scrollAreaWidgetContents->setGeometry(QRect(0, 0, 963, 642));

	QVBoxLayout* verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);
	verticalLayout->setSpacing(0);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	verticalLayout->setContentsMargins(0, 0, 0, 0);

	imageLabel = new ImageLabel(scrollAreaWidgetContents);
	imageLabel->setObjectName(QString::fromUtf8("imageLabel"));

	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(imageLabel->sizePolicy().hasHeightForWidth());
	imageLabel->setSizePolicy(sizePolicy);

	verticalLayout->addWidget(imageLabel);

	QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	verticalLayout->addItem(verticalSpacer);

	scrollArea->setWidget(scrollAreaWidgetContents);

	mainLayout->addWidget(scrollArea);


	imageLabel->setMouseTracking(true);
	imageLabel->m_scale = 1.0;
}


void PicViewer::resizeEvent(QResizeEvent* e)
{
	QWidget::resizeEvent(e);

	reloadImage();
}


