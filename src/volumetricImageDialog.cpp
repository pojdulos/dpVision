#include "volumetricImageDialog.h"


//#include <QResizeEvent>

#include "ImageLabel.h"
#include "UI.h"
#include <QKeyEvent>
#include <QPainter>

#include <omp.h>

///////////////////////////////////////////////////////////////////////////////////////////
// ImageDialog
///////////////////////////////////////////////////////////////////////////////////////////

VolumetricImageDialog::VolumetricImageDialog(Volumetric* volTK, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_VolTK(volTK)
{
	ui.setupUi(this);

	this->setMouseTracking(false);

	this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint); // | Qt::WindowMinimizeButtonHint);

	m_imageLabel = new ImageLabel(QSize(volTK->columns(), volTK->rows()), ui.scrollArea);

	m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_imageLabel->setMouseTracking(true);
	m_imageLabel->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
	m_imageLabel->setScaledContents(true);

	ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	ui.scrollArea->setWidget(m_imageLabel);

	currentView = Volumetric::LayerPlane::XY;
	
	resize_masks();
	
	current_i = 0;
	common_mask = true;

	qInfo() << "LAYERS: " << m_VolTK->layers();
	ui.horizontalSlider->setRange(0, m_VolTK->layers() - 1);
	dispBits = 16;

	setTreshWidget(false, 0, 0xffff, 0, 0xffff);

	free_norm = CVector3f(0.0f, 0.0f, 1.0f).getNormalized();
	free_up = CVector3f(0.0f, 1.0f, 0.0f).getNormalized();
	free_origin = CPoint3f(m_VolTK->columns() / 2, m_VolTK->rows() / 2, m_VolTK->layers() / 2);

	ui.orig_x->setValue(free_origin.x);
	ui.orig_y->setValue(free_origin.y);
	ui.orig_z->setValue(free_origin.z);

	ui.norm_x->setValue(free_norm.x);
	ui.norm_y->setValue(free_norm.y);
	ui.norm_z->setValue(free_norm.z);

	ui.free_plane->setEnabled(false);

	ui.displ_min->setValue(m_VolTK->m_minDisplWin);
	ui.displ_max->setValue(m_VolTK->m_maxDisplWin);

	ui.displ_min->setMinimum(m_VolTK->m_minVal);
	ui.displ_min->setMaximum(m_VolTK->m_maxDisplWin);

	ui.displ_max->setMinimum(m_VolTK->m_minDisplWin);
	ui.displ_max->setMaximum(m_VolTK->m_maxVal);

	m_imageLabel->m_scale = 1.0;

	QObject::connect(m_imageLabel, &ImageLabel::scaleChanged, this, &VolumetricImageDialog::onImageScaleChanged);
	QObject::connect(m_imageLabel, &ImageLabel::mousePressed, this, &VolumetricImageDialog::onImageMousePressed);
	QObject::connect(m_imageLabel, &ImageLabel::mouseMoved, this, &VolumetricImageDialog::onImageMouseMoved);
	QObject::connect(m_imageLabel, &ImageLabel::selectionChanged, this, &VolumetricImageDialog::onSelectionChanged);
	
	// m_imageLabel->m_work_mode == Select
	ui.a_btn->setChecked(true);

	QObject::connect(ui.a_btn, &QToolButton::toggled, this, &VolumetricImageDialog::on_A_btn_toggled);
	QObject::connect(ui.b_btn, &QToolButton::toggled, this, &VolumetricImageDialog::on_B_btn_toggled);
	QObject::connect(ui.c_btn, &QToolButton::toggled, this, &VolumetricImageDialog::on_C_btn_toggled);

	QObject::connect(ui.hide_all_above_upper, &QCheckBox::toggled, this, &VolumetricImageDialog::on_hide_all_above_upper_toggled);


	ui.horizontalSlider->setMaximum(m_VolTK->layers() - 1);
}

void VolumetricImageDialog::initContent(std::vector<QString> paths)
{
}


//QImage bitmap_to_image2(const QBitmap& bitmap)
//{
//	QImage image(bitmap.size(), QImage::Format_ARGB32);
//	image.fill(Qt::GlobalColor::transparent);
//
//	QPainter painter(&image);
//	painter.setPen(Qt::NoPen);
//	painter.setBrush(QColor(0, 255, 255));
//
//	painter.drawPixmap(0, 0, QPixmap::fromImage(bitmap.toImage()));
//	painter.end();
//
//	return image;
//}


void VolumetricImageDialog::on_del_in_select_mode(Qt::KeyboardModifiers modifiers)
{
	QRect& sel = m_imageLabel->m_selection;

	if (sel.isValid()) {
		qInfo() << "ERASING Rectangle selection: " << m_imageLabel->m_selection << Qt::endl;

		int slice = ui.horizontalSlider->value();

		switch (currentView)
		{
		case Volumetric::LayerPlane::XY:
		{
			auto fn_XY = [&](int layer)
				{
#pragma omp parallel for
					for (int y = sel.top(); y <= sel.bottom(); y++)
					{
						int row = std::max(0, std::min(y, (int)m_VolTK->rows() - 1));
						for (unsigned int x = sel.left(); x <= sel.right(); x++)
						{
							int column = std::max((unsigned int)0, std::min(x, m_VolTK->columns() - 1));
							(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
						}
					}
				};

			auto fn_XY_rev = [&](int layer)
				{
					int maxY = m_VolTK->rows(); // y == row
					int maxX = m_VolTK->columns(); // x == column

#pragma omp parallel for
					for (int y = 0; y < maxY; y++)
					{
						int row = y;
						for (int x = 0; x < maxX; x++)
						{
							if (y < sel.top() || y > sel.bottom() || x < sel.left() || x > sel.right())
							{
								int column = x;
								(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
							}
						}
					}
				};


			if (modifiers & Qt::ControlModifier)
			{
				for (int layer = 0; layer < m_VolTK->layers(); layer++)
				{
					if (modifiers & Qt::ShiftModifier)
						fn_XY_rev(layer);
					else
						fn_XY(layer);
				}
			}
			else
			{
				if (modifiers & Qt::ShiftModifier)
					fn_XY_rev(slice);
				else
					fn_XY(slice);
			}
		}
		break;
		case Volumetric::LayerPlane::YZ:
		{
			auto fn_YZ = [&](int column)
				{
#pragma omp parallel for
					for (int y = sel.top(); y <= sel.bottom(); y++)
					{
						int row = std::max(0, std::min(y, (int)m_VolTK->rows() - 1));
						for (unsigned int x = sel.left(); x <= sel.right(); x++)
						{
							int layer = std::max((unsigned int)0, std::min(x, m_VolTK->layers() - 1));
							(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
						}
					}
				};

			auto fn_YZ_rev = [&](int column)
				{
					int maxY = m_VolTK->rows(); // y == row
					int maxX = m_VolTK->layers(); // x == layer

#pragma omp parallel for
					for (int y = 0; y < maxY; y++)
					{
						int row = y;
						for (int x = 0; x < maxX; x++)
						{
							if (y < sel.top() || y > sel.bottom() || x < sel.left() || x > sel.right())
							{
								int layer = x;
								(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
							}
						}
					}
				};

			if (modifiers == Qt::ControlModifier)
			{
				for (int col = 0; col < m_VolTK->columns(); col++)
				{
					if (modifiers & Qt::ShiftModifier)
						fn_YZ_rev(col);
					else
						fn_YZ(col);
				}
			}
			else
			{
				if (modifiers & Qt::ShiftModifier)
					fn_YZ_rev(slice);
				else
					fn_YZ(slice);
			}
		}
		break;
		case Volumetric::LayerPlane::ZX:
		{
			auto fn_ZX = [&](int row)
				{
#pragma omp parallel for
					for (int y = sel.top(); y <= sel.bottom(); y++)
					{
						int layer = std::max(0, std::min(y, (int)m_VolTK->layers() - 1));
						for (unsigned int x = sel.left(); x <= sel.right(); x++)
						{
							int column = std::max((unsigned int)0, std::min(x, m_VolTK->columns() - 1));
							(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
						}
					}
				};

			auto fn_ZX_rev = [&](int row)
				{
					int maxY = m_VolTK->layers(); // y == layer
					int maxX = m_VolTK->columns(); // x == column

#pragma omp parallel for
					for (int y = 0; y < maxY; y++)
					{
						int layer = y;
						for (int x = 0; x < maxX; x++)
						{
							if (y < sel.top() || y > sel.bottom() || x < sel.left() || x > sel.right())
							{
								int column = x;
								(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
							}
						}
					}
				};

			if (modifiers == Qt::ControlModifier)
			{
				for (int row = 0; row < m_VolTK->rows(); row++)
				{
					if (modifiers & Qt::ShiftModifier)
						fn_ZX_rev(row);
					else
						fn_ZX(row);
				}
			}
			else
			{
				if (modifiers & Qt::ShiftModifier)
					fn_ZX_rev(slice);
				else
					fn_ZX(slice);
			}
		}
		break;
		default:
			UI::MESSAGEBOX::information("Function is implemented for XY view only");
			break;
		}
		changeDisplayedPixmap(slice);
	}
	else {
		qInfo() << "NONE SELECTED" << Qt::endl;
	}
}

void VolumetricImageDialog::on_del_in_eraser_mode(Qt::KeyboardModifiers modifiers)
{
	int slice = ui.horizontalSlider->value();
	if (!common_mask && (modifiers & Qt::ControlModifier))
	{
		m_masks[current_i] = m_imageLabel->mask_to_bitmap();
	}

	if (currentView == Volumetric::LayerPlane::XY)
	{
		auto fn_XY = [&](int layer, bool rev)
			{
				int maxY = m_VolTK->rows(); // y == row
				int maxX = m_VolTK->columns(); // x == column

				QImage mask = common_mask ? m_imageLabel->mask_ref() : ImageLabel::mask_bitmap_to_image(m_masks[layer]);

#pragma omp parallel for
				for (int y = 0; y < maxY; y++)
				{
					int row = y;
					for (int x = 0; x < maxX; x++)
					{
						int column = x;
						if ((rev && qGray(mask.pixel(x, y)) == 0) || (!rev && qGray(mask.pixel(x, y)) != 0))
						{
							(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
						}
					}
				}
			};

		if (modifiers & Qt::ControlModifier)
		{
			for (int layer = 0; layer < m_VolTK->layers(); layer++)
			{
				fn_XY(layer, (modifiers & Qt::ShiftModifier));
			}
		}
		else
		{
			fn_XY(slice, (modifiers & Qt::ShiftModifier));
		}
	}
	else if (currentView == Volumetric::LayerPlane::YZ)
	{
		auto fn_YZ = [&](int column, bool rev)
			{
				int maxY = m_VolTK->rows(); // y == row
				int maxX = m_VolTK->layers(); // x == layer

				QImage mask = common_mask ? m_imageLabel->mask_ref() : ImageLabel::mask_bitmap_to_image(m_masks[column]);

#pragma omp parallel for
				for (int y = 0; y < maxY; y++)
				{
					int row = y;
					for (int x = 0; x < maxX; x++)
					{
						int layer = x;
						if ((rev && qGray(mask.pixel(x, y)) == 0) || (!rev && qGray(mask.pixel(x, y)) != 0))
						{
							(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
						}
					}
				}
			};

		if (modifiers & Qt::ControlModifier)
		{
			for (int column = 0; column < m_VolTK->columns(); column++)
			{
				fn_YZ(column, (modifiers & Qt::ShiftModifier));
			}
		}
		else
		{
			fn_YZ(slice, (modifiers & Qt::ShiftModifier));
		}
	}
	else if (currentView == Volumetric::LayerPlane::ZX)
	{
		auto fn_ZX = [&](int row, bool rev)
			{
				int maxY = m_VolTK->layers(); // y == layer
				int maxX = m_VolTK->columns(); // x == column

				QImage mask = common_mask ? m_imageLabel->mask_ref() : ImageLabel::mask_bitmap_to_image(m_masks[row]);

#pragma omp parallel for
				for (int y = 0; y < maxY; y++)
				{
					int layer = y;
					for (int x = 0; x < maxX; x++)
					{
						int column = x;
						if ((rev && qGray(mask.pixel(x, y)) == 0) || (!rev && qGray(mask.pixel(x, y)) != 0))
						{
							(*m_VolTK)[layer][row * m_VolTK->columns() + column] = m_VolTK->m_minVal;
						}
					}
				}
			};

		if (modifiers & Qt::ControlModifier)
		{
			for (int row = 0; row < m_VolTK->rows(); row++)
			{
				fn_ZX(row, (modifiers & Qt::ShiftModifier));
			}
		}
		else
		{
			fn_ZX(slice, (modifiers & Qt::ShiftModifier));
		}
	}

	if (!common_mask && (modifiers & Qt::ControlModifier))
	{
		clear_masks();
	}

	changeDisplayedPixmap(slice);
}

void VolumetricImageDialog::onImageKeyPressed(QKeyEvent* e)
{
	//qInfo() << "ON IMAGE KEY PRESSED " << e->key() << Qt::endl;
	if (m_imageLabel->m_work_mode == ImageLabel::WorkMode::WorkModeSelect)
	{
		if (e->key() == Qt::Key::Key_Delete)
		{
			on_del_in_select_mode(e->modifiers());
		}
	}
	else if (m_imageLabel->m_work_mode == ImageLabel::WorkMode::WorkModeErase)
	{
		if ( e->key() == Qt::Key::Key_Delete )
		{
			on_del_in_eraser_mode(e->modifiers());
		}
		else if ((e->modifiers() & Qt::ControlModifier) && (e->key() == Qt::Key::Key_1))
		{
			if ((!common_mask) && (current_i > ui.horizontalSlider->minimum()))
			{
				m_imageLabel->mask_set(m_masks[current_i - 1]);
			}
		}
		else if ((e->modifiers() & Qt::ControlModifier) && (e->key() == Qt::Key::Key_2))
		{
			if ((!common_mask) && (current_i < ui.horizontalSlider->maximum()))
			{
				m_imageLabel->mask_set(m_masks[current_i + 1]);
			}
		}
	}
	UI::updateAllViews();
}

void VolumetricImageDialog::setTreshWidget(bool check, int min, int max, Volumetric::VoxelType low, Volumetric::VoxelType up)
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

bool VolumetricImageDialog::getTresh(Volumetric::VoxelType &lowT, Volumetric::VoxelType &upT )
{
	if (ui.treshGroupBox->isChecked())
	{
		lowT = ui.treshSlider->value();
		upT = ui.upTreshSlider->value();
	}

	return ui.treshGroupBox->isChecked();
}

void VolumetricImageDialog::test_dialog(Volumetric* volum)
{
	VolumetricImageDialog* dialog = new VolumetricImageDialog(volum);

	dialog->show();
}


QMenu* VolumetricImageDialog::createLabelMenu()
{
	QMenu* menu = new QMenu("TEST");

	menu->addAction("item1", this, &VolumetricImageDialog::test_slot);
	menu->addAction("item2", this, &VolumetricImageDialog::test_slot);
	menu->addAction("item3", this, &VolumetricImageDialog::test_slot);

	return menu;
}



//void VolTKimageDialog::addPicture(QString path)
//{
//	//m_obrazki.push_back(QPixmap(path));
//	//ui.horizontalSlider->setMaximum(m_obrazki.size() - 1);
//
//	m_paths.push_back(path);
//	ui.horizontalSlider->setMaximum(m_paths.size() - 1);
//}

void VolumetricImageDialog::addName(QString name)
{
	m_names.push_back(name);
	ui.horizontalSlider->setMaximum(m_names.size() - 1);
}

void VolumetricImageDialog::setSelection(int tx, int ty, int bx, int by)
{
	m_imageLabel->m_selection = QRect( tx, ty, bx - tx, by - ty );
}

void VolumetricImageDialog::setSelection(QRect s)
{
	m_imageLabel->m_selection = QRect(s);
}

QRect VolumetricImageDialog::getSelection()
{
	return m_imageLabel->m_selection;
}


void VolumetricImageDialog::resizeEvent(QResizeEvent*e)
{
	QDialog::resizeEvent(e);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::mousePressEvent(QMouseEvent* e)
{
	qInfo() << "mousePressEvent";
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


QImage tesst4(QImage image, uint16_t treshold, uint16_t upTreshold, bool draw_upper)
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
			if (depth == 16) // grayscale, 16bit�w na piksel
			{
				lightness = ((uint16_t*)src)[x];
			}
			else if (depth == 8) // grayscale, 8bit�w na piksel
			{
				lightness = src[x];
			}
			else if (depth == 32) // ARGB, 8bit�w na kolor
			{
				lightness = src[4 * x];
			}
			else if (depth == 24) // RGB, 8bit�w na kolor
			{
				lightness = src[3 * x];
			}
			else if (depth == 48) // RGB, 16bit�w na kolor
			{
				lightness = ((uint16_t*)src)[3 * x];
			}

			if ((lightness >= treshold) && (lightness <= upTreshold))
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
			else if (draw_upper && (lightness > upTreshold))
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


// Funkcja obliczająca wymiary obrazu rzutowanego oraz jego głębokość
CPoint3f VolumetricImageDialog::calculateProjectionDimensions(const CPoint3f& orig, const CVector3f& norm, const CVector3f& up,
	float columns, float rows, float layers) {
	
	// Wektory pomocnicze do przekształcenia współrzędnych
	CVector3f right = up.crossProduct(norm).getNormalized();
	CVector3f newUp = norm.crossProduct(right).getNormalized();

	// Współrzędne wierzchołków kostki
	std::vector<CPoint3f> vertices = {
		{0.0f, 0.0f, 0.0f},
		{columns, 0.0f, 0.0f},
		{columns, rows, 0.0f},
		{0.0f, rows, 0.0f},
		{0.0f, 0.0f, layers},
		{columns, 0.0f, layers},
		{columns, rows, layers},
		{0.0f, rows, layers}
	};

	// Transformacja współrzędnych wierzchołków do układu płaszczyzny rzutowania
	std::vector<CPoint3f> projectedVertices;
	for (const auto& vertex : vertices) {
		CPoint3f translated = { vertex.x - orig.x, vertex.y - orig.y, vertex.z - orig.z };
		float x = translated.x * right.x + translated.y * right.y + translated.z * right.z;
		float y = translated.x * newUp.x + translated.y * newUp.y + translated.z * newUp.z;
		float z = translated.x * norm.x + translated.y * norm.y + translated.z * norm.z;
		projectedVertices.push_back({ x, y, z });
	}

	// Obliczenie rozmiarów obrazu na płaszczyźnie rzutowania
	float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max(), maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max(), maxZ = std::numeric_limits<float>::lowest();

	for (const auto& vertex : projectedVertices) {
		minX = std::min(minX, vertex.x);
		maxX = std::max(maxX, vertex.x);
		minY = std::min(minY, vertex.y);
		maxY = std::max(maxY, vertex.y);
		minZ = std::min(minZ, vertex.z);
		maxZ = std::max(maxZ, vertex.z);
	}

	return CPoint3f(maxX - minX, maxY - minY, maxZ - minZ);
}


void createRotationMatrixZ(float theta, float matrix[3][3]) {
	float rad = theta * PI / 180.0f;
	matrix[0][0] = std::cos(rad);	matrix[0][1] = -std::sin(rad);	matrix[0][2] = 0;
	matrix[1][0] = std::sin(rad);	matrix[1][1] = std::cos(rad);	matrix[1][2] = 0;
	matrix[2][0] = 0;				matrix[2][1] = 0;				matrix[2][2] = 1;
}


void createRotationMatrixY(float theta, float matrix[3][3]) {
	float rad = theta * PI / 180.0f;
	matrix[0][0] = std::cos(rad); matrix[0][1] = 0; matrix[0][2] = std::sin(rad);
	matrix[1][0] = 0; matrix[1][1] = 1; matrix[1][2] = 0;
	matrix[2][0] = -std::sin(rad); matrix[2][1] = 0; matrix[2][2] = std::cos(rad);
}

void createRotationMatrixX(float phi, float matrix[3][3]) {
	float rad = phi * PI / 180.0f;
	matrix[0][0] = 1; matrix[0][1] = 0; matrix[0][2] = 0;
	matrix[1][0] = 0; matrix[1][1] = std::cos(rad); matrix[1][2] = -std::sin(rad);
	matrix[2][0] = 0; matrix[2][1] = std::sin(rad); matrix[2][2] = std::cos(rad);
}

CVector3f calculateFreeNorm(float cam_yaw, float cam_pitch) {
	// Wektor początkowy free_norm
	CVector3f free_norm = { 0.0f, 0.0f, 1.0f };

	// Macierze rotacji
	//float rotationZ[3][3];
	float rotationY[3][3];
	float rotationX[3][3];

	//createRotationMatrixZ(cam_pitch, rotationZ);
	createRotationMatrixY(cam_yaw, rotationY);
	createRotationMatrixX(cam_pitch, rotationX);

	// Rotacja wokół osi Y (yaw)
	CVector3f free_norm_yaw = free_norm.transform(rotationY);

	free_norm_yaw.normalize();

	// Rotacja wokół osi X (pitch)
	CVector3f free_norm_pitch = free_norm_yaw.transform(rotationX);

	return free_norm_pitch.getNormalized();


	//// Rotacja wokół osi X (pitch)
	//CVector3f free_norm_pitch = free_norm.transform(rotationX);

	//free_norm_pitch.normalize();

	//// Rotacja wokół osi Y (yaw)
	//CVector3f free_norm_yaw = free_norm_pitch.transform(rotationY);

	//return free_norm_yaw.getNormalized();
}

CVector3f calculateFreeNorm3(float cam_yaw, float cam_pitch) {
	// Wektor początkowy free_norm
	CVector3f free_norm = { 1.0f, 0.0f, 0.0f };

	// Macierze rotacji
	float rotationZ[3][3];
	float rotationY[3][3];
	//float rotationX[3][3];

	createRotationMatrixZ(cam_yaw, rotationZ);
	//createRotationMatrixY(cam_yaw, rotationY);
	createRotationMatrixY(cam_pitch, rotationY);

	// Rotacja wokół osi Y (yaw)
	CVector3f free_norm_yaw = free_norm.transform(rotationZ);

	free_norm_yaw.normalize();

	// Rotacja wokół osi X (pitch)
	CVector3f free_norm_pitch = free_norm_yaw.transform(rotationY);

	return free_norm_pitch.getNormalized();
}


CVector3f calculateFreeUp3(float cam_yaw, float cam_pitch) {
	// Wektor początkowy free_up
	CVector3f free_up = { 0.0f, 0.0f, -1.0f };

	// Macierze rotacji
	float rotationZ[3][3];
	float rotationY[3][3];
	//float rotationX[3][3];

	createRotationMatrixZ(cam_yaw, rotationZ);
	//createRotationMatrixY(cam_yaw, rotationY);
	//createRotationMatrixX(cam_pitch, rotationX);
	createRotationMatrixY(cam_pitch, rotationY);

	// Rotacja wokół osi Y (yaw) i następnie wokół osi X (pitch)
	//CVector3f free_up_transformed = free_up.transform(rotationY).transform(rotationX);
	CVector3f free_up_transformed = free_up.transform(rotationY).transform(rotationZ);

	return free_up_transformed.getNormalized();
}


CVector3f calculateFreeUp(float cam_yaw, float cam_pitch) {
	// Wektor początkowy free_up
	CVector3f free_up = { 0.0f, 1.0f, 0.0f };

	// Macierze rotacji
	float rotationY[3][3];
	float rotationX[3][3];

	createRotationMatrixY(cam_yaw, rotationY);
	createRotationMatrixX(cam_pitch, rotationX);

	// Rotacja wokół osi Y (yaw) i następnie wokół osi X (pitch)
	//CVector3f free_up_transformed = free_up.transform(rotationY).transform(rotationX);
	CVector3f free_up_transformed = free_up.transform(rotationX).transform(rotationY);

	return free_up_transformed.getNormalized();
}

CVector3f calculateFreeUp2(const CVector3f& free_norm) {
	// Zakładamy, że pierwotny wektor up to (0, 1, 0)
	CVector3f world_up = { 0.0f, 1.0f, 0.0f };

	// Jeśli free_norm jest prawie równoległy do world_up, użyj wektora alternatywnego
	if (std::abs(free_norm.x) < 1e-6 && std::abs(free_norm.z) < 1e-6) {
		world_up = { 0.0f, 0.0f, 1.0f };
	}

	// Oblicz wektor right jako iloczyn wektorowy world_up i free_norm
	CVector3f right = world_up.crossProduct(free_norm).getNormalized();

	// Oblicz nowy wektor up jako iloczyn wektorowy free_norm i right
	CVector3f free_up = free_norm.crossProduct(right).getNormalized();

	return free_up;
}

void VolumetricImageDialog::calcFreeValues()
{
	float cam_yaw = ui.cam_yaw->value();
	float cam_pitch = ui.cam_pitch->value();

	free_origin = CPoint3f(m_VolTK->columns() / 2, m_VolTK->rows() / 2, m_VolTK->layers() / 2);

	free_norm = calculateFreeNorm3(cam_yaw, cam_pitch);
	free_up = calculateFreeUp3(cam_yaw, cam_pitch);
	//free_up = calculateFreeUp2(free_norm);

	//qInfo() << free_up.x << ", " << free_up.y << ", " << free_up.z << Qt::endl;

	//CVector3f right = CVector3f(0.0f,0.1f,0.0f).crossProduct(free_norm);

	//free_up = right.crossProduct(free_norm).getNormalized();

	free_view_size = calculateProjectionDimensions(free_origin, free_norm, free_up, m_VolTK->columns(), m_VolTK->rows(), m_VolTK->layers());

	free_origin = free_origin - free_norm * ceil(free_view_size.z / 2);

	ui.orig_x->blockSignals(true);
	ui.orig_y->blockSignals(true);
	ui.orig_z->blockSignals(true);

	ui.orig_x->setValue(free_origin.x);
	ui.orig_y->setValue(free_origin.y);
	ui.orig_z->setValue(free_origin.z);

	ui.orig_x->blockSignals(false);
	ui.orig_y->blockSignals(false);
	ui.orig_z->blockSignals(false);

	ui.norm_x->blockSignals(true);
	ui.norm_y->blockSignals(true);
	ui.norm_z->blockSignals(true);

	ui.norm_x->setValue(free_norm.x);
	ui.norm_y->setValue(free_norm.y);
	ui.norm_z->setValue(free_norm.z);

	ui.norm_x->blockSignals(false);
	ui.norm_y->blockSignals(false);
	ui.norm_z->blockSignals(false);

	ui.horizontalSlider->setRange(0, ceil(free_view_size.z) - 1);
}


void VolumetricImageDialog::changeDisplayedPixmap(int i, bool forget_mask)
{
	if (currentView == Volumetric::LayerPlane::TEST)
	{
		//float n_x = ui.norm_x->value();
		//float n_y = ui.norm_y->value();
		//float n_z = ui.norm_z->value();

		//free_norm = CVector3f(n_x, n_y, n_z);
		//free_norm.normalize();

		//float o_x = ui.orig_x->value();
		//float o_y = ui.orig_y->value();
		//float o_z = ui.orig_z->value();

		//free_origin = CPoint3f(o_x, o_y, o_z);

		CPoint3f planePoint = free_origin + free_norm * float(i);

		changeDisplayedPixmap(m_VolTK->generateFreeViewSliceImage(free_norm, planePoint, free_up, ceil(free_view_size.x), ceil(free_view_size.y), 1.0f, ui.displ_win->isChecked()));
	}
	else
	{
		if (ui.rtg->isChecked())
			changeDisplayedPixmap(m_VolTK->getRTGasImage(currentView, ui.displ_win->isChecked()), i);
		else
			changeDisplayedPixmap(m_VolTK->getLayerAsImage(i, currentView, ui.displ_win->isChecked()), i);
	}

	if (!common_mask)
	{
		if (!forget_mask)
		{
			m_masks[current_i] = m_imageLabel->mask_to_bitmap();
		}

		m_imageLabel->mask_set(m_masks[i]);
	}
	current_i = i;
}

void VolumetricImageDialog::changeDisplayedPixmap(QImage image, int i)
{
	if (ui.img_invert->isChecked())
	{
		image.invertPixels();
	}

	if (ui.medianGroupBox->isChecked())
	{
		image = doMedianFilter(image, ui.medianFilterSizeSpinBox->value());
	}

	if (ui.convGroupBox->isChecked())
	{
		image = aplyConvFilter(image, conv_filter);
	}

	if (ui.treshGroupBox->isChecked())
	{
		image = tesst4(image, ui.treshSlider->value(), ui.upTreshSlider->value(), ui.hide_all_above_upper->isChecked());
	}

	if (ui.fitCheckBox->isChecked())
	{
		int w = ui.scrollArea->contentsRect().width();
		int h = ui.scrollArea->contentsRect().height();

		m_imageLabel->setImageDisplayed(image, w, h);
	}
	else
	{
		m_imageLabel->setImageDisplayed(image);
	}

	this->update();
}


#include <QCheckBox>

void VolumetricImageDialog::onFitCheckBox(int i)
{
	if (ui.fitCheckBox->isChecked())
	{
		back_scale = m_imageLabel->m_scale;
	}
	else
	{
		m_imageLabel->m_scale = back_scale;
	}

	changeDisplayedPixmap(ui.horizontalSlider->value());
}


void VolumetricImageDialog::onSelectionChanged()//QRect r)
{
	QRect sel = m_imageLabel->m_selection;
	ui.selGroupBox->setChecked((sel.width() > 0) && (sel.height() > 0));

	ui.selLeft->blockSignals(true);
	ui.selLeft->setValue(sel.left());
	ui.selLeft->blockSignals(false);

	ui.selTop->blockSignals(true);
	ui.selTop->setValue(sel.top());
	ui.selTop->blockSignals(false);

	ui.selRight->blockSignals(true);
	ui.selRight->setValue(sel.right());
	ui.selRight->blockSignals(false);

	ui.selBottom->blockSignals(true);
	ui.selBottom->setValue(sel.bottom());
	ui.selBottom->blockSignals(false);
}

void VolumetricImageDialog::onSelLeftEdited(int l)
{
	if ( (l>0) && (l<m_imageLabel->m_selection.right()))
		m_imageLabel->m_selection.setLeft(l);
}

void VolumetricImageDialog::onSelRightEdited(int r)
{
	if (r > m_imageLabel->m_selection.left())
		m_imageLabel->m_selection.setRight(r);
}

void VolumetricImageDialog::onSelTopEdited(int t)
{
	if ((t > 0) && (t < m_imageLabel->m_selection.bottom()))
		m_imageLabel->m_selection.setTop(t);
}

void VolumetricImageDialog::onSelBottomEdited(int b)
{
	if (b > m_imageLabel->m_selection.top())
		m_imageLabel->m_selection.setBottom(b);
}



void VolumetricImageDialog::onTreshCheckBox(int i)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}


void VolumetricImageDialog::on_hide_all_above_upper_toggled(bool b)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_A_btn_toggled(bool b)
{
	m_imageLabel->mask_reset();

	if (b) {
		m_imageLabel->m_work_mode = ImageLabel::WorkMode::WorkModeSelect;

		ui.b_btn->blockSignals(true);
		ui.b_btn->setChecked(!b);
		ui.b_btn->blockSignals(false);

		ui.c_btn->blockSignals(true);
		ui.c_btn->setChecked(!b);
		ui.c_btn->blockSignals(false);
	}
	else {
		m_imageLabel->m_work_mode = ImageLabel::WorkMode::WorkModeUnknown;
	}

	qInfo() << "current mode is: " << m_imageLabel->m_work_mode << Qt::endl;
}

void VolumetricImageDialog::on_B_btn_toggled(bool b)
{
	m_imageLabel->mask_reset();

	if (b) {
		m_imageLabel->m_work_mode = ImageLabel::WorkMode::WorkModeErase;
		common_mask = true;

		ui.a_btn->blockSignals(true);
		ui.a_btn->setChecked(!b);
		ui.a_btn->blockSignals(false);

		ui.c_btn->blockSignals(true);
		ui.c_btn->setChecked(!b);
		ui.c_btn->blockSignals(false);

		ui.d_btn->blockSignals(true);
		ui.d_btn->setChecked(!b);
		ui.d_btn->blockSignals(false);
	}
	else {
		m_imageLabel->m_work_mode = ImageLabel::WorkMode::WorkModeUnknown;
	}

	qInfo() << "current mode is: " << m_imageLabel->m_work_mode << Qt::endl;
}

void VolumetricImageDialog::on_C_btn_toggled(bool b)
{
	m_imageLabel->mask_reset();

	if (b) {
		m_imageLabel->m_work_mode = ImageLabel::WorkMode::WorkModeZoom;

		ui.a_btn->blockSignals(true);
		ui.a_btn->setChecked(!b);
		ui.a_btn->blockSignals(false);

		ui.b_btn->blockSignals(true);
		ui.b_btn->setChecked(!b);
		ui.b_btn->blockSignals(false);

		ui.d_btn->blockSignals(true);
		ui.d_btn->setChecked(!b);
		ui.d_btn->blockSignals(false);
	}
	else {
		m_imageLabel->m_work_mode = ImageLabel::WorkMode::WorkModeUnknown;
	}

	qInfo() << "current mode is: " << m_imageLabel->m_work_mode << Qt::endl;
}

void VolumetricImageDialog::on_d_btn_toggled(bool b)
{
	m_imageLabel->mask_reset();

	if (b) {
		m_imageLabel->m_work_mode = ImageLabel::WorkMode::WorkModeErase;
		common_mask = false;

		ui.a_btn->blockSignals(true);
		ui.a_btn->setChecked(!b);
		ui.a_btn->blockSignals(false);

		ui.b_btn->blockSignals(true);
		ui.b_btn->setChecked(!b);
		ui.b_btn->blockSignals(false);

		ui.c_btn->blockSignals(true);
		ui.c_btn->setChecked(!b);
		ui.c_btn->blockSignals(false);
	}
	else {
		m_imageLabel->m_work_mode = ImageLabel::WorkMode::WorkModeUnknown;
	}

	qInfo() << "current mode is: " << m_imageLabel->m_work_mode << Qt::endl;
}

void VolumetricImageDialog::on_go_first_clicked()
{
	ui.horizontalSlider->setValue(ui.horizontalSlider->minimum());
}

void VolumetricImageDialog::on_go_prev_clicked()
{
	int val = ui.horizontalSlider->value() - 1;
	if (val < ui.horizontalSlider->minimum())
		val = ui.horizontalSlider->minimum();

	ui.horizontalSlider->setValue(val);
}

void VolumetricImageDialog::on_cam_yaw_valueChanged(int v)
{
	if (v < 0)
	{
		ui.cam_yaw->setValue(360+v);
	}
	else if (v > 359)
	{
		ui.cam_yaw->setValue(v-360);
	}
	else
	{
		ui.cam_yaw_slider->blockSignals(true);
		ui.cam_yaw_slider->setValue(v);
		ui.cam_yaw_slider->blockSignals(false);

		calcFreeValues();

		changeDisplayedPixmap(ui.horizontalSlider->value());
	}
}

void VolumetricImageDialog::on_cam_pitch_valueChanged(int v)
{
	ui.cam_pitch_slider->blockSignals(true);
	ui.cam_pitch_slider->setValue(v);
	ui.cam_pitch_slider->blockSignals(false);

	calcFreeValues();

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_norm_x_valueChanged(double x)
{
	free_norm.x = x;
	free_norm.normalize();
	ui.norm_x->blockSignals(true);
	ui.norm_y->blockSignals(true);
	ui.norm_z->blockSignals(true);

	ui.norm_x->setValue(free_norm.x);
	ui.norm_y->setValue(free_norm.y);
	ui.norm_z->setValue(free_norm.z);

	ui.norm_z->blockSignals(false);
	ui.norm_y->blockSignals(false);
	ui.norm_x->blockSignals(false);

	calcFreeValues();

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_norm_y_valueChanged(double y)
{
	free_norm.y = y;
	free_norm.normalize();
	ui.norm_x->blockSignals(true);
	ui.norm_y->blockSignals(true);
	ui.norm_z->blockSignals(true);

	ui.norm_x->setValue(free_norm.x);
	ui.norm_y->setValue(free_norm.y);
	ui.norm_z->setValue(free_norm.z);

	ui.norm_z->blockSignals(false);
	ui.norm_y->blockSignals(false);
	ui.norm_x->blockSignals(false);

	calcFreeValues();

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_norm_z_valueChanged(double z)
{
	free_norm.z = z;
	free_norm.normalize();
	ui.norm_x->blockSignals(true);
	ui.norm_y->blockSignals(true);
	ui.norm_z->blockSignals(true);

	ui.norm_x->setValue(free_norm.x);
	ui.norm_y->setValue(free_norm.y);
	ui.norm_z->setValue(free_norm.z);

	ui.norm_z->blockSignals(false);
	ui.norm_y->blockSignals(false);
	ui.norm_x->blockSignals(false);

	calcFreeValues();

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_orig_x_valueChanged(double x)
{
	//free_origin.x = x;

	//free_origin = CPoint3f(m_VolTK->columns() / 2, m_VolTK->rows() / 2, m_VolTK->layers() / 2);

	//free_view_size = calculateProjectionDimensions(free_origin, free_norm, free_up, m_VolTK->columns(), m_VolTK->rows(), m_VolTK->layers());

	//free_origin = free_origin - free_norm * ceil(free_view_size.z / 2);

	//ui.horizontalSlider->setRange(0, ceil(free_view_size.z) - 1);

	//changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_orig_y_valueChanged(double y)
{
	//free_origin.y = y;

	//free_origin = CPoint3f(m_VolTK->columns() / 2, m_VolTK->rows() / 2, m_VolTK->layers() / 2);

	//free_view_size = calculateProjectionDimensions(free_origin, free_norm, free_up, m_VolTK->columns(), m_VolTK->rows(), m_VolTK->layers());

	//free_origin = free_origin - free_norm * ceil(free_view_size.z / 2);

	//ui.horizontalSlider->setRange(0, ceil(free_view_size.z) - 1);

	//changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_orig_z_valueChanged(double z)
{
	//free_origin.z = z;

	//free_origin = CPoint3f(m_VolTK->columns() / 2, m_VolTK->rows() / 2, m_VolTK->layers() / 2);

	//free_view_size = calculateProjectionDimensions(free_origin, free_norm, free_up, m_VolTK->columns(), m_VolTK->rows(), m_VolTK->layers());

	//free_origin = free_origin - free_norm * ceil(free_view_size.z / 2);

	//ui.horizontalSlider->setRange(0, ceil(free_view_size.z) - 1);

	//changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_rtg_toggled(bool b)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
	ui.image_sliders->setEnabled(!b);

	ui.c_btn->setEnabled(true);
	ui.c_btn->setChecked(b);

	ui.a_btn->setEnabled(!b);
	ui.b_btn->setEnabled(!b);
	ui.d_btn->setEnabled(!b);
}

void VolumetricImageDialog::on_img_invert_toggled(bool b)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_displ_win_toggled(bool b)
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::on_displ_min_valueChanged(int v)
{
	if (v > m_VolTK->m_maxDisplWin)
	{
		v = m_VolTK->m_maxDisplWin;
	}
	else if (v < m_VolTK->m_minVal)
	{
		v = m_VolTK->m_minVal;
	}

	m_VolTK->m_minDisplWin = v;

	changeDisplayedPixmap(ui.horizontalSlider->value());

	ui.displ_max->blockSignals(true);
	ui.displ_max->setMinimum(v);
	ui.displ_max->blockSignals(false);

	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}

void VolumetricImageDialog::on_displ_max_valueChanged(int v)
{
	if (v < m_VolTK->m_minDisplWin)
		v = m_VolTK->m_minDisplWin;
	else if (v > m_VolTK->m_maxVal)
		v = m_VolTK->m_maxVal;

	m_VolTK->m_maxDisplWin = v;

	changeDisplayedPixmap(ui.horizontalSlider->value());

	ui.displ_min->blockSignals(true);
	ui.displ_min->setMaximum(v);
	ui.displ_min->blockSignals(false);

	UI::DOCK::PROPERTIES::updateProperties();
	UI::updateAllViews();
}


void VolumetricImageDialog::on_go_next_clicked()
{
	int val = ui.horizontalSlider->value() + 1;
	if (val > ui.horizontalSlider->maximum())
		val = ui.horizontalSlider->maximum();

	ui.horizontalSlider->setValue(val);
}

void VolumetricImageDialog::on_go_last_clicked()
{
	ui.horizontalSlider->setValue(ui.horizontalSlider->maximum());
}

void VolumetricImageDialog::onTreshValueChanged(int i)
{
	ui.lowerTresh->blockSignals(true);
	ui.lowerTresh->setValue(i);
	ui.lowerTresh->blockSignals(false);

	ui.upperTresh->setMinimum(i + 1);
	ui.upTreshSlider->setMinimum(i + 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::onLowerTreshEdited(int i)
{
	ui.treshSlider->blockSignals(true);
	ui.treshSlider->setValue(i);
	ui.treshSlider->blockSignals(false);

	ui.upperTresh->setMinimum(i + 1);
	ui.upTreshSlider->setMinimum(i + 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}



void VolumetricImageDialog::onUpperTreshValueChanged(int i)
{
	ui.upperTresh->blockSignals(true);
	ui.upperTresh->setValue(i);
	ui.upperTresh->blockSignals(false);

	ui.lowerTresh->setMaximum(i - 1);
	ui.treshSlider->setMaximum(i - 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::onUpperTreshEdited(int i)
{
	ui.upTreshSlider->blockSignals(true);
	ui.upTreshSlider->setValue(i);
	ui.upTreshSlider->blockSignals(false);

	ui.lowerTresh->setMaximum(i - 1);
	ui.treshSlider->setMaximum(i - 1);

	changeDisplayedPixmap(ui.horizontalSlider->value());
}

void VolumetricImageDialog::clear_masks()
{
	for (auto& m : m_masks)
	{
		//m.fill(Qt::GlobalColor::transparent);
		m.fill(Qt::black);
	}
}

void VolumetricImageDialog::resize_masks()
{
	unsigned int cnt, cols, rows;
	
	if (currentView == Volumetric::LayerPlane::YZ)
	{
		cnt = m_VolTK->columns();
		rows = m_VolTK->rows();
		cols = m_VolTK->layers();
	}
	else if (currentView == Volumetric::LayerPlane::ZX)
	{
		cnt = m_VolTK->rows();
		rows = m_VolTK->layers();
		cols = m_VolTK->columns();
	}
	else // XY
	{
		cnt = m_VolTK->layers();
		rows = m_VolTK->rows();
		cols = m_VolTK->columns();
	}

	m_masks.resize(cnt);
	for (auto& m : m_masks)
	{
		m = QBitmap(cols, rows);
		m.fill(Qt::black);
	}
}

void VolumetricImageDialog::onViewPlaneChanged(QString val)
{
	bool b = (val == "Free");
	ui.free_plane->setEnabled(b);

	ui.c_btn->setEnabled(true);
	ui.c_btn->setChecked(b);

	ui.a_btn->setEnabled(!b);
	ui.b_btn->setEnabled(!b);
	ui.d_btn->setEnabled(!b);


	if (val == "YZ")
	{
		currentView = Volumetric::LayerPlane::YZ;

		ui.horizontalSlider->setRange(0, m_VolTK->columns() - 1);
	}
	else if (val == "ZX")
	{
		currentView = Volumetric::LayerPlane::ZX;

		ui.horizontalSlider->setRange(0, m_VolTK->rows() - 1);
	}
	else if (val == "Free")
	{
		currentView = Volumetric::LayerPlane::TEST;

		calcFreeValues();
	}
	else // "XY"
	{
		currentView = Volumetric::LayerPlane::XY;

		ui.horizontalSlider->setRange(0, m_VolTK->layers() - 1);
	}

	if (!common_mask) resize_masks();

	ui.horizontalSlider->setValue(0);
	changeDisplayedPixmap(0, true);
	current_i = 0;
}

void VolumetricImageDialog::onDispBitsChanged(QString val)
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

void VolumetricImageDialog::onSliderValueChanged(int i)
{
	changeDisplayedPixmap(i);
}



QImage VolumetricImageDialog::aplyConvFilter(QImage srcImg, Filter fltr)
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

			if (sum > 0.00000001)
				((uint16_t*)dstLine)[x] = round(temp / sum);
			else
				((uint16_t*)dstLine)[x] = round(temp);
		}
	}

	return dstImg;
}


void VolumetricImageDialog::onConvFilterBtn()
{
	FilterDialog* dlg = new FilterDialog(&conv_filter);

	if (dlg->exec())
	{
		//QImage image = aplyConvFilter(m_VolTK->getLayerAsImage(ui.horizontalSlider->value(), currentView), conv_filter);
		
		changeDisplayedPixmap(ui.horizontalSlider->value());
	}
}

#include <QMouseEvent>
void VolumetricImageDialog::onImageScaleChanged()
{
	changeDisplayedPixmap(ui.horizontalSlider->value());
}

#include <QtWidgets>


void VolumetricImageDialog::onImageMousePressed(QMouseEvent* e)
{
	if (e->buttons() == Qt::RightButton)
	{
		// context menu
		QMenu* menu = new QMenu("Context menu");

		if (m_imageLabel->m_work_mode == ImageLabel::WorkMode::WorkModeSelect)
		{
			menu->addAction("next slice", this, [&]() {
				on_go_next_clicked();
				})->setEnabled((current_i < ui.horizontalSlider->maximum()));

			menu->addAction("previous slice", this, [&]() {
				on_go_prev_clicked();
				})->setEnabled(current_i > ui.horizontalSlider->minimum());
		}
		else if (m_imageLabel->m_work_mode == ImageLabel::WorkMode::WorkModeErase)
		{
			menu->addAction("next slice", this, [&]() {
				on_go_next_clicked();
				})->setEnabled((current_i < ui.horizontalSlider->maximum()));

			if (!common_mask)
			{
				menu->addAction("next and copy current mask", this, [&]() {
					on_go_next_clicked();
					if (current_i > ui.horizontalSlider->minimum()) m_imageLabel->mask_set(m_masks[current_i - 1]);
					})->setEnabled((current_i < ui.horizontalSlider->maximum()));
			}

			menu->addAction("previous slice", this, [&]() {
				on_go_prev_clicked();
				})->setEnabled(current_i > ui.horizontalSlider->minimum());

			if (!common_mask)
			{
				menu->addAction("previous and copy current mask", this, [&]() {
					on_go_prev_clicked();
					if (current_i < ui.horizontalSlider->maximum()) m_imageLabel->mask_set(m_masks[current_i + 1]);
					})->setEnabled(current_i > ui.horizontalSlider->minimum());
			
			
				menu->addSeparator();

				QAction *a1 = menu->addAction("copy mask from prevoius slice", this, [&]() {
					m_imageLabel->mask_set(m_masks[current_i - 1]);
					});
				a1->setEnabled(current_i > ui.horizontalSlider->minimum());
				a1->setShortcut(Qt::CTRL + Qt::Key_1);

				QAction* a2 = menu->addAction("copy mask from next slice", this, [&]() {
					m_imageLabel->mask_set(m_masks[current_i + 1]);
					});
				a2->setEnabled(current_i < ui.horizontalSlider->maximum());
				a2->setShortcut(Qt::CTRL + Qt::Key_2);
			}

			menu->addSeparator();

			menu->addAction("clear current mask", this, [&]() {
				m_imageLabel->mask_reset();
				});

			if (!common_mask)
			{
				menu->addAction("revert changes on current mask", this, [&]() {
					m_imageLabel->mask_set(m_masks[current_i]);
					});

				menu->addSeparator();

				menu->addAction("clear all masks", this, [&]() {
					m_imageLabel->mask_reset();
					this->clear_masks();
					});
			}
		}

		menu->exec(m_imageLabel->mapToGlobal(e->pos()));
	}
	else if (e->buttons() == Qt::LeftButton)
	{
		if (m_imageLabel->m_work_mode == ImageLabel::WorkMode::WorkModeZoom)
		{
			lastPos = e->pos();
		}
	}
}

void VolumetricImageDialog::onImageMouseMoved(QMouseEvent* e)
{
	if (e->buttons() == Qt::LeftButton)
	{
		if (m_imageLabel->m_work_mode == ImageLabel::WorkMode::WorkModeZoom)
		{
			int dx = e->pos().x() - lastPos.x();
			int dy = e->pos().y() - lastPos.y();

			if (abs(dx) >= abs(dy))
			{
				dx = dx > 0 ? 1 : -1;
				ui.cam_yaw->setValue(ui.cam_yaw->value() + dx);
			}
			else
			{
				dy = dy > 0 ? -1 : 1;
				ui.cam_pitch->setValue(ui.cam_pitch->value() + dy);
			}

			lastPos = e->pos();
		}
	}
}


//
//int mediana(NowaKostka1& kostka, size_t destAdress, size_t cols, int size)
//{
//	int halfSize = size / 2;
//	size_t srcAdress = destAdress - halfSize;
//
//	int srcVal[81]; //array for maximal size 9
//	
//	size_t srcRowAdress = srcAdress - cols * halfSize;
//
//	for (int srow = 0; srow < size; srow++)
//	{
//		size_t srcCellAdress = srcRowAdress + srow;
//		for (int scol = 0; scol < size; scol++)
//		{
//			srcVal[srow * size + scol] = ((NowaKostka1&)kostka).get(srcCellAdress);
//			srcCellAdress++;
//		}
//
//		srcRowAdress += cols;
//	}
//	return med(srcVal, size*size);
//}
//
//int VolTKimageDialog::mediana0(NowaKostka1& kostka, size_t dest0adress, int filterSize)
//{
//	size_t srcRowAdress = dest0adress;
//
//	int* srcVal = new int[filterSize*filterSize]; //array for maximal size 9
//	int fastIdx = 0;
//	for (int srow = 0; srow < filterSize; srow++)
//	{
//		size_t srcCellAdress = srcRowAdress + srow;
//		for (int scol = 0; scol < filterSize; scol++)
//		{
//			//srcVal[srow * filterSize + scol] = ((NowaKostka1&)kostka).get(srcCellAdress);
//			srcVal[fastIdx] = kostka.get(srcCellAdress);
//			fastIdx++;
//			srcCellAdress++;
//		}
//
//		srcRowAdress += kostka.m_cols;
//	}
//
//	int result = med(srcVal, filterSize * filterSize);
//	delete[] srcVal;
//	
//	return result;
//}
//
//
//void VolTKimageDialog::do3dMedianFilter(NowaKostka1& src, NowaKostka1&dst, int size)
//{
//	UI::PROGRESSBAR::init(0, src.m_lays, 0);
//	UI::PROGRESSBAR::setText("3d data filtering: ");
//
//	size_t destOffset2 = src.m_cols * (size / 2) + (size / 2);
//	
//	bool setTresh = ui.treshGroupBox->isChecked();
//	int lowTresh = ui.treshSlider->value();
//	int upTresh = ui.upTreshSlider->value();
//
//	for (size_t layer = 0; layer < src.m_lays; layer++)
//	{
//		if (!(layer%10)) UI::PROGRESSBAR::setValue(layer);
//		size_t layOffset = src.m_rows * src.m_cols * layer;
////		#pragma loop(hint_parallel(0))
//		for (size_t row = 0; row < (src.m_rows - (size - 1)); row++)
//		{
//			size_t rowOffset = src.m_cols * row;
//			for (size_t col = 0; col < (src.m_cols - (size - 1)); col++)
//			{
//				size_t currentFilter0adress = layOffset + rowOffset + col;
//				size_t currentDestAdress = currentFilter0adress + destOffset2;
//
//				int newValue = mediana0(src, currentFilter0adress, size);
//				
//				//if (setTresh)
//				//{
//				//	if (newValue < lowTresh)
//				//	{
//				//		newValue = 0;
//				//	}
//				//	else if (newValue > upTresh)
//				//	{
//				//		newValue = 0;
//				//	}
//				//}
//
//				//dst.set(currentDestAdress, newValue);
//				dst.set(col+(size/2),row+(size/2),layer, newValue);
//			}
//		}
//	}
//	UI::PROGRESSBAR::hide();
//}
//

//#include "AP.h"

//void VolTKimageDialog::onCreateVolTKStructureBtn()
//{
//	Volumetric* nowy = new Volumetric(*m_VolTK);
//
//	if (ui.medianGroupBox->isChecked())
//	{
//		nowy->kostka.setAllData(0);
//		do3dMedianFilter(m_VolTK->kostka, nowy->kostka, ui.medianFilterSizeSpinBox->value());
//	}
//
//	int lowTresh = ui.treshSlider->value();
//	int upTresh = ui.upTreshSlider->value();
//
//	if (ui.treshGroupBox->isChecked())
//	{
//		for (size_t l = 0; l < nowy->kostka.m_lays; l++)
//			for (size_t r = 0; r < nowy->kostka.m_rows; r++)
//				for (size_t c = 0; c < nowy->kostka.m_cols; c++)
//				{
//					int val = nowy->kostka.get(c, r, l);
//
//					if (val < lowTresh)
//					{
//						nowy->kostka.set(c, r, l, 0);
//					}
//					else if (val > upTresh)
//					{
//						nowy->kostka.set(c, r, l, 0xffff);
//					}
//					else
//					{
//						nowy->kostka.set(c, r, l, val);
//					}
//				}
//	}
//
//	nowy->createDisplayData();
//	nowy->setGeometry();
//
//	AP::OBJECT::addChild(m_VolTK->getParent(), nowy);
//
//	accept();
//}