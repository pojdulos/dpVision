#include "ParserTKD.h"
#include "Utilities.h"

#include <fstream>
#include <sstream>

#include "Image.h"
//#include "Volumetric.h"
#include "VolTK.h"

CParserTKD::CParserTKD()
{
	setDescr("TKD files");
	m_exts.insert("tkd");

	is16bit = false;

#if defined(TESTVOLTK)

#else
	treshold.insert(0x8000);
#endif
}

CParserTKD::~CParserTKD(void)
{
#if not defined(TESTVOLTK)
	for each (auto l in maskMap)
		delete l;
	maskMap.clear();
#endif
}




#if defined(TESTVOLTK)


void CParserTKD::cleanLayerW(int w, int h, int depth, int layerId, std::vector<void*>& maskMap, WORD treshold)
{
	if (treshold == 0) return;

	std::vector<WORD>* layer = (std::vector<WORD>*) maskMap[layerId];
	std::vector<WORD>* prevLayer = (std::vector<WORD>*) maskMap[layerId - 1];
	std::vector<WORD>* nextLayer = (std::vector<WORD>*) maskMap[layerId + 1];

	for (int y = 1; y < h - 1; y++)
	{
		for (int x = 1; x < w - 1; x++)
		{
			WORD mask = layer->at(x + y * w);

			if ( mask >= treshold )
			{
				// s¹siedzi "na wprost"
				if ((layer->at(x - 1 + y * w) != 0)
					&& (layer->at(x + (y - 1) * w) != 0)
					&& (prevLayer->at(x + y * w) != 0)
					&& (layer->at(x + 1 + y * w) != 0)
					&& (layer->at(x + (y + 1) * w) != 0)
					&& (nextLayer->at(x + y * w) != 0))
				{
					layer->at(x + y * w) = 1;
				}
			}
		}
	}
}

void CParserTKD::cleanLayerB(int w, int h, int depth, int layerId, std::vector<void*>& maskMap, BYTE treshold)
{
	if (treshold == 0) return;

	std::vector<BYTE>* layer = (std::vector<BYTE>*) maskMap[layerId];
	std::vector<BYTE>* prevLayer = (std::vector<BYTE>*) maskMap[layerId - 1];
	std::vector<BYTE>* nextLayer = (std::vector<BYTE>*) maskMap[layerId + 1];

	for (int y = 1; y < h - 1; y++)
	{
		for (int x = 1; x < w - 1; x++)
		{
			BYTE mask = layer->at(x + y * w);

			if (mask >= treshold)
			{
				// s¹siedzi "na wprost"
				if ((layer->at(x - 1 + y * w) != 0)
					&& (layer->at(x + (y - 1) * w) != 0)
					&& (prevLayer->at(x + y * w) != 0)
					&& (layer->at(x + 1 + y * w) != 0)
					&& (layer->at(x + (y + 1) * w) != 0)
					&& (nextLayer->at(x + y * w) != 0))
				{
					layer->at(x + y * w) = 1;
				}
			}
		}
	}
}

size_t CParserTKD::readVoxelsBACK(CVolTK* pVolTK, int depth, std::string prefix, std::string ext, int min, int max, WORD treshold, bool cleanIt)
{
	UI::PROGRESSBAR::init(min, max, min);

	int w = pVolTK->cols();
	int h = pVolTK->rows();

	std::vector<void*> maskMap;

	bool isNoSelection = (m_winTX > m_winBX) && (m_winTY > m_winBY);
	bool isSelection = !isNoSelection;

	size_t lbv = 0;
	int iz = 0;
	for (int j = min; j <= max; j++)
	{
		std::string numEXT("");
		if (j < 100) numEXT += "0";
		if (j < 10) numEXT += "0";
		numEXT += std::to_string(j);


		std::string imgname(plikSiatki.absolutePathA() + "\\" + prefix + numEXT + "." + ext);

		bool imgFileExists = UI::fileExists(imgname);

		std::vector<BYTE> kolory;
		kolory.clear();

		UI::STATUSBAR::printfTimed(1000, "Reading layer nb %d", j);

		if (imgFileExists)
		{

			std::pair<int, int> wh = CImage::loadToBYTEvector(UI::str2wstr(imgname), kolory, depth);

			//w = wh.first;
			//h = wh.second;

			void* layer;
			if (depth == 16)
				layer = new std::vector<WORD>(h * w, 0);
			else
				layer = new std::vector<BYTE>(h * w, 0);


			if ((pVolTK->m_div == 1) || ((iz % pVolTK->m_div) == 0))
			{
				int ix = 0;
				int iy = 0;

				for (int i = 0; i < kolory.size(); )
				{
					if ((pVolTK->m_div == 1) || (((ix % pVolTK->m_div) == 0) && ((iy % pVolTK->m_div) == 0)))
					{

						WORD kolor;

						if (depth == 16) // grayscale, 16bitów na piksel
						{
							kolor = *((WORD*)&kolory[i]);

							i += 2;
						}
						else if (depth == 8) // grayscale, 8bitów na piksel
						{
							kolor = kolory[i];

							i++;
						}
						else if (depth == 32) // ARGB, 8bitów na kolor
						{
							kolor = CRGBA::fromArgb32(&kolory[i]).R();

							i += 4;
						}
						else if (depth == 24) // RGB, 8bitów na kolor
						{
							kolor = CRGBA::fromArgb32(&kolory[i]).R();

							i += 3;
						}
						else if (depth == 48) // RGB, 16bitów na kolor
						{
							kolor = *((WORD*)&kolory[i]);

							i += 6;
						}

						if (kolor >= treshold)
						{
							if (isNoSelection || ((ix >= m_winTX) && (ix < m_winBX) && (iy >= m_winTY) && (iy < m_winBY)))
							{

								if (is16bit)
								{
									((std::vector<WORD>*)layer)->at(ix + iy * w) = kolor;
								}
								else
								{
									((std::vector<BYTE>*)layer)->at(ix + iy * w) = kolor;
								}

							}
						}

					}
					else
					{
						if (depth == 16) // grayscale, 16bitów na piksel
						{
							i += 2;
						}
						else if (depth == 8) // grayscale, 8bitów na piksel
						{
							i++;
						}
						else if (depth == 32) // ARGB, 8bitów na kolor
						{
							i += 4;
						}
						else if (depth == 24) // RGB, 8bitów na kolor
						{
							i += 3;
						}
						else if (depth == 48) // RGB, 16bitów na kolor
						{
							i += 6;
						}
					}

					ix++;
					if (ix >= w)
					{
						ix = 0;
						iy++;
					}
				}
			}
			maskMap.push_back(layer);
		}

		//if (cleanIt)
		//{
		//	if (iz > 0)
		//	{
		//		if (iz > 1)
		//		{
		//			if (iz < (max - min))
		//			{
		//				if ( is16bit )
		//					cleanLayerW(w, h, depth, iz - 1, maskMap, treshold);
		//				else
		//					cleanLayerB(w, h, depth, iz - 1, maskMap, treshold);
		//			}

		//			if ( is16bit )
		//				delete (std::vector<WORD>*)maskMap[iz - 2];
		//			else
		//				delete (std::vector<BYTE>*)maskMap[iz - 2];
		//		}

		//		if ( is16bit )
		//			lbv += pVolTK->addLayerW(iz - 1, (std::vector<WORD>*) maskMap[iz - 1], treshold);
		//		else
		//			lbv += pVolTK->addLayerB(iz - 1, (std::vector<BYTE>*) maskMap[iz - 1], treshold);
		//	}
		//}
		//else
		//{
		if (is16bit)
		{
			lbv += pVolTK->addLayerW(iz, (std::vector<WORD>*) maskMap[iz], treshold);
			delete (std::vector<WORD>*)maskMap[iz];
		}
		else
		{
			lbv += pVolTK->addLayerB(iz, (std::vector<BYTE>*) maskMap[iz], treshold);
			delete (std::vector<BYTE>*) maskMap[iz];
		}
		//}

		iz += 1;

		UI::PROGRESSBAR::setValue(j);
	}

	//if (cleanIt)
	//{
	//	if ( is16bit )
	//	{
	//		lbv += pVolTK->addLayerW(max - min, (std::vector<WORD>*) maskMap[max - min], treshold);
	//		delete (std::vector<WORD>*)maskMap[max - min - 1];
	//		delete (std::vector<WORD>*)maskMap[max - min];
	//	}
	//	else
	//	{
	//		lbv += pVolTK->addLayerB(max - min, (std::vector<BYTE>*) maskMap[max - min], treshold);
	//		delete (std::vector<BYTE>*)maskMap[max - min - 1];
	//		delete (std::vector<BYTE>*)maskMap[max - min];
	//	}
	//}

	return lbv;
}


size_t CParserTKD::readVoxels(CVolTK* pVolTK, int depth, std::string prefix, std::string ext, int min, int max, WORD lowTreshold, WORD upTreshold, bool cleanIt )
{
	UI::PROGRESSBAR::init(min, max, min);

	int w = pVolTK->cols();
	int h = pVolTK->rows();

	std::vector<void*> maskMap;

	bool isNoSelection = (m_winTX > m_winBX) && (m_winTY > m_winBY);

	if (isNoSelection)
	{
		m_winTX = 0;
		m_winBX = w - 1;
		m_winTY = 0;
		m_winBY = h - 1;
	}

	int byteOffset = depth / 8;

	size_t lbv = 0;
	int iz = 0;
	for (int j = min; j <= max; j++)
	{
		std::string numEXT("");
		if (j < 100) numEXT += "0";
		if (j < 10) numEXT += "0";
		numEXT += std::to_string(j);


		std::string imgname(plikSiatki.absolutePathA() + "\\" + prefix + numEXT + "." + ext);

		bool imgFileExists = UI::fileExists(imgname);

		std::vector<BYTE> kolory;
		kolory.clear();

		UI::STATUSBAR::printfTimed(1000, "Reading layer nb %d", j);

		if (imgFileExists)
		{

			std::pair<int, int> wh = CImage::loadToBYTEvector(UI::str2wstr(imgname), kolory, depth);

			//w = wh.first;
			//h = wh.second;

			void* layer;
			if (depth == 16)
				layer = new std::vector<WORD>(h * w, 0);
			else
				layer = new std::vector<BYTE>(h * w, 0);


			if ((pVolTK->m_div == 1) || ((iz % pVolTK->m_div) == 0))
			{
				for (int iy = m_winTY; iy <= m_winBY; iy++)
				{
					for (int ix = m_winTX; ix <= m_winBX; ix++)
					{
						if ((pVolTK->m_div == 1) || (((ix % pVolTK->m_div) == 0) && ((iy % pVolTK->m_div) == 0)))
						{
							int i = byteOffset * ( ix + w * iy );

							WORD kolor;

							if (depth == 16) // grayscale, 16bitów na piksel
							{
								kolor = *((WORD*)&kolory[i]);
							}
							else if (depth == 8) // grayscale, 8bitów na piksel
							{
								kolor = kolory[i];
							}
							else if (depth == 32) // ARGB, 8bitów na kolor
							{
								kolor = CRGBA::fromArgb32(&kolory[i]).R();
							}
							else if (depth == 24) // RGB, 8bitów na kolor
							{
								kolor = CRGBA::fromArgb32(&kolory[i]).R();
							}
							else if (depth == 48) // RGB, 16bitów na kolor
							{
								kolor = *((WORD*)&kolory[i]);
							}

							if ( (kolor >= lowTreshold) && (kolor <= upTreshold))
							{
								if (is16bit)
								{
									((std::vector<WORD>*)layer)->at(ix + iy * w) = kolor;
								}
								else
								{
									((std::vector<BYTE>*)layer)->at(ix + iy * w) = kolor;
								}
							}

						}
					}
				}
			}
			maskMap.push_back(layer);
		}

		if (cleanIt)
		{
			if (iz > 0)
			{
				if (iz > 1)
				{
					if (iz < (max - min))
					{
						if ( is16bit )
							cleanLayerW(w, h, depth, iz - 1, maskMap, lowTreshold);
						else
							cleanLayerB(w, h, depth, iz - 1, maskMap, lowTreshold);
					}

					if ( is16bit )
						delete (std::vector<WORD>*)maskMap[iz - 2];
					else
						delete (std::vector<BYTE>*)maskMap[iz - 2];
				}

				if ( is16bit )
					lbv += pVolTK->addLayerW(iz - 1, (std::vector<WORD>*) maskMap[iz - 1], lowTreshold, upTreshold);
				else
					lbv += pVolTK->addLayerB(iz - 1, (std::vector<BYTE>*) maskMap[iz - 1], lowTreshold, upTreshold);
			}
		}
		else
		{
			if ( is16bit )
			{
				lbv += pVolTK->addLayerW(iz, (std::vector<WORD>*) maskMap[iz], lowTreshold, upTreshold);
				delete (std::vector<WORD>*)maskMap[iz];
			}
			else
			{
				lbv += pVolTK->addLayerB(iz, (std::vector<BYTE>*) maskMap[iz], lowTreshold, upTreshold);
				delete (std::vector<BYTE>*) maskMap[iz];
			}
		}

		iz += 1;

		UI::PROGRESSBAR::setValue(j);
	}

	if (cleanIt)
	{
		if ( is16bit )
		{
			lbv += pVolTK->addLayerW(max - min, (std::vector<WORD>*) maskMap[max - min], lowTreshold, upTreshold);
			delete (std::vector<WORD>*)maskMap[max - min - 1];
			delete (std::vector<WORD>*)maskMap[max - min];
		}
		else
		{
			lbv += pVolTK->addLayerB(max - min, (std::vector<BYTE>*) maskMap[max - min], lowTreshold, upTreshold);
			delete (std::vector<BYTE>*)maskMap[max - min - 1];
			delete (std::vector<BYTE>*)maskMap[max - min];
		}
	}

	return lbv;
}

WORD dajKolor(size_t i, std::vector<BYTE> &kolory, int depth)
{
	WORD kolor;

	if (depth == 16) // grayscale, 16bitów na piksel
	{
		kolor = *((WORD*)&kolory[i]);
	}
	else if (depth == 8) // grayscale, 8bitów na piksel
	{
		kolor = kolory[i];
	}
	else if (depth == 32) // ARGB, 8bitów na kolor
	{
		kolor = CRGBA::fromArgb32(&kolory[i]).R();
	}
	else if (depth == 24) // RGB, 8bitów na kolor
	{
		kolor = CRGBA::fromArgb32(&kolory[i]).R();
	}
	else if (depth == 48) // RGB, 16bitów na kolor
	{
		kolor = *((WORD*)&kolory[i]);
	}

	return kolor;
}

size_t CParserTKD::readVoxels2(CVolTK* pVolTK, int depth, std::string prefix, std::string ext, int min, int max, WORD lowTreshold, WORD upTreshold, bool cleanIt)
{
	UI::PROGRESSBAR::init(min, max, min);

	int imageWidth = pVolTK->cols();
	int imageHeight = pVolTK->rows();

	std::map<int,void*> maskMap;

	bool isNoSelection = (m_winTX > m_winBX) && (m_winTY > m_winBY);

	if (isNoSelection)
	{
		m_winTX = 0;
		m_winBX = imageWidth - 1;
		m_winTY = 0;
		m_winBY = imageHeight - 1;
	}
	else
	{
		pVolTK->setColsRows(m_winBX - m_winTX + 1, m_winBY - m_winTY + 1);
	}

	int byteOffset = depth / 8;

	size_t lbv = 0;

	for (int j = min; j <= max; j += pVolTK->m_div)
	{
		int iz = j - min;

		std::string numEXT("");
		if (j < 100) numEXT += "0";
		if (j < 10) numEXT += "0";
		numEXT += std::to_string(j);

		std::string imgname(plikSiatki.absolutePathA() + "\\" + prefix + numEXT + "." + ext);

		bool imgFileExists = UI::fileExists(imgname);

		UI::STATUSBAR::printfTimed(1000, "Reading layer nb %d", j);

		if (imgFileExists)
		{
			std::vector<BYTE> kolory;
			std::pair<int, int> wh = CImage::loadToBYTEvector(UI::str2wstr(imgname), kolory, depth);

			void* layer;
			if (is16bit)
				layer = new std::vector<WORD>(pVolTK->rows() * pVolTK->cols(), 0);
			else
				layer = new std::vector<BYTE>(pVolTK->rows() * pVolTK->cols(), 0);


			for (int iy = m_winTY; iy <= m_winBY; iy += pVolTK->m_div)
			{
				for (int ix = m_winTX; ix <= m_winBX; ix += pVolTK->m_div)
				{
					size_t i = (size_t) byteOffset * (ix + static_cast<size_t>(imageWidth) * iy);

					WORD kolor = dajKolor(i, kolory, depth);

					if ((kolor > lowTreshold) && (kolor <= upTreshold))
					{
						size_t idx = (ix - m_winTX) + (iy - m_winTY) * pVolTK->cols();

						if (is16bit)
						{
							((std::vector<WORD>*)layer)->at(idx) = kolor;// upTreshold;
						}
						else
						{
							((std::vector<BYTE>*)layer)->at(idx) = kolor;// upTreshold;
						}
					}
				}
			}
			
			maskMap[iz] = layer;
		}

		//if (cleanIt)
		//{
		//	if (iz > 0)
		//	{
		//		if (iz > 1)
		//		{
		//			if (iz < (max - min))
		//			{
		//				if (is16bit)
		//					cleanLayerW(w, h, depth, iz - 1, maskMap, lowTreshold);
		//				else
		//					cleanLayerB(w, h, depth, iz - 1, maskMap, lowTreshold);
		//			}

		//			if (is16bit)
		//				delete (std::vector<WORD>*)maskMap[iz - 2];
		//			else
		//				delete (std::vector<BYTE>*)maskMap[iz - 2];
		//		}

		//		if (is16bit)
		//			lbv += pVolTK->addLayerW(iz - 1, (std::vector<WORD>*) maskMap[iz - 1], lowTreshold, upTreshold);
		//		else
		//			lbv += pVolTK->addLayerB(iz - 1, (std::vector<BYTE>*) maskMap[iz - 1], lowTreshold, upTreshold);
		//	}
		//}
		//else
		//{
			if (is16bit)
			{
				lbv += pVolTK->addLayerW(iz, (std::vector<WORD>*) maskMap[iz], lowTreshold, upTreshold);
				delete (std::vector<WORD>*)maskMap[iz];
			}
			else
			{
				lbv += pVolTK->addLayerB(iz, (std::vector<BYTE>*) maskMap[iz], lowTreshold, upTreshold);
				delete (std::vector<BYTE>*) maskMap[iz];
			}
		//}

		UI::PROGRESSBAR::setValue(j);
	}

	//if (cleanIt)
	//{
	//	if (is16bit)
	//	{
	//		lbv += pVolTK->addLayerW(max - min, (std::vector<WORD>*) maskMap[max - min], lowTreshold, upTreshold);
	//		delete (std::vector<WORD>*)maskMap[max - min - 1];
	//		delete (std::vector<WORD>*)maskMap[max - min];
	//	}
	//	else
	//	{
	//		lbv += pVolTK->addLayerB(max - min, (std::vector<BYTE>*) maskMap[max - min], lowTreshold, upTreshold);
	//		delete (std::vector<BYTE>*)maskMap[max - min - 1];
	//		delete (std::vector<BYTE>*)maskMap[max - min];
	//	}
	//}

	return lbv;
}

//size_t CParserTKD::removeInternalVoxels(int w, int h, std::vector<std::vector<WORD>*> &maskMap, WORD treshold )
//{
//	UI::PROGRESSBAR::init(0, maskMap.size(), 0);
//	for (int z = 1; z < maskMap.size() - 1; z++)
//	{
//		UI::STATUSBAR::printfTimed(1000, "Cleaning layer %d", z);
//
//		cleanLayer(w, h, z, maskMap, treshold);
//		
//		UI::PROGRESSBAR::setValue(z);
//	}
//}

#include "TKimageDialog.h"
#include <QtCore/QDir>

size_t CParserTKD::Run()
{
	if (this->bIsNotSet) return 0;

	long lbv = 0;

	//===================================================================

	FILE* tkd = fopen(plikSiatki.absoluteFilePathA().c_str(), "r");

	char ext[10], prefix[250];
	int min, max;
	double pxx = 1.0, pxy = 1.0, pxz = 1.0;

	m_winTX = 0;// 1180;
	m_winTY = 0;// 1200;

	m_winBX = -1;// 1640;
	m_winBY = -1;// 1650;

	uint16_t lowTreshold = 0x8000;
	uint16_t upTreshold = 0xffff;

	fscanf(tkd, "extension = %s\n", ext);
	fscanf(tkd, "prefix = %s\n", prefix);
	fscanf(tkd, "scope = %d,%d\n", &min, &max);
	fscanf(tkd, "treshold = %u\n", &lowTreshold);
	fscanf(tkd, "voxel = %lf,%lf,%lf\n", &pxx, &pxy, &pxz);

	int depth;
	int w, h;


	//QDir directory(QString::fromStdString(plikSiatki.absolutePathA()));
	//QStringList images = directory.entryList(QStringList() << "*.tif" << "*.TIF", QDir::Files, QDir::SortFlag::Name);
	//foreach(QString filename, images) {
	//	//do whatever you need to do
	//}

	QString imgname(plikSiatki.absolutePath() + QString::asprintf("/%s%03d.%s", prefix, min, ext));

	if ( CFileInfo(imgname).exists() )
	{
		CImage* im = CImage::load(imgname);
		w = im->width();
		h = im->height();
		depth = im->depth();
		delete im;
	}
	else
	{
		return 0;
	}

	if ((depth == 16) || (depth == 48))
	{
		is16bit = true;
	}
	else
	{
		is16bit = false;
	}

	TKimageDialog* dialog = new TKimageDialog(0, 0);

	Ui_TKimageDialog& pmUi = dialog->getUI();


	dialog->setPath( plikSiatki.absolutePath() );
	for (int j = min; j <= max; j++)
	{
		QString qname = QString::asprintf("/%s%03d.%s", prefix, j, ext);
		if ( CFileInfo( plikSiatki.absolutePath() + qname ).exists() )
		{
			dialog->addName(qname);
			pmUi.minCombo->addItem(qname, QVariant(j));
			pmUi.maxCombo->addItem(qname, QVariant(j));
		}
	}

	pmUi.minCombo->setCurrentIndex(0);
	pmUi.maxCombo->setCurrentIndex(max - min);


	pmUi.fileInfoLabel->setText(QString::asprintf("Image format is %s (%d x %d x %d bits)", ext, w, h, depth));


	if (!is16bit)
	{
		if (lowTreshold > 255)
		{
			lowTreshold >>= 8;
		}

		if (upTreshold > 255)
		{
			upTreshold >>= 8;
		}
	}

	if ( (lowTreshold > 0) || (upTreshold < (is16bit ? 0xFFFF : 0xFF)))
	{
		dialog->setTreshWidget(true, 0, is16bit ? 0xFFFF : 0xFF, lowTreshold, upTreshold);
	}
	else
	{
		dialog->setTreshWidget(false, 0, is16bit ? 0xFFFF : 0xFF, lowTreshold, upTreshold);
	}

	
	pmUi.VoxXedit->setText(QString::number(pxx));
	pmUi.VoxYedit->setText(QString::number(pxy));
	pmUi.VoxZedit->setText(QString::number(pxz));


	dialog->setSelection(m_winTX, m_winTY, m_winBX, m_winBY);

	dialog->m_is16bit = is16bit;

	if (dialog->exec())
	{
		if ( ! dialog->getTresh(lowTreshold, upTreshold) )
		{
			lowTreshold = 0;
			upTreshold = is16bit ? 0xffff : 0xff;
		}

		min = pmUi.minCombo->currentData().toInt();
		max = pmUi.maxCombo->currentData().toInt();

		if (min > max) max = min;

		pxx = pmUi.VoxXedit->text().toDouble();
		
		if (pmUi.xyzCheckBox->checkState() == Qt::CheckState::Checked)
		{
			pxy = pxx;
			pxz = pxx;
		}
		else
		{
			pxy = pmUi.VoxYedit->text().toDouble();
			pxz = pmUi.VoxZedit->text().toDouble();
		}

		QRect selection = dialog->getSelection();
		m_winTX = selection.left();
		m_winBX = selection.right();
		m_winTY = selection.top();
		m_winBY = selection.bottom();


		CVolTK* pVolTK = new CVolTK(m_model, w, h, max-min+1, depth);

		bool cleanIt = (pmUi.cleanCheckBox_2->isEnabled()) && (pmUi.cleanCheckBox_2->checkState() == Qt::CheckState::Checked);

		switch (pmUi.thinningCombo_2->currentIndex())
		{
			case 3:pVolTK->m_div = 8; break;
			case 2:pVolTK->m_div = 4; break;
			case 1:pVolTK->m_div = 2; break;
			case 0:
			default: pVolTK->m_div = 1; break;
		}

		lbv = readVoxels2(pVolTK, depth, prefix, ext, min, max, lowTreshold, upTreshold, cleanIt);

		pVolTK->kostka.m_voxelSize = CPoint3d(pxx, pxy, pxz);

		pVolTK->m_currentIntensityRange = pVolTK->m_dataIntensityRange;
		//pVolTK->m_winMax = pVolTK->m_dataIntensityRange.upper;

		UI::PROGRESSBAR::hide();

		pVolTK->setLabel(L"volTK.t");

		m_model->addChild(pVolTK);

		UI::STATUSBAR::setText("Done!");
	}
	else
	{
		UI::STATUSBAR::setText("Load canceled.");
	}

	delete dialog;
	return lbv;
}


#else

CTriple<double> CParserTKD::readVoxelsOLD(int& w, int& h, std::vector<std::vector<WORD>*>& maskMap, WORD treshold, WORD& minData, WORD& maxData)
{
	FILE* tkd = fopen(plikSiatki.absoluteFilePathA().c_str(), "r");

	char ext[10], prefix[250];
	int min, max, sx, sy, sz;
	double pxx = 1.0, pxy = 1.0, pxz = 1.0;

	fscanf(tkd, "%s", ext);
	fscanf(tkd, "%s", prefix);
	fscanf(tkd, "%d,%d", &min, &max);
	fscanf(tkd, "%d,%d,%d", &sx, &sy, &sz);
	fscanf(tkd, "%lf,%lf,%lf", &pxx, &pxy, &pxz);

	maxData = 0;
	minData = 0xffff;

	UI::PROGRESSBAR::init(min, max, min);

	int iz = 0;
	for (int j = min; j <= max; j++)
	{
		std::string numEXT("");
		if (j < 100) numEXT += "0";
		if (j < 10) numEXT += "0";
		numEXT += std::to_string(j);


		std::string imgname(plikSiatki.GetDirA() + "\\" + prefix + numEXT + "." + ext);

		bool imgFileExists = UI::fileExists(imgname);

		std::vector<BYTE> kolory;
		kolory.clear();

		UI::STATUSBAR::printfTimed(1000, "Reading layer nb %d", j);

		if (imgFileExists)
		{
			int depth;
			std::pair<int, int> wh = CMainApplication::loadImageRAW(UI::str2wstr(imgname), kolory, depth);

			w = wh.first;
			h = wh.second;

			std::vector<WORD>* layer = new std::vector<WORD>(h * w);

			int ix = 0;
			int iy = 0;

			for (int i = 0; i < kolory.size(); )
			{
				if (depth == 32)
				{
					CRGBA kolor = CRGBA::fromARGB32(&kolory[i]);
					if (256 * kolor.R() >= treshold)
					{
						layer->at(ix + iy * w) = 256 * kolor.R();
					}
					else
					{
						layer->at(ix + iy * w) = 0;
					}

					i += 4;
				}
				else if (depth == 16)
				{
					WORD kolor = *((WORD*)&kolory[i]);
					if (kolor >= treshold)
					{
						layer->at(ix + iy * w) = kolor;
						if (minData > kolor) minData = kolor;
						if (maxData < kolor) maxData = kolor;
					}
					else
					{
						layer->at(ix + iy * w) = 0;
					}

					i += 2;
				}
				else if (depth == 8)
				{
					BYTE kolor = kolory[i];
					if (256 * kolor >= treshold)
					{
						layer->at(ix + iy * w) = 255 * kolor;
					}
					else
					{
						layer->at(ix + iy * w) = 0;
					}

					i++;
				}


				ix++;
				if (ix >= w)
				{
					ix = 0;
					iy++;
				}
			}

			maskMap.push_back(layer);
		}
		iz += 1;

		UI::PROGRESSBAR::setValue(j);
	}

	return CTriple<double>(pxx, pxy, pxz);
}
size_t CParserTKD::removeInternalVoxelsOLD(int w, int h, std::vector<std::vector<WORD>*>& maskMap, WORD treshold)
{
	UI::PROGRESSBAR::init(0, maskMap.size(), 0);
	for (int z = 1; z < maskMap.size() - 1; z++)
	{
		UI::STATUSBAR::printfTimed(1000, "Cleaning layer %d", z);
		std::vector<WORD>* layer = maskMap[z];

		for (int y = 1; y < h - 1; y++)
		{
			for (int x = 1; x < w - 1; x++)
			{
				if (layer->at(x + y * w) != 0)
				{
					WORD& mask = layer->at(x + y * w);

					if (mask >= treshold)
					{
						// s¹siedzi "na wprost"
						if ((maskMap[z]->at(x - 1 + y * w) != 0)
							&& (maskMap[z]->at(x + (y - 1) * w) != 0)
							&& (maskMap[z - 1]->at(x + y * w) != 0)
							&& (maskMap[z]->at(x + 1 + y * w) != 0)
							&& (maskMap[z]->at(x + (y + 1) * w) != 0)
							&& (maskMap[z + 1]->at(x + y * w) != 0))
						{
							maskMap[z]->at(x + y * w) = 1;
						}

						//// s¹siedzi "po krawêdzi" wokó³ osi z
						//else  if ((maskMap[z]->at(x - 1 + (y - 1)*w) != 0)
						//	&& (maskMap[z]->at(x - 1 + (y + 1)*w) != 0)
						//	&& (maskMap[z]->at(x + 1 + (y + 1)*w) != 0)
						//	&& (maskMap[z]->at(x + 1 + (y - 1)*w) != 0))
						//{
						//	maskMap[z]->at(x + y*w) = 1;
						//}

						//// s¹siedzi "po krawêdzi" wokó³ osi y
						//else  if ((maskMap[z - 1]->at(x - 1 + y*w) != 0)
						//	&& (maskMap[z + 1]->at(x - 1 + y*w) != 0)
						//	&& (maskMap[z + 1]->at(x + 1 + y*w) != 0)
						//	&& (maskMap[z - 1]->at(x + 1 + y*w) != 0))
						//{
						//	maskMap[z]->at(x + y*w) = 1;
						//}

						//// s¹siedzi "po krawêdzi" wokó³ osi x
						//else  if ((maskMap[z - 1]->at(x + (y - 1)*w) != 0)
						//	&& (maskMap[z + 1]->at(x + (y - 1)*w) != 0)
						//	&& (maskMap[z + 1]->at(x + (y + 1)*w) != 0)
						//	&& (maskMap[z - 1]->at(x + (y + 1)*w) != 0))
						//{
						//	maskMap[z]->at(x + y*w) = 1;
						//}

						//// s¹siedzi "po wierzco³kach" wokó³ osi x
						//else  if ((maskMap[z - 1]->at(x - 1 + (y - 1)*w) != 0)
						//	&& (maskMap[z + 1]->at(x - 1 + (y - 1)*w) != 0)
						//	&& (maskMap[z + 1]->at(x - 1 + (y + 1)*w) != 0)
						//	&& (maskMap[z - 1]->at(x - 1 + (y + 1)*w) != 0)
						//	&& (maskMap[z - 1]->at(x + 1 + (y - 1)*w) != 0)
						//	&& (maskMap[z + 1]->at(x + 1 + (y - 1)*w) != 0)
						//	&& (maskMap[z + 1]->at(x + 1 + (y + 1)*w) != 0)
						//	&& (maskMap[z - 1]->at(x + 1 + (y + 1)*w) != 0))
						//{
						//	maskMap[z]->at(x + y*w) = 1;
						//}
					}
				}
			}
		}
		UI::PROGRESSBAR::setValue(z);
	}
}


size_t CParserTKD::Run()
{
	if (this->bIsNotSet) return 0;

	treshold.insert(255 * 144);
	treshold.insert(255 * 160);

	long lbv = 0;

	int w, h;

	WORD minData, maxData;

	CTriple<double> scale = readVoxelsOLD(w, h, maskMap, *treshold.begin(), minData, maxData);

	UI::STATUSBAR::printf("%d %d", minData, maxData);

	removeInternalVoxelsOLD(w, h, maskMap, *treshold.begin());

#if defined(TESTPOLYVOX)

	std::map<BYTE, CVolumetricData::VolumeType*> pVolData;
	for (std::set<BYTE>::reverse_iterator it = treshold.rbegin(); it != treshold.rend(); it++)
	{
		pVolData[*it] = new CVolumetricData::VolumeType(PolyVox::Region(0, 0, 0, w - 1, h - 1, maskMap.size() - 1));
	}

	uint8_t uDensity = CVolumetricData::VoxelType::getMaxDensity();  // ZAKRES: 0-15 !!!
	uint8_t uMaterial = 7; // mask / 16; // ZAKRES: 0-15 !!!

	CVolumetricData::VoxelType voxel; // = volData.getVoxelAt( ix, iy, iz );

	voxel.setDensity(uDensity);
	voxel.setMaterial(uMaterial);

#elif defined(TESTVOLTK)
	CVolTK* pVolTK = new CVolTK(m_model);

	pVolTK->m_winMin = minData;
	pVolTK->m_winMax = maxData;

#else
	std::map<WORD, CPointCloud*> pData;

	for (std::set<WORD>::reverse_iterator it = treshold.rbegin(); it != treshold.rend(); it++)
	{
		pData[*it] = new CPointCloud(m_model);
		pData[*it]->setLabel(L"t = " + std::to_wstring(*it));
	}
#endif


	for (int z = 0; z < maskMap.size(); z++)
	{
		UI::STATUSBAR::printfTimed(1000, "Copying layer %d", z);

		std::vector<WORD>* layer = maskMap[z];

		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				WORD mask = layer->at(x + y * w);

#if defined(TESTVOLTK)
				//pVolTK->addVoxelW(scale.X() * x, scale.Y() * y, scale.Z() * z, mask);
				pVolTK->addVoxelW(x, y, z, mask);
				lbv++;
#else
				std::set<WORD>::reverse_iterator it2 = treshold.rbegin();
				for (std::set<WORD>::reverse_iterator it = treshold.rbegin(); it != treshold.rend(); it++)
				{
					if ((mask >= *it) && ((it == treshold.rbegin()) || (mask < *it2)))
					{
#if defined(TESTPOLYVOX)
						pVolData[*it]->setVoxelAt(x, y, z, voxel);
						//#elif defined(TESTVOLTK)
						//						pVolTK[*it]->addVoxelW( x, y, z, mask );
#else
						BYTE c = mask / 255;
						pData[*it]->addVertex(CVertex(scale.X() * x, scale.Y() * y, scale.Z() * z), CRGBA(c, c, c, c));
#endif

						lbv++;
					}
					it2 = it;
				}
#endif
			}
		}
		UI::PROGRESSBAR::setValue(z);
	}

	UI::PROGRESSBAR::hide();

#if defined(TESTPOLYVOX)
	for (std::set<BYTE>::reverse_iterator it = treshold.rbegin(); it != treshold.rend(); it++)
	{
		CVolumetricData* v = new CVolumetricData(m_model);

		v->setVolume(pVolData[*it]);
		v->m_alpha = 255;
		v->m_type = 0;

		v->setMin(CPoint3f(0, 0, 0));
		v->setMax(CPoint3f(w - 1, h - 1, maskMap.size() - 1));

		v->setLabel(L"vol.t = " + std::to_wstring(*it));

		m_model->addChild(v);
	}
	m_model->getTransform().setScale(scale.X());

#elif defined(TESTVOLTK)
	pVolTK->setLabel(L"volTK.t");
	pVolTK->m_voxelSize = scale;

	m_model->addChild(pVolTK);
#else
	for (std::set<WORD>::reverse_iterator it = treshold.rbegin(); it != treshold.rend(); it++)
	{
		CPoint3f ctr = pData[*it]->getCenterOfWeight();

		for (int i = 0; i < pData[*it]->vertices().size(); i++)
		{
			pData[*it]->vnormals().push_back(CVector3f(ctr, pData[*it]->vertices()[i]));
		}

		m_model->addChild(pData[*it]);
		pData[*it]->afterLoad(m_model);
	}
#endif

	return lbv;
}

#endif
