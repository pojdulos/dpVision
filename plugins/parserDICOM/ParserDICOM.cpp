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
	m_save_exts.insert("dcm");
}

CParserDICOM::~CParserDICOM(void)
{
}

#include "imebra/dataSet.h"
//
//
//int convert(std::string srcFname, std::string dstFname);
//
//
std::string CParserDICOM::decomposeFname(std::string fname, std::string& number)
{
	size_t last_index = fname.find_last_not_of("0123456789");
	number = fname.substr(last_index + 1);

	return fname.substr(0, last_index+1);
}

std::vector<float> convertInt16ToFloat(const char* srcBuffer, size_t size) {
	// Upewniamy siê, ¿e rozmiar jest wielokrotnoœci¹ 2
	if (size % 2 != 0) {
		throw std::invalid_argument("Size must be a multiple of 2.");
	}

	size_t numElements = size / 2; // Poniewa¿ ka¿dy int16_t to 2 bajty
	Volumetric::SliceDataType result(numElements);

	// Przechodzimy przez dane w krokach po 2 bajty (16 bitów)
	for (size_t i = 0; i < numElements; ++i) {
		int16_t value;
		std::memcpy(&value, srcBuffer + 2 * i, sizeof(int16_t)); // Kopiujemy 2 bajty z bufora do value
		result[i] = static_cast<float>(value); // Konwertujemy int16_t do float
	}

	return result;
}


size_t CParserDICOM::parse_dicom_file(std::string dicomPath, uint16_t slide, Volumetric::SliceDataType &slice, Volumetric::SliceMetadata &metadata, bool isReversed)
{
	//qInfo() << "Czytam plaster " << slide << Qt::endl;

	imebra::DataSet dataSet = imebra::CodecFactory::load(dicomPath);

	uint32_t nbOfFrames = 1;
	try {
		nbOfFrames = dataSet.getUnsignedLong(imebra::TagId(imebra::tagId_t::NumberOfFrames_0028_0008), 0);

		if (nbOfFrames > 1) {
			qInfo() << "WIELOWARSTWOWY PLIK DICOM" << Qt::endl;
		}
	}
	catch (...) {};

	//metadata.image_position_patient[0] = 0.0f;
	//metadata.image_position_patient[1] = 0.0f;
	//metadata.image_position_patient[2] = 0.0f;

	//metadata.image_position_patient[2] = metadata.slice_location = slide;

	try {
		metadata.image_position_patient[0] = dataSet.getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 0);
		metadata.image_position_patient[1] = dataSet.getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 1);
		metadata.slice_location = metadata.image_position_patient[2] = dataSet.getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 2);

		//qInfo() << "ImagePositionPatient_0020_0032: " << metadata.image_position_patient[0] << metadata.image_position_patient[1] << metadata.image_position_patient[2];
	}
	catch (...)
	{
		try {
			metadata.image_position_patient[2] = metadata.slice_location = dataSet.getDouble(imebra::TagId(imebra::tagId_t::SliceLocation_0020_1041), 0);
		}
		catch (...)
		{
			metadata.image_position_patient[2] = metadata.slice_location;
		}
		// qInfo() << "[" << metadata.image_position_patient[2] << "]\n";
	}

	try {
		metadata.pixel_spacing[0] = dataSet.getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 0);
		metadata.pixel_spacing[1] = dataSet.getDouble(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 1);

		//qInfo() << "PixelSpacing_0028_0030: " << metadata.pixel_spacing[0] << metadata.pixel_spacing[1];
	}
	catch (...)
	{
		;
	}

	metadata.slice_distance = metadata.slice_thickness = dataSet.getDouble(imebra::TagId(imebra::tagId_t::SliceThickness_0018_0050), 0);

	//qInfo() << "sliceThickness: " << metadata.slice_thickness << endl;

	//if (isReversed)
	//{
	//	// SPRAWDZIÆ TO KONIECZNIE
	//	metadata.image_position_patient[2] = metadata.image_position_patient[2] - metadata.slice_thickness;
	//}

	try {
		metadata.gantry_detector_tilt = dataSet.getDouble(imebra::TagId(imebra::tagId_t::GantryDetectorTilt_0018_1120), 0);
		// qInfo() << "Gantry Detector Tilt " << metadata.gantry_detector_tilt;
	}
	catch (...)
	{
		;
	}

	try {
		metadata.rescale_intercept = dataSet.getDouble(imebra::TagId(imebra::tagId_t::RescaleIntercept_0028_1052), 0);
		metadata.rescale_slope = dataSet.getDouble(imebra::TagId(imebra::tagId_t::RescaleSlope_0028_1053), 1);
	}
	catch (...)
	{
		metadata.rescale_intercept = 0.0;
		metadata.rescale_slope = 1.0;
	}


	size_t frameNumber = 0;

	//std::unique_ptr<imebra::Image> image0(nullptr);
//	imebra::Image image0(nullptr);

	size_t lbv = 0;

	bool isOK = true;

	imebra::Image image0(dataSet.getImage(frameNumber));
	//try {
	//	image0 = (imebra::Image) dataSet.getImage(frameNumber);
	//}
	//catch (...) {
	//	isOK = false;
	//}

	
	if (isOK)
	{
		size_t w = image0.getWidth();
		size_t h = image0.getHeight();
		imebra::bitDepth_t d = image0.getDepth();

		auto dataHandler = image0.getReadingDataHandler();

		size_t dsize;
		const char* srcBuffer = dataHandler.data(&dsize);
		
		if ((metadata.rescale_intercept != 0) || (metadata.rescale_slope != 1.0)) {
			Volumetric::SliceDataType tmp_slice = convertInt16ToFloat(srcBuffer, dsize);
			slice.resize(tmp_slice.size());
			float a = metadata.rescale_slope;
			float b = metadata.rescale_intercept;
			std::transform(tmp_slice.begin(), tmp_slice.end(), slice.begin(), [a, b](float val) { return val*a + b; } );
		}
		else {
			slice = convertInt16ToFloat(srcBuffer, dsize);
		}

		qInfo() << "OK: " << slice.size() << Qt::endl;

		//delete image0;
	}

	//delete dataSet;
	return 1;
}

long CParserDICOM::read_files(Volumetric* volum, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix)
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

		Volumetric::SliceDataType slice;
		Volumetric::SliceMetadata metadata;

		lbv += parse_dicom_file(currentPath, slide, slice, metadata, false);

		volum->m_volume.push_back(slice);
		volum->metadata.push_back(metadata);
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

//	displayDicomTags(plikSiatki.absoluteFilePathA());

	std::cout << "czytam tagi z pierwszego pliku: " << plikSiatki.absoluteFilePathA() << std::endl;
	imebra::DataSet dataSet0 = imebra::CodecFactory::load(plikSiatki.absoluteFilePathA());
	auto tags0 = dataSet0.getTags();

	std::string photometric = dataSet0.getString(imebra::TagId(imebra::tagId_t::PhotometricInterpretation_0028_0004), 0);
	
	if (photometric.compare("MONOCHROME2"))
	{
		UI::MESSAGEBOX::error("Sorry. Now I can read monochrome DICOMs only.");
		return 0;
	}

	int pixelRepresentation = dataSet0.getUnsignedLong(imebra::TagId(imebra::tagId_t::PixelRepresentation_0028_0103), 0);
	int depth = dataSet0.getUnsignedLong(imebra::TagId(imebra::tagId_t::BitsAllocated_0028_0100), 0);


	/* ROZMIAR POJEDYNCZEGO PLASTRA */
	int rows = dataSet0.getUnsignedLong(imebra::TagId(imebra::tagId_t::Rows_0028_0010), 0);
	int columns = dataSet0.getUnsignedLong(imebra::TagId(imebra::tagId_t::Columns_0028_0011), 0);


	/* SPRAWDZAM KOLEJNOŒÆ PLASTRÓW */

	double sliceLocation0 = 0.0;

	try {
		sliceLocation0 = dataSet0.getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 2);
	}
	catch (...)
	{
		try {
			sliceLocation0 = dataSet0.getDouble(imebra::TagId(imebra::tagId_t::SliceLocation_0020_1041), 0);
		}
		catch (...)	{}
	}
	//delete dataSet0;

	std::cout << "czytam tagi z ostatniego pliku: " << lastPath << std::endl;

	imebra::DataSet dataSet1 = imebra::CodecFactory::load(lastPath);
	auto tags1 = dataSet1.getTags();

	double sliceLocation1 = std::stoi(lastNumber);
	try {
		sliceLocation1 = dataSet1.getDouble(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 2);
	}
	catch (...)
	{
		try {
			sliceLocation1 = dataSet1.getDouble(imebra::TagId(imebra::tagId_t::SliceLocation_0020_1041), 0);
		}
		catch (...) {}
	}
	//delete dataSet1;


	/* DOPIERO TU NAST¥PI W£AŒCIWE ODCZYTYWANIE DANYCH */

	Volumetric* volum = new Volumetric();
	volum->setLabel("dicom file");

	read_files(volum, nbOfFiles, plikSiatki.absolutePathA(), shortName, firstNumber, plikSiatki.suffixA());

	if (sliceLocation0 > sliceLocation1)
	{
		qInfo() << endl << "odwrocona kolejnosc -> odwracam wektory" << endl;
		std::reverse(volum->m_volume.begin(), volum->m_volume.end());
		std::reverse(volum->metadata.begin(), volum->metadata.end());
	}
	//if (sliceLocation0 <= sliceLocation1)
	//{
	//	qInfo() << endl << "normalna kolejnosc, czytam od pierwszego do ostatniego !" << endl;
	//	readFromFirstToLast(volum, nbOfFiles, plikSiatki.absolutePathA(), shortName, firstNumber, plikSiatki.suffixA());
	//}
	//else
	//{
	//	qInfo() << endl << "odwrocona kolejnosc, czytam od ostatniego do pierwszego !" << endl;
	//	readFromLastToFirst(volum, nbOfFiles, plikSiatki.absolutePathA(), shortName, firstNumber, plikSiatki.suffixA());
	//}

	volum->m_shape[0] = volum->m_volume.size();
	volum->m_shape[1] = rows;
	volum->m_shape[2] = columns;

	volum->adjustMinMax();

	//volum->m_minDisplWin = 600.0f;

	m_model->addChild(volum);
	m_model->importChildrenGeometry();

	return volum->m_shape[0] * volum->m_shape[1] * volum->m_shape[2];
}

imebra::Image CParserDICOM::createImebraImageFromVoxels(const std::vector<float>& voxels, std::uint32_t width, std::uint32_t height) {
	if (voxels.size() != width * height) {
		throw std::invalid_argument("Voxel data size does not match the specified dimensions.");
	}

	// Create the image
	imebra::MutableImage image(width, height, imebra::bitDepth_t::depthS16, "MONOCHROME2", 15);
	
	// Get the raw data handler for writing data
	auto writingHandler = image.getWritingDataHandler();

	// Fill the buffer with voxel data, scaling the float values to fit the uint16_t range
	for (size_t i = 0; i < voxels.size(); ++i) {
		std::int16_t val = static_cast<std::int16_t>(round(voxels[i]));
		writingHandler.setInt16(i, val);
	}

	return image;
}


bool CParserDICOM::_export(Volumetric* volum, QString dir_name, QString fname_base)
{
	QDir dir(dir_name);
	if (!dir.exists()) dir.mkdir(".");


	UI::PROGRESSBAR::init(0, volum->m_shape[0], 0);
	UI::PROGRESSBAR::setText("Zapisuje pliki: ");

	for (int i = 0; i < volum->m_shape[0]; i++)
	{
		UI::PROGRESSBAR::setValue(i);

		imebra::MutableDataSet ds("1.2.840.10008.1.2.1");// , "ISO 2022 IR 6");

		ds.setString(imebra::TagId(imebra::tagId_t::PatientName_0010_0010), "Anonymous");
		ds.setString(imebra::TagId(imebra::tagId_t::PatientID_0010_0020), "123456");
		ds.setString(imebra::TagId(imebra::tagId_t::Modality_0008_0060), "MR");
		ds.setString(imebra::TagId(imebra::tagId_t::SeriesDescription_0008_103E), "generated with dpVision (c++) software");
		ds.setUnsignedLong(imebra::TagId(imebra::tagId_t::Rows_0028_0010), volum->m_shape[1]);
		ds.setUnsignedLong(imebra::TagId(imebra::tagId_t::Columns_0028_0011), volum->m_shape[2]);

		try {
			imebra::WritingDataHandler wh = ds.getWritingDataHandler(imebra::TagId(imebra::tagId_t::PixelSpacing_0028_0030), 0, imebra::tagVR_t::DS);
			wh.setSize(2);
			wh.setDouble(0, volum->metadata[i].pixel_spacing[0]);
			wh.setDouble(1, volum->metadata[i].pixel_spacing[1]);
		}
		catch (const std::exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
			return false;
		}

		ds.setUnsignedLong(imebra::TagId(imebra::tagId_t::BitsAllocated_0028_0100), 16);
		ds.setUnsignedLong(imebra::TagId(imebra::tagId_t::BitsStored_0028_0101), 16);
		ds.setUnsignedLong(imebra::TagId(imebra::tagId_t::HighBit_0028_0102), 15);
		ds.setSignedLong(imebra::TagId(imebra::tagId_t::PixelRepresentation_0028_0103), 1);

		ds.setString(imebra::TagId(imebra::tagId_t::PhotometricInterpretation_0028_0004), "MONOCHROME2");

		ds.setUnsignedLong(imebra::TagId(imebra::tagId_t::SamplesPerPixel_0028_0002), 1);

		try {
			imebra::WritingDataHandler wh = ds.getWritingDataHandler(imebra::TagId(imebra::tagId_t::ImagePositionPatient_0020_0032), 0, imebra::tagVR_t::DS);
			wh.setSize(3);
			wh.setDouble(0, volum->metadata[i].image_position_patient[0]);
			wh.setDouble(1, volum->metadata[i].image_position_patient[1]);
			wh.setDouble(2, volum->metadata[i].image_position_patient[2]);
		}
		catch (const std::exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
			return false;
		}

		ds.setDouble(imebra::TagId(imebra::tagId_t::SliceLocation_0020_1041), volum->metadata[i].image_position_patient[2]);
		ds.setDouble(imebra::TagId(imebra::tagId_t::SliceThickness_0018_0050), volum->metadata[i].slice_thickness);

		ds.setDouble(imebra::TagId(imebra::tagId_t::RescaleIntercept_0028_1052), 0.0); // volum->metadata[i].rescale_intercept);
		ds.setDouble(imebra::TagId(imebra::tagId_t::RescaleSlope_0028_1053), 1.0); // volum->metadata[i].rescale_slope);

		ds.setDouble(imebra::TagId(imebra::tagId_t::GantryDetectorTilt_0018_1120), 0.0); // volum->metadata[i].rescale_slope);

		//imebra::DataSet loadedDataSet = imebra::CodecFactory::load("v:/test/117690.jpg");
		//imebra::Image img(loadedDataSet.getImage(0));

		imebra::Image img = createImebraImageFromVoxels(volum->m_volume[i], volum->m_shape[2], volum->m_shape[1]);

		ds.setImage(0, img, imebra::imageQuality_t::high);

		QString fname(fname_base + QString("%1.dcm").arg(i, 3, 10, QChar('0')));
		imebra::CodecFactory::save(ds, dir.absoluteFilePath(fname).toStdString(), imebra::codecType_t::dicom);
	}
	UI::PROGRESSBAR::hide();

	return true;
}

bool CParserDICOM::save(CModel3D* obj, const QString path)
{
	QFileInfo finfo(path);
	return _export((Volumetric*)obj->getChild(), finfo.absolutePath(), finfo.baseName());
}

bool CParserDICOM::save(QVector<CBaseObject*> objects, const QString path)
{
	QFileInfo finfo(path);
	return _export((Volumetric*)objects[0], finfo.absolutePath(), finfo.baseName());
}
