#include "VolTKimageDialog.h"


//#include <QResizeEvent>

#include "ImageLabel.h"


void DPVISION_EXPORT testVolTKimageDialog(CVolTK* volTK)
{
	VolTKimageDialog* dialog = new VolTKimageDialog(volTK);

	//Ui_TKimageDialog& pmUi = dialog->getUI();


	//dialog->setPath(plikSiatki.absolutePath());
	//for (int j = min; j <= max; j++)
	//{
	//	QString qname = QString::asprintf("/%s%03d.%s", prefix, j, ext);
	//	if (CFileInfo(plikSiatki.absolutePath() + qname).exists())
	//	{
	//		dialog->addName(qname);
	//		pmUi.minCombo->addItem(qname, QVariant(j));
	//		pmUi.maxCombo->addItem(qname, QVariant(j));
	//	}
	//}

	//pmUi.minCombo->setCurrentIndex(0);
	//pmUi.maxCombo->setCurrentIndex(max - min);


	//pmUi.fileInfoLabel->setText(QString::asprintf("Image format is %s (%d x %d x %d bits)", ext, w, h, depth));


	//if (!is16bit)
	//{
	//	if (lowTreshold > 255)
	//	{
	//		lowTreshold >>= 8;
	//	}

	//	if (upTreshold > 255)
	//	{
	//		upTreshold >>= 8;
	//	}
	//}

	//if ((lowTreshold > 0) || (upTreshold < (is16bit ? 0xFFFF : 0xFF)))
	//{
	//	dialog->setTreshWidget(true, 0, is16bit ? 0xFFFF : 0xFF, lowTreshold, upTreshold);
	//}
	//else
	//{
	//	dialog->setTreshWidget(false, 0, is16bit ? 0xFFFF : 0xFF, lowTreshold, upTreshold);
	//}


	//pmUi.VoxXedit->setText(QString::number(pxx));
	//pmUi.VoxYedit->setText(QString::number(pxy));
	//pmUi.VoxZedit->setText(QString::number(pxz));


	//dialog->setSelection(m_winTX, m_winTY, m_winBX, m_winBY);

	//dialog->m_is16bit = is16bit;

	if (dialog->exec())
	{

	}
}

///////////////////////////////////////////////////////////////////////////////////////////
// ImageDialog
///////////////////////////////////////////////////////////////////////////////////////////

VolTKimageDialog::VolTKimageDialog(CVolTK* volTK, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_VolTK(volTK)
{
	ui.setupUi(this);

	m_imageLabel = new ImageLabel(ui.scrollArea);
	ui.scrollArea->setWidget(m_imageLabel);
	m_imageLabel->setMouseTracking(true);

	m_imageLabel->m_scale = 1.0;

	this->setMaximumWidth(1600); this->setMinimumWidth(400);
	this->setMaximumHeight(940); this->setMinimumHeight(265);

	currentView = CVolTK::Layer::XY;
	dispBits = 16;
	setTreshWidget(false, 0, 0xffff, 0, 0xffff);

	ui.horizontalSlider->setRange(0, m_VolTK->kostka.m_lays-1);


	QObject::connect(m_imageLabel, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
	
}

void VolTKimageDialog::initContent(std::vector<QString> paths)
{
}

void VolTKimageDialog::setTreshWidget(bool check, int min, int max, uint16_t low, uint16_t up)
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

bool VolTKimageDialog::getTresh( uint16_t &lowT, uint16_t &upT )
{
	if (ui.treshGroupBox->isChecked())
	{
		lowT = ui.treshSlider->value();
		upT = ui.upTreshSlider->value();
	}

	return ui.treshGroupBox->isChecked();
}

//void VolTKimageDialog::addPicture(QString path)
//{
//	//m_obrazki.push_back(QPixmap(path));
//	//ui.horizontalSlider->setMaximum(m_obrazki.size() - 1);
//
//	m_paths.push_back(path);
//	ui.horizontalSlider->setMaximum(m_paths.size() - 1);
//}

void VolTKimageDialog::addName(QString name)
{
	m_names.push_back(name);
	ui.horizontalSlider->setMaximum(m_names.size() - 1);
}

void VolTKimageDialog::setSelection(int tx, int ty, int bx, int by)
{
	m_imageLabel->m_sel = QRectF( tx, ty, bx - tx, by - ty );
}

void VolTKimageDialog::setSelection(QRect s)
{
	m_imageLabel->m_sel = QRectF(s);
}

QRect VolTKimageDialog::getSelection()
{
	return m_imageLabel->m_sel.toRect();
}


void VolTKimageDialog::resizeEvent(QResizeEvent*e)
{
	QDialog::resizeEvent(e);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}




QImage tesst3(QImage image)
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


QImage tesst4(QImage image, uint16_t treshold, uint16_t upTreshold)
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
			else if (lightness > upTreshold)
			{
				if ((depth == 16) || (depth == 48))
				{
					((uint16_t*)line)[x] = 0xffff;
				}
				else
				{
					line[x] = 0xff;
				}
			}
		}
	}

	return result;
}

//dla algorytmu Hoar'e - obliczanie mediany
int partition(int* c, int a, int b)
{
	int e, tmp;
	a = a;
	b = b;
	e = c[a];        //elemennt dzielacy
	while (a < b)
	{
		while ((a < b) && (c[b] >= e)) b--;
		while ((a < b) && (c[a] < e)) a++;
		if (a < b)
		{
			tmp = c[a];
			c[a] = c[b];
			c[b] = tmp;
		}
	}
	return a;
}

//algorytmu Hoar'e - obliczanie mediany
int med(int* c, int size)
{
	//algorytm Hoare'a
	int i = 0;
	int j = size - 1;
	int w = j / 2;
	int k;
	while (i != j)
	{
		k = partition(c, i, j);
		k = k - i + 1;
		if (k >= w)
			j = i + k - 1;
		if (k < w)
		{
			w -= k;
			i += k;
		}
	}
	return c[i];
}

QImage doMedianFilter(QImage srcImg, int size)
{
	QImage dstImg(srcImg.width(), srcImg.height(), QImage::Format::Format_Grayscale16);
	dstImg.fill(Qt::black);

	int* srcVal = new int[size * size];

	for (qint32 y = 0; y < (srcImg.height() - (size-1)); y++)
	{
		std::vector<uchar*> line(size);
		
		for (int s=0;s<size;s++) line[s] = srcImg.scanLine(y+s);

		uchar* dstLine = dstImg.scanLine(y+(size/2));

		for (qint32 x = 0; x < (srcImg.width() - (size-1)); x++)
		{

			for (int sy = 0; sy < size; sy++)
				for (int sx = 0; sx < size; sx++)
					srcVal[sy*size+sx] = ((uint16_t*)line[sy])[x + sx];
					//srcVal.push_back(((uint16_t*)line[sy])[x + sx]);

			int mediana = med(srcVal, size * size);

			((uint16_t*)dstLine)[x+1] = round(mediana);
		}
	}

	delete[] srcVal;

	return dstImg;
}


void VolTKimageDialog::changeDisplayedPixmap(int i)
{
	changeDisplayedPixmap(m_VolTK->getLayerAsImage(i, currentView, dispBits));
}

void VolTKimageDialog::changeDisplayedPixmap(QImage image)
{
	if (ui.medianGroupBox->isChecked())
	{
		image = doMedianFilter(image, ui.medianFilterSizeSpinBox->value());
	}

	if (ui.treshGroupBox->isChecked())
	{
		image = tesst4(image, ui.treshSlider->value(), ui.upTreshSlider->value());
	}

	QPixmap pic = QPixmap::fromImage(image);

	int imgW = pic.width();
	int imgH = pic.height();

	if ((imgW < this->maximumWidth()) && (imgH < this->maximumHeight()))
	{
		this->setMaximumWidth(imgW); this->setMinimumWidth(imgW);
		this->setMaximumHeight(imgH + 40); this->setMinimumHeight(imgH + 40);
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

		m_imageLabel->m_scale = (double)pic.width() / p.width();
	}
	else
	{
		ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
		ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
		m_imageLabel->setPixmap(pic);

		m_imageLabel->m_scale = 1.0;
	}
}


void VolTKimageDialog::onFitCheckBox(int i)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolTKimageDialog::onSelectionChanged()//QRect r)
{
	QRect sel = m_imageLabel->m_sel.toRect();
	ui.selGroupBox->setChecked((sel.width() > 0) && (sel.height() > 0));

	ui.selLeft->setValue(sel.left());
	ui.selTop->setValue(sel.top());
	ui.selRight->setValue(sel.right());
	ui.selBottom->setValue(sel.bottom());
}

void VolTKimageDialog::onSelLeftEdited(int l)
{
	if ( (l>0) && (l<m_imageLabel->m_sel.right()))
		m_imageLabel->m_sel.setLeft(l);
}

void VolTKimageDialog::onSelRightEdited(int r)
{
	if (r > m_imageLabel->m_sel.left())
		m_imageLabel->m_sel.setRight(r);
}

void VolTKimageDialog::onSelTopEdited(int t)
{
	if ((t > 0) && (t < m_imageLabel->m_sel.bottom()))
		m_imageLabel->m_sel.setTop(t);
}

void VolTKimageDialog::onSelBottomEdited(int b)
{
	if (b > m_imageLabel->m_sel.top())
		m_imageLabel->m_sel.setBottom(b);
}



void VolTKimageDialog::onTreshCheckBox(int i)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolTKimageDialog::onTreshValueChanged(int i)
{
	ui.lowerTresh->blockSignals(true);
	ui.lowerTresh->setValue(i);
	ui.lowerTresh->blockSignals(false);

	ui.upperTresh->setMinimum(i + 1);
	ui.upTreshSlider->setMinimum(i + 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolTKimageDialog::onLowerTreshEdited(int i)
{
	ui.treshSlider->blockSignals(true);
	ui.treshSlider->setValue(i);
	ui.treshSlider->blockSignals(false);

	ui.upperTresh->setMinimum(i + 1);
	ui.upTreshSlider->setMinimum(i + 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}



void VolTKimageDialog::onUpperTreshValueChanged(int i)
{
	ui.upperTresh->blockSignals(true);
	ui.upperTresh->setValue(i);
	ui.upperTresh->blockSignals(false);

	ui.lowerTresh->setMaximum(i - 1);
	ui.treshSlider->setMaximum(i - 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolTKimageDialog::onUpperTreshEdited(int i)
{
	ui.upTreshSlider->blockSignals(true);
	ui.upTreshSlider->setValue(i);
	ui.upTreshSlider->blockSignals(false);

	ui.lowerTresh->setMaximum(i - 1);
	ui.treshSlider->setMaximum(i - 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolTKimageDialog::onViewPlaneChanged(QString val)
{
	if (val == "XY")
	{
		currentView = CVolTK::Layer::XY;

		ui.horizontalSlider->setRange(0, m_VolTK->kostka.m_lays - 1);
	}
	else if (val == "YZ")
	{
		currentView = CVolTK::Layer::YZ;

		ui.horizontalSlider->setRange(0, m_VolTK->kostka.m_cols - 1);
	}
	else if (val == "ZX")
	{
		currentView = CVolTK::Layer::ZX;

		ui.horizontalSlider->setRange(0, m_VolTK->kostka.m_rows - 1);
	}
	ui.horizontalSlider->setValue(0);
	changeDisplayedPixmap(0);
}

void VolTKimageDialog::onDispBitsChanged(QString val)
{
	if (val == "8bit")
	{
		dispBits = 8;
		setTreshWidget(false, 0, 0xff, 0, 0xff);
	}
	else if (val == "12bit")
	{
		dispBits = 12;
		setTreshWidget(false, 0, 0xfff, 0, 0xfff);
	}
	else if (val == "16bit")
	{
		dispBits = 16;
		setTreshWidget(false, 0, 0xffff, 0, 0xffff);
	}
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolTKimageDialog::onSliderValueChanged(int i)
{
	changeDisplayedPixmap(i);
}



QImage VolTKimageDialog::aplyConvFilter(QImage srcImg, Filter fltr)
{
	//QImage dstImg = srcImg.copy();
	
	QImage dstImg(srcImg.width(), srcImg.height(), QImage::Format::Format_Grayscale16);
	dstImg.fill(Qt::black);

	for (qint32 y = 1; y < (srcImg.height()-1); y++)
	{
		uchar* line[3];
		line[0] = srcImg.scanLine(y - 1);
		line[1] = srcImg.scanLine(y);
		line[2] = srcImg.scanLine(y + 1);

		uchar* dstLine = dstImg.scanLine(y);
		
		for (qint32 x = 1; x < (srcImg.width()-1); x++)
		{
			double srcVal[3][3];

			srcVal[0][0] = ((uint16_t*)line[0])[x - 1];
			srcVal[0][1] = ((uint16_t*)line[0])[x];
			srcVal[0][2] = ((uint16_t*)line[0])[x + 1];

			srcVal[1][0] = ((uint16_t*)line[1])[x - 1];
			srcVal[1][1] = ((uint16_t*)line[1])[x];
			srcVal[1][2] = ((uint16_t*)line[1])[x + 1];

			srcVal[2][0] = ((uint16_t*)line[2])[x - 1];
			srcVal[2][1] = ((uint16_t*)line[2])[x];
			srcVal[2][2] = ((uint16_t*)line[2])[x + 1];

			double temp = 0.0;
			double sum = 0.0;
			for (int yy = 0; yy < 3; yy++)
			{
				for (int xx = 0; xx < 3; xx++)
				{
					sum += fltr(yy, xx);
					temp += srcVal[yy][xx] * fltr(yy, xx);
				}
			}

			((uint16_t*)dstLine)[x] = round(temp / sum);
		}
	}

	return dstImg;
}


void VolTKimageDialog::onConvFilterBtn()
{
	Filter filter;

	FilterDialog* dlg = new FilterDialog(&filter);

	if (dlg->exec())
	{
		QImage image = aplyConvFilter(m_VolTK->getLayerAsImage(ui.horizontalSlider->value(), currentView, dispBits), filter);
		
		changeDisplayedPixmap(image);
	}
}





int mediana(NowaKostka1& kostka, size_t destAdress, size_t cols, int size)
{
	int halfSize = size / 2;
	size_t srcAdress = destAdress - halfSize;

	int srcVal[81]; //array for maximal size 9
	
	size_t srcRowAdress = srcAdress - cols * halfSize;

	for (int srow = 0; srow < size; srow++)
	{
		size_t srcCellAdress = srcRowAdress + srow;
		for (int scol = 0; scol < size; scol++)
		{
			srcVal[srow * size + scol] = ((NowaKostka1&)kostka).get(srcCellAdress);
			srcCellAdress++;
		}

		srcRowAdress += cols;
	}
	return med(srcVal, size*size);
}

int VolTKimageDialog::mediana0(NowaKostka1& kostka, size_t dest0adress, int filterSize)
{
	size_t srcRowAdress = dest0adress;

	int* srcVal = new int[filterSize*filterSize]; //array for maximal size 9
	int fastIdx = 0;
	for (int srow = 0; srow < filterSize; srow++)
	{
		size_t srcCellAdress = srcRowAdress + srow;
		for (int scol = 0; scol < filterSize; scol++)
		{
			//srcVal[srow * filterSize + scol] = ((NowaKostka1&)kostka).get(srcCellAdress);
			srcVal[fastIdx] = kostka.get(srcCellAdress);
			fastIdx++;
			srcCellAdress++;
		}

		srcRowAdress += kostka.m_cols;
	}

	int result = med(srcVal, filterSize * filterSize);
	delete[] srcVal;
	
	return result;
}


void VolTKimageDialog::do3dMedianFilter(NowaKostka1& src, NowaKostka1&dst, int size)
{
	UI::PROGRESSBAR::init(0, src.m_lays, 0);
	UI::PROGRESSBAR::setText("3d data filtering: ");

	size_t destOffset2 = src.m_cols * (size / 2) + (size / 2);
	
	bool setTresh = ui.treshGroupBox->isChecked();
	int lowTresh = ui.treshSlider->value();
	int upTresh = ui.upTreshSlider->value();

	for (size_t layer = 0; layer < src.m_lays; layer++)
	{
		if (!(layer%10)) UI::PROGRESSBAR::setValue(layer);
		size_t layOffset = src.m_rows * src.m_cols * layer;
//		#pragma loop(hint_parallel(0))
		for (size_t row = 0; row < (src.m_rows - (size - 1)); row++)
		{
			size_t rowOffset = src.m_cols * row;
			for (size_t col = 0; col < (src.m_cols - (size - 1)); col++)
			{
				size_t currentFilter0adress = layOffset + rowOffset + col;
				size_t currentDestAdress = currentFilter0adress + destOffset2;

				int newValue = mediana0(src, currentFilter0adress, size);
				
				//if (setTresh)
				//{
				//	if (newValue < lowTresh)
				//	{
				//		newValue = 0;
				//	}
				//	else if (newValue > upTresh)
				//	{
				//		newValue = 0;
				//	}
				//}

				//dst.set(currentDestAdress, newValue);
				dst.set(col+(size/2),row+(size/2),layer, newValue);
			}
		}
	}
	UI::PROGRESSBAR::hide();
}


#include "AP.h"

void VolTKimageDialog::onCreateVolTKStructureBtn()
{
	CVolTK* nowy = new CVolTK(*m_VolTK);

	if (ui.medianGroupBox->isChecked())
	{
		nowy->kostka.setAllData(0);
		do3dMedianFilter(m_VolTK->kostka, nowy->kostka, ui.medianFilterSizeSpinBox->value());
	}

	int lowTresh = ui.treshSlider->value();
	int upTresh = ui.upTreshSlider->value();

	if (ui.treshGroupBox->isChecked())
	{
		for (size_t l = 0; l < nowy->kostka.m_lays; l++)
			for (size_t r = 0; r < nowy->kostka.m_rows; r++)
				for (size_t c = 0; c < nowy->kostka.m_cols; c++)
				{
					int val = nowy->kostka.get(c, r, l);

					if (val < lowTresh)
					{
						nowy->kostka.set(c, r, l, 0);
					}
					else if (val > upTresh)
					{
						nowy->kostka.set(c, r, l, 0xffff);
					}
					else
					{
						nowy->kostka.set(c, r, l, val);
					}
				}
	}

	nowy->createDisplayData();
	nowy->setGeometry();

	AP::OBJECT::addChild(m_VolTK->getParent(), nowy);

	accept();
}