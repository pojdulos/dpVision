#include "TKimageDialog.h"


//#include <QResizeEvent>

#include "ImageLabel.h"



///////////////////////////////////////////////////////////////////////////////////////////
// ImageDialog
///////////////////////////////////////////////////////////////////////////////////////////

TKimageDialog::TKimageDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

	m_imageLabel = new ImageLabel(ui.scrollArea);
	ui.scrollArea->setWidget(m_imageLabel);
	m_imageLabel->setMouseTracking(true);

	m_imageLabel->m_scale = 1.0;

	this->setMaximumWidth(1600); this->setMinimumWidth(400);
	this->setMaximumHeight(940); this->setMinimumHeight(265);

	ui.horizontalSlider->setRange(0, 0);

	QObject::connect(m_imageLabel, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
	
}

void TKimageDialog::initContent(std::vector<QString> paths)
{
}

void TKimageDialog::setTreshWidget(bool check, int min, int max, uint16_t low, uint16_t up)
{
	ui.treshGroupBox->setChecked(check);
	ui.treshSlider->setRange(min, max);
	ui.treshSlider->setValue(low);

	ui.upTreshSlider->setRange(min, max);
	ui.upTreshSlider->setValue(up);

	ui.lowerTresh->setRange(min, max);
	ui.lowerTresh->setValue(low);

	ui.upperTresh->setRange(min, max);
	ui.upperTresh->setValue(up);

}

bool TKimageDialog::getTresh( uint16_t &lowT, uint16_t &upT )
{
	if (ui.treshGroupBox->isChecked())
	{
		lowT = ui.treshSlider->value();
		upT = ui.upTreshSlider->value();
	}

	return ui.treshGroupBox->isChecked();
}

//void TKimageDialog::addPicture(QString path)
//{
//	//m_obrazki.push_back(QPixmap(path));
//	//ui.horizontalSlider->setMaximum(m_obrazki.size() - 1);
//
//	m_paths.push_back(path);
//	ui.horizontalSlider->setMaximum(m_paths.size() - 1);
//}

void TKimageDialog::addName(QString name)
{
	m_names.push_back(name);
	ui.horizontalSlider->setMaximum(m_names.size() - 1);
}

void TKimageDialog::setSelection(int tx, int ty, int bx, int by)
{
	m_imageLabel->m_sel = QRect( tx, ty, bx - tx, by - ty );
}

void TKimageDialog::setSelection(QRect s)
{
	m_imageLabel->m_sel = QRect(s);
}

QRect TKimageDialog::getSelection()
{
	return m_imageLabel->m_sel;
}


void TKimageDialog::resizeEvent(QResizeEvent*e)
{
	QDialog::resizeEvent(e);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}




QImage tesst(QImage image)
{
	// Retrieve the maximum lightness component
	qint32 maxLightness = 0;
	for (qint32 x = 0, width = image.width(); x < width; x++) {
		for (qint32 y = 0, height = image.height(); y < height; y++) {
			// Get the image pixel's lightness
			qint32 lightness = QColor::fromRgb(image.pixel(x, y)).toHsl().lightness();
			// Compare with the maximum lightness found so far
			maxLightness = qMax(maxLightness, lightness);
		}
	}

	// Create the resulting image
	QRgb resultingRgb = QColor::fromHsl(0, 0, maxLightness).toRgb().rgb();  // The RGB used for the maximum lightness pixels
	// Create an image with the same size as the original
	QImage result(image.width(), image.height(), QImage::Format_RGB888);
	// Fill with white color
	result.fill(Qt::white);
	// Iterate through the pixels of the original image
	for (qint32 x = 0, width = image.width(); x < width; x++) {
		for (qint32 y = 0, height = image.height(); y < height; y++) {
			qint32 lightness = QColor::fromRgb(image.pixel(x, y)).toHsl().lightness();
			// Check if the pixel's lightness is equal to the maximum lightness
			if (lightness == maxLightness)
				result.setPixel(x, y, resultingRgb);  // Set the resulting image's pixel to have the proper gray color
		}
	}
	// Now result (monochrome QImage) contains the pixels with maximum lightness from the original picture

	return result;
}


QImage tesst2(QImage image, uint16_t treshold, uint16_t upTreshold)
{
	int width = image.width();
	int height = image.height();
	int depth = image.depth();
	QImage::Format format = image.format();

	QImage result;
	
	if ((depth == 16) || (depth == 48))
	{
		result = QImage(width, height, QImage::Format::Format_Grayscale16 );
	}
	else
	{
		result = QImage(width, height, QImage::Format::Format_Grayscale8);
	}

	result.fill(Qt::black);

	for (qint32 y = 0; y < height; y++)
	{
		uchar* src = image.scanLine(y);
		uchar* line = result.scanLine(y);

		for (qint32 x = 0; x < width; x++)
		{
			uint32_t lightness = 0;
			if (depth == 16) // grayscale, 16bitów na piksel
			{
				lightness = ((uint16_t*)src)[x];
			}
			else if (depth == 8) // grayscale, 8bitów na piksel
			{
				lightness = src[x];
			}
			else if (depth == 32) // ARGB, 8bitów na kolor
			{
				lightness = src[4 * x];
			}
			else if (depth == 24) // RGB, 8bitów na kolor
			{
				lightness = src[3 * x];
			}
			else if (depth == 48) // RGB, 16bitów na kolor
			{
				lightness = ((uint16_t*)src)[3 * x];
			}

			if ( (lightness >= treshold) && (lightness <= upTreshold))
			{

				if ((depth == 16) || (depth == 48))
				{
					double mult = 65535.0 / (upTreshold - treshold);

					uint16_t c = mult * (lightness - treshold);

					((uint16_t*)line)[x] = c; // lightness;
				}
				else
				{
					double mult = 255.0 / (upTreshold - treshold);

					uchar c = mult * (lightness - treshold);

					line[x] = c; // lightness;
				}
			}


		}
	}

	return result;
}


void TKimageDialog::changeDisplayedPixmap(int i)
{
	//QPixmap pic( m_paths[i] );

	QImage image(m_path+"/"+m_names[i]);
	
	if (ui.treshGroupBox->isChecked())
	{
		image = tesst2(image, ui.treshSlider->value(), ui.upTreshSlider->value());
	}
	
	QPixmap pic = QPixmap::fromImage(image);

	int imgW = pic.width();
	int imgH = pic.height();

	if ( (imgW < this->maximumWidth()) && (imgH < this->maximumHeight()) )
	{
		this->setMaximumWidth(imgW); this->setMinimumWidth(imgW);
		this->setMaximumHeight(imgH+40); this->setMinimumHeight(imgH+40);
		ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

		m_imageLabel->setPixmap(pic);

		m_imageLabel->m_scale = 1.0;
	}
	else if (ui.fitCheckBox->isChecked())
	{
		ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		int w = ui.scrollArea->width();
		int h = ui.scrollArea->height();

		QPixmap p = pic.scaled(w, h, Qt::KeepAspectRatio);
		m_imageLabel->setPixmap(p);

		m_imageLabel->m_scale = (double)p.width() / pic.width();
	}
	else
	{
		ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
		ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
		m_imageLabel->setPixmap(pic);

		m_imageLabel->m_scale = 1.0;
	}
}


void TKimageDialog::onFitCheckBox(int i)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void TKimageDialog::onSelectionChanged()//QRect r)
{
	QRect sel = m_imageLabel->m_sel;
	ui.selGroupBox->setChecked((sel.width() > 0) && (sel.height() > 0));

	ui.selLeft->setValue(sel.left());
	ui.selTop->setValue(sel.top());
	ui.selRight->setValue(sel.right());
	ui.selBottom->setValue(sel.bottom());
}

void TKimageDialog::onSelLeftEdited(int l)
{
	if ( (l>0) && (l<m_imageLabel->m_sel.right()))
		m_imageLabel->m_sel.setLeft(l);
}

void TKimageDialog::onSelRightEdited(int r)
{
	if (r > m_imageLabel->m_sel.left())
		m_imageLabel->m_sel.setRight(r);
}

void TKimageDialog::onSelTopEdited(int t)
{
	if ((t > 0) && (t < m_imageLabel->m_sel.bottom()))
		m_imageLabel->m_sel.setTop(t);
}

void TKimageDialog::onSelBottomEdited(int b)
{
	if (b > m_imageLabel->m_sel.top())
		m_imageLabel->m_sel.setBottom(b);
}



void TKimageDialog::onTreshCheckBox(int i)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void TKimageDialog::onTreshValueChanged(int i)
{
	ui.lowerTresh->blockSignals(true);
	ui.lowerTresh->setValue(i);
	ui.lowerTresh->blockSignals(false);

	ui.upperTresh->setMinimum(i + 1);
	ui.upTreshSlider->setMinimum(i + 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void TKimageDialog::onLowerTreshEdited(int i)
{
	ui.treshSlider->blockSignals(true);
	ui.treshSlider->setValue(i);
	ui.treshSlider->blockSignals(false);

	ui.upperTresh->setMinimum(i + 1);
	ui.upTreshSlider->setMinimum(i + 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}



void TKimageDialog::onUpperTreshValueChanged(int i)
{
	ui.upperTresh->blockSignals(true);
	ui.upperTresh->setValue(i);
	ui.upperTresh->blockSignals(false);

	ui.lowerTresh->setMaximum(i - 1);
	ui.treshSlider->setMaximum(i - 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void TKimageDialog::onUpperTreshEdited(int i)
{
	ui.upTreshSlider->blockSignals(true);
	ui.upTreshSlider->setValue(i);
	ui.upTreshSlider->blockSignals(false);

	ui.lowerTresh->setMaximum(i - 1);
	ui.treshSlider->setMaximum(i - 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void TKimageDialog::onSliderValueChanged(int i)
{
	changeDisplayedPixmap(i);
}

