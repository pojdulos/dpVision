#include "ParserDICOM.h"
#include "Utilities.h"

#include <fstream>
#include <sstream>

#include "Image.h"
#include "AP.h"

#include "dpLog.h"
#include <QMessageLogger>

CParserDICOM::CParserDICOM()
{
	setDescr("DICOM files");
	m_exts.insert("dcm");
}

CParserDICOM::~CParserDICOM(void)
{
}



int convert(std::string srcFname, std::string dstFname);


std::string CParserDICOM::decomposeFname(std::string fname, std::string& number)
{
	size_t last_index = fname.find_last_not_of("0123456789");
	number = fname.substr(last_index + 1);

	return fname.substr(0, last_index+1);
}


size_t CParserDICOM::parseSingleFile( std::string currentDir, std::string shortName, std::string currentNumber, std::string extension )
{
	std::string tmpJPGname(currentDir + "/" + shortName + currentNumber + "_tmp");
	std::string tmpJPGext(".jpg");

	UI::STATUSBAR::setText("Please wait. I'm converting .dcm to .jpg ... (" + currentNumber + ")");

	size_t framesCount = convert(currentDir + "/" + shortName + currentNumber + "." + extension, tmpJPGname + tmpJPGext);

	size_t lbv = 0;

	float iz = std::stof(currentNumber) / 10.0f;

	UI::PROGRESSBAR::init(0, framesCount, 0);

	for (int j = 0; j < framesCount; j++)
	{
		m_model->addChild(pMeshData = new CMesh(m_model));

		std::string numEXT("_");
		if (j < 100) numEXT += "0";
		if (j < 10) numEXT += "0";
		numEXT += std::to_string(j);

		pMeshData->setLabel(UI::str2wstr("Layer " + currentNumber + "_" + numEXT));

		std::string imgname(tmpJPGname + numEXT + tmpJPGext);
		bool imgFileExists = UI::fileExists(imgname);

		std::vector<CRGBA> kolory;

		kolory.clear();


		UI::STATUSBAR::printfTimed(1000, "Reaing layer nb %d", j);
		UI::PROGRESSBAR::setValue(j);

		if (imgFileExists)
		{
			std::pair<int, int> wh = CImage::loadToRGBAvector(UI::str2wstr(imgname), kolory, 32);

			int w = wh.first;
			int h = wh.second;

			int ix = 0;
			int iy = 0;

			for (std::vector<CRGBA>::iterator it = kolory.begin(); it != kolory.end(); it++)
			{
				if ((*it).red() > 0)
				{
					pMeshData->addVertex(CVertex((float)ix / 10.0f, (float)iy / 10.0f, iz), *it);

					lbv++;
				}

				ix++;
				if (ix >= w)
				{
					ix = 0;
					iy++;
				}
			}

			UI::FILESYSTEM::deleteFile(imgname);
		}
		iz += 1;

		pMeshData->afterLoad(m_model);
	}

	UI::PROGRESSBAR::hide();

	return lbv;
}




size_t CParserDICOM::parseSingleDICOM(std::string dicomPath, std::string currentNumber)
{
	imebra::DataSet* dataSet = imebra::CodecFactory::load(dicomPath);

	double sliceThick = dataSet->getDouble(imebra::TagId(imebra::tagId_t::SliceThickness_0018_0050), 0);
	
	double pixelSize[2] = {
		dataSet->getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 0),
		dataSet->getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 1)
	};

	double imagePosition[3] = {
		dataSet->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 0),
		dataSet->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 1),
		dataSet->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 2)
	};

	//imebra::ModalityVOILUT
	//std::unique_ptr<imebra::Image> image0m(dataSet->getImageApplyModalityTransform(0));

	std::unique_ptr<imebra::Image> image0(dataSet->getImage(0));

	size_t w = image0->getWidth();
	size_t h = image0->getHeight();
	imebra::bitDepth_t d = image0->getDepth();

	auto data = image0->getReadingDataHandler();

	size_t dsize;
	const char* buffer = data->data(&dsize);

	size_t lbv = 0;


	m_model->addChild(pMeshData = new CMesh(m_model));
	pMeshData->setLabel(UI::str2wstr("Layer " + currentNumber));

	UI::PROGRESSBAR::init(0, h, 0);

	for (size_t y = 0; y < h; y++)
	{
		UI::PROGRESSBAR::setValue(y);
		for (size_t x = 0; x < w; x++) 
		{
			size_t idx = (w * y + x);

			ushort pix = ((ushort*)buffer)[idx];

			if (pix > 2000)
			{
				float intensity = (float)pix / 4095.0f;

				CRGBA c(intensity, intensity, intensity, 0.25f);
				CVertex v( imagePosition[0]+ pixelSize[0]*x, imagePosition[1]+pixelSize[1]*y, imagePosition[2] );

				pMeshData->addVertex(v, c);

				lbv++;
			}

		}
	}

	UI::PROGRESSBAR::hide();

	delete dataSet;

	pMeshData->afterLoad(m_model);

	return lbv;
}

#include "VolTK.h"

size_t CParserDICOM::parseSingleDICOM2volTK(std::string dicomPath, uint16_t slide, RAWdicomData& kostka, double& sliceLocation, bool isReversed)
{
	imebra::DataSet* dataSet = imebra::CodecFactory::load(dicomPath);

	uint32_t nbOfFrames = 1;
	try {
		nbOfFrames = dataSet->getUnsignedLong(imebra::TagId(imebra::tagId_t::NumberOfFrames_0028_0008), 0);
	}
	catch (...) {};


	//double pixelSize[2] = {
	//	dataSet->getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 0),
	//	dataSet->getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 1)
	//};



	CVector3d imagePosition(0.0, 0.0, 0.0);
	try {
		imagePosition.x = dataSet->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 0);
		imagePosition.y = dataSet->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 1);
		sliceLocation = imagePosition.z = dataSet->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 2);
		qInfo() << "ImagePositionPatient_0020_0032: " << imagePosition.x << imagePosition.y << imagePosition.z;
	}
	catch (...)
	{
		try {
			imagePosition.z = sliceLocation = dataSet->getDouble(imebra::TagId(imebra::tagId_t::SliceLocation_0020_1041), 0);
		}
		catch (...)
		{
			imagePosition.z = sliceLocation;
		}
		qInfo() << "[" << imagePosition.z <<"]\n";
	}

	double sliceThickness = dataSet->getDouble(imebra::TagId(imebra::tagId_t::SliceThickness_0018_0050), 0);
	qInfo() << "sliceThickness: " << sliceThickness << endl;
	
	if (isReversed)
	{
		imagePosition.z = imagePosition.z - sliceThickness;
	}

	kostka.m_imgPos.push_back(imagePosition);

	double gantryDetectorTilt = 0.0;
	try {
		gantryDetectorTilt = dataSet->getDouble(imebra::TagId(imebra::tagId_t::GantryDetectorTilt_0018_1120), 0);
		//qInfo() << "Gantry Detector Tilt " << gantryDetectorTilt;
	}
	catch (...)
	{
		;
	}

	kostka.m_gantra.push_back(gantryDetectorTilt);

	//imebra::ModalityVOILUT
	//std::unique_ptr<imebra::Image> image0m(dataSet->getImageApplyModalityTransform(0));

	size_t frameNumber = 0;
	
	//std::unique_ptr<imebra::Image> image0(nullptr);
	imebra::Image *image0(nullptr);

	size_t lbv = 0;

	bool isOK = true;

	while (isOK && (frameNumber<nbOfFrames))
	{
		try {
			image0 = dataSet->getImage(frameNumber);
		}
		catch (...) {
			isOK = false;
		}

		if (isOK)
		{
			size_t w = image0->getWidth();
			size_t h = image0->getHeight();
			imebra::bitDepth_t d = image0->getDepth();

			auto dataHandler = image0->getReadingDataHandler();

			short layerNb = slide + frameNumber;
			char* dstBuffer = (char*)kostka.layer(layerNb);

			size_t dsize;
			const char* srcBuffer = dataHandler->data(&dsize);
			memcpy(dstBuffer, srcBuffer, dsize);

			lbv += w * h;
			delete image0;
		}

		frameNumber++;
	}
	delete dataSet;

	return lbv;
}


void CParserDICOM::displayDicomTags(std::string fileName)
{
	std::cout << "czytam tagi z pliku: " << fileName;

	imebra::DataSet* dataSet = imebra::CodecFactory::load(fileName);

	auto tags = dataSet->getTags();

	//std::map<std::string, std::string> result;

	for (auto t : tags)
	{
		std::string name = "{unknown name}";
		std::string value = "{unknown value}";

		try {
			try {
				name = imebra::DicomDictionary::getTagName(t);
			}
			catch (...) {
				qInfo() << QString("%1 error:GetTagName").arg(t.getTagId());
			};

			//imebra::Tag *tag0 = dataSet->getTag(t);
			//auto dt = tag0->getDataType();

			if (!name.compare("Image Position (Patient)"))
			{
				value = dataSet->getString(t, 0) + ", " + dataSet->getString(t, 1) + ", " + dataSet->getString(t, 2);
			}
			else if (!name.compare("Pixel Spacing"))
			{
				value = dataSet->getString(t, 0) + ", " + dataSet->getString(t, 1);
			}
			else
			{
				value = dataSet->getString(t, 0);
			}
		}
		catch (...) {
			qInfo() << QString::asprintf("%u error:getString", t.getTagId());
		};

		//result[name] = value;
		qInfo() << QString("%1: %2").arg(QString::fromStdString(name)).arg(QString::fromStdString(value));
	}
}

long CParserDICOM::readFromFirstToLast(RAWdicomData& kostka, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix)
{
	UI::STATUSBAR::setText("Please wait. I'm reading .dcm...");

	long lbv = 0;
	double zSize = 1.0;
	double sliceLoc = 0.0;
	double lastsliceLoc = 0.0;

	UI::PROGRESSBAR::init(0, nbOfFiles, 0);
	UI::PROGRESSBAR::setText("Czytam pliki: ");
	for (int slide = 0; slide < nbOfFiles; slide++)
	{
		UI::PROGRESSBAR::setValue(slide);

		std::string currentPath = dirPath + "/" + shortName;

		std::string currentNumber = std::to_string(std::stoi(firstNumber) + slide);
		currentNumber = std::string(firstNumber.length() - std::min(firstNumber.length(), currentNumber.length()), '0') + currentNumber;
		currentPath += currentNumber;

		currentPath += "." + suffix;

		lastsliceLoc = sliceLoc;
		lbv += parseSingleDICOM2volTK(currentPath, slide, kostka, sliceLoc);
	}
	UI::PROGRESSBAR::hide();

	return lbv;
}

long CParserDICOM::readFromLastToFirst(RAWdicomData& kostka, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix)
{
	UI::STATUSBAR::setText("Please wait. I'm reading .dcm...");

	long lbv = 0;
	double zSize = 1.0;
	double sliceLoc = 0.0;
	double lastsliceLoc = 0.0;

	UI::PROGRESSBAR::init(0, nbOfFiles, 0);
	UI::PROGRESSBAR::setText("Czytam pliki: ");
	for (int slide = 0; slide < nbOfFiles; slide++)
	{
		UI::PROGRESSBAR::setValue(slide);

		int trueSlideNumber = nbOfFiles - (slide + 1);

		std::string currentPath = dirPath + "/" + shortName;

		std::string currentNumber = std::to_string(std::stoi(firstNumber) + trueSlideNumber);
		currentNumber = std::string(firstNumber.length() - std::min(firstNumber.length(), currentNumber.length()), '0') + currentNumber;
		currentPath += currentNumber;

		currentPath += "." + suffix;

		lastsliceLoc = sliceLoc;
		lbv += parseSingleDICOM2volTK(currentPath, slide, kostka, sliceLoc, true);
	}
	UI::PROGRESSBAR::hide();

	return lbv;
}

size_t CParserDICOM::Run()
{
	if (this->bIsNotSet) return 0;

	qInfo() << "Wybrano " << plikSiatki.absoluteFilePath();

	std::string firstNumber;
	std::string shortName = decomposeFname(plikSiatki.completeBaseNameA(), firstNumber);

	bool singleFile = firstNumber.empty();
	uint32_t nbOfFiles = 1;

	std::string lastNumber = firstNumber;
	std::string lastPath = plikSiatki.absoluteFilePathA();

	std::string currentNumber = firstNumber;
	std::string currentPath = plikSiatki.absolutePathA() + "/" + shortName + currentNumber + "." + plikSiatki.suffixA();

	if (!singleFile)
	{
		while (UI::fileExists(currentPath))
		{
			lastNumber = currentNumber;
			lastPath = currentPath;

			currentNumber = std::to_string(std::stoi(currentNumber) + 1);
			currentPath = plikSiatki.absolutePathA() + "/" + shortName + currentNumber + "." + plikSiatki.suffixA();

			if (!UI::fileExists(currentPath)) {
				currentNumber = std::string(firstNumber.length() - std::min(firstNumber.length(), currentNumber.length()), '0') + currentNumber;
				currentPath = plikSiatki.absolutePathA() + "/" + shortName + currentNumber + "." + plikSiatki.suffixA();
			}
		}

		nbOfFiles = std::stoi(currentNumber) - std::stoi(firstNumber);
	}

	displayDicomTags(plikSiatki.absoluteFilePathA());

	std::cout << "czytam tagi z pierwszego pliku: " << plikSiatki.absoluteFilePathA() << std::endl;
	imebra::DataSet* dataSet0 = imebra::CodecFactory::load(plikSiatki.absoluteFilePathA());
	auto tags0 = dataSet0->getTags();

	m_tags.photometric = dataSet0->getString(imebra::TagId(imebra::tagId_t::PhotometricInterpretation_0028_0004), 0);
	
	if (m_tags.photometric.compare("MONOCHROME2"))
	{
		UI::MESSAGEBOX::error("Sorry. Now I can read monochrome DICOMs only.");
		return 0;
	}

	m_tags.pixelRepresentation = dataSet0->getUnsignedLong(imebra::TagId(imebra::tagId_t::PixelRepresentation_0028_0103), 0);


	m_tags.height = dataSet0->getUnsignedLong(imebra::TagId(imebra::tagId_t::Rows_0028_0010), 0);
	m_tags.width = dataSet0->getUnsignedLong(imebra::TagId(imebra::tagId_t::Columns_0028_0011), 0);

	m_tags.depth = dataSet0->getUnsignedLong(imebra::TagId(imebra::tagId_t::BitsAllocated_0028_0100), 0);
	


	CVolTK* pVolTK = new CVolTK(m_model, m_tags.width, m_tags.height, nbOfFiles, m_tags.depth);
	pVolTK->setLabel("volTK.t");
	m_model->addChild(pVolTK);

	pVolTK->m_div = 1;

	CPoint3d tmpVoxelSize;

	try {
		
		tmpVoxelSize = CPoint3d(
			dataSet0->getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 0),
			dataSet0->getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 1),
			dataSet0->getDouble(imebra::TagId(imebra::tagId_t::SliceThickness_0018_0050), 0));
	}
	catch (...)
	{
		tmpVoxelSize = CPoint3d(1.0, 1.0, 1.0);
	}



	try {
		pVolTK->m_alpha = dataSet0->getDouble(imebra::TagId(imebra::tagId_t::GantryDetectorTilt_0018_1120), 0);
	}
	catch (...)
	{
		pVolTK->m_alpha = 0.0;
	}

	CVector3d imagePosition0( 0.0, 0.0, 0.0 );
	double sliceLocation0 = 0.0;

	try {
		imagePosition0.x = dataSet0->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 0);
		imagePosition0.y = dataSet0->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 1);
		sliceLocation0 = imagePosition0.z = dataSet0->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 2);
	}
	catch (...)
	{
		try {
			imagePosition0.z = sliceLocation0 = dataSet0->getDouble(imebra::TagId(imebra::tagId_t::SliceLocation_0020_1041), 0);
		}
		catch (...)	{}
	}

	m_tags.rescaleSlope = dataSet0->getSignedLong(imebra::TagId(imebra::tagId_t::RescaleSlope_0028_1053), 0);
	m_tags.rescaleIntercept = dataSet0->getSignedLong(imebra::TagId(imebra::tagId_t::RescaleIntercept_0028_1052), 0);
	//m_tags.rescaleType;
	m_tags.windowCenter = dataSet0->getSignedLong(imebra::TagId(imebra::tagId_t::WindowCenter_0028_1050), 0);;
	m_tags.windowWidth = dataSet0->getSignedLong(imebra::TagId(imebra::tagId_t::WindowWidth_0028_1051), 0);;


	delete dataSet0;


	std::cout << "czytam tagi z ostatniego pliku: " << lastPath << std::endl;
	imebra::DataSet* dataSet1 = imebra::CodecFactory::load(lastPath);
	auto tags1 = dataSet1->getTags();

	CVector3d imagePosition1(0.0, 0.0, 0.0);
	double sliceLocation1 = std::stoi(lastNumber);
	try {
		imagePosition1.x = dataSet1->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 0);
		imagePosition1.y = dataSet1->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 1);
		sliceLocation1 = imagePosition1.z = dataSet1->getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 2);
	}
	catch (...)
	{
		try {
			imagePosition1.z = sliceLocation1 = dataSet1->getDouble(imebra::TagId(imebra::tagId_t::SliceLocation_0020_1041), 0);
		}
		catch (...) {}
	}

	delete dataSet1;


	size_t lbv = 0;

	RAWdicomData tmpKostka(m_tags.width, m_tags.height, nbOfFiles);

	if (sliceLocation0 <= sliceLocation1)
	{
		qInfo() << endl << "normalna kolejnosc, czytam od pierwszego do ostatniego !" << endl;

		lbv += readFromFirstToLast(tmpKostka, nbOfFiles, plikSiatki.absolutePathA(), shortName, firstNumber, plikSiatki.suffixA());
	}
	else
	{
		qInfo() << endl << "odwrocona kolejnosc, czytam od ostatniego do pierwszego !" << endl;

		lbv += readFromLastToFirst(tmpKostka, nbOfFiles, plikSiatki.absolutePathA(), shortName, firstNumber, plikSiatki.suffixA());
		//m_tags.firstImagePosition = imagePosition1;
		//m_tags.lastImagePosition = imagePosition0;
	}

	m_tags.firstImagePosition = CVector3d(CPoint3d(0), tmpKostka.m_imgPos.front());// imagePosition0;
	m_tags.lastImagePosition = CVector3d(CPoint3d(0), tmpKostka.m_imgPos.back());// imagePosition1;

	tmpKostka.m_voxelSize = tmpVoxelSize;

	tmpKostka.pixelRepresentation = m_tags.pixelRepresentation;
	tmpKostka.rescaleIntercept = m_tags.rescaleIntercept;
	tmpKostka.rescaleSlope = m_tags.rescaleSlope;
	tmpKostka.windowCenter = m_tags.windowCenter;
	tmpKostka.windowWidth = m_tags.windowWidth;

	if (tmpKostka.size() > 0) {
		if (pVolTK->kostka.init(tmpKostka.m_cols, tmpKostka.m_rows, tmpKostka.m_lays))
		{
			pVolTK->kostka.m_imgPos = tmpKostka.m_imgPos;
			pVolTK->kostka.m_gantra = tmpKostka.m_gantra;
			pVolTK->kostka.m_voxelSize = tmpKostka.m_voxelSize;

			for (int l = 0; l < tmpKostka.m_lays; l++)
				for (int r = 0; r < tmpKostka.m_rows; r++)
					for (int c = 0; c < tmpKostka.m_cols; c++)
					{
						pVolTK->kostka.set(c, r, l, tmpKostka.getRescaled(c, r, l, 0, 65535));
					}
		}
	}

	pVolTK->createDisplayData();

	qInfo()
		<< pVolTK->m_currentIntensityRange.lower
		<< pVolTK->m_currentIntensityRange.upper
		<< pVolTK->m_dataIntensityRange.lower
		<< pVolTK->m_dataIntensityRange.upper
		<< endl;

	//pVolTK->m_winMax = pVolTK->m_dataIntensityRange.upper;

	pVolTK->setGeometry();

	m_model->importChildrenGeometry();
	m_model->transform().translation() = m_tags.firstImagePosition;

	return lbv;
}
