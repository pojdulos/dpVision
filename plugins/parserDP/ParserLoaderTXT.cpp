#include "ParserLoaderTXT.h"

#include "UI.h"

ParserLoaderTXT::ParserLoaderTXT(QString filename, QObject* parent) : ThreadParserLoader(filename, parent)
{
	inputFile.setFileName(filename);

	if ( ! inputFile.open(QIODevice::ReadOnly | QFile::Text) )
	{
		UI::STATUSBAR::setText("Can't open file.");
	}

	in.setDevice(&inputFile);

	itIsOrderedCloud = false;
	hasReflections = false;
	hasAngles = false;
	hasDistances = false;

	hasFloatXYZ = false;

	hasColor = false;

	hasFloatRGB = false;
	hasFloatAlpha = false;
	hasByteRGB = false;
	hasByteAlpha = false;
	hasByteGrayscale = false;
	hasWordGrayscale = false;

	types.clear();
}

ParserLoaderTXT::~ParserLoaderTXT()
{
	inputFile.close();
}

void ParserLoaderTXT::addField(FieldType type)
{
	if ((type == floatX) || (type == floatY) || (type == floatZ)) hasFloatXYZ = true;
	else if ((type == byteRed) || (type == byteGreen) || (type == byteBlue)) hasByteRGB = true;
	else if (type == byteAlpha) hasByteAlpha = true;
	else if ((type == floatRed) || (type == floatGreen) || (type == floatBlue)) hasFloatRGB = true;
	else if (type == floatAlpha) hasFloatAlpha = true;
	else if (type == byteGrayscale) hasByteGrayscale = true;
	else if (type == wordGrayscale) hasWordGrayscale = true;

	else if (type >= byteReflection)
	{
		itIsOrderedCloud = true;

		if (type == byteReflection) hasReflections = true;
		else if ((type == floatHorizontalAngle) || (type == floatVerticalAngle)) hasAngles = true;
		else if (type == floatDistance) hasDistances = true;

	}

	hasColor = (hasByteRGB || hasByteAlpha || hasFloatRGB || hasFloatAlpha || hasByteGrayscale || hasWordGrayscale);

	types.push_back( type );
}

#include "PointCloud.h"
#include "OrderedPointCloud.h"

void ParserLoaderTXT::parseLine(QString myline)
{
	auto line = myline.splitRef(separator);

	CVertex vertex;
	CRGBA rgba((BYTE)255, 255, 255, 255);

	float distance;
	float angleH, angleV;
	BYTE reflection;

	for (int i = 0; i < line.size(); i++)
	{
		switch (types[i])
		{
		case floatX:
			vertex.x = line[i].toFloat();
			break;
		case floatY:
			vertex.y = line[i].toFloat();
			break;
		case floatZ:
			vertex.z = line[i].toFloat();
			break;
		case byteRed:
			rgba.setRed(line[i].toUShort());
			break;
		case byteGreen:
			rgba.setGreen(line[i].toUShort());
			break;
		case byteBlue:
			rgba.setBlue(line[i].toUShort());
			break;
		case byteAlpha:
			rgba.setAlpha(line[i].toUShort());
			break;
		case floatRed:
			rgba.fR(line[i].toFloat());
			break;
		case floatGreen:
			rgba.fG(line[i].toFloat());
			break;
		case floatBlue:
			rgba.fB(line[i].toFloat());
			break;
		case floatAlpha:
			rgba.fA(line[i].toFloat());
			break;
		case byteGrayscale:
			rgba.setGray8(line[i].toUShort());
			break;
		case wordGrayscale:
			rgba.setGray16(line[i].toUShort());
			break;
		case byteReflection:
			reflection = line[i].toUShort();
			break;
		case floatHorizontalAngle:
			angleH = line[i].toFloat();
			break;
		case floatVerticalAngle:
			angleV = line[i].toFloat();
			break;
		case floatDistance:
			distance = line[i].toFloat();
			break;

		case Ignore:
		default:
			// IGNORE VALUE line[i]
			break;

		}

	}

	if (hasColor)
		((CPointCloud*)m_child)->addVertex(vertex, rgba);
	else
		((CPointCloud*)m_child)->addVertex(vertex);

	if (hasAngles)
	{
		((COrderedPointCloud*)m_child)->m_angles.push_back(std::pair<float, float>(angleH, angleV));
	}

	if (hasDistances)
	{
		((COrderedPointCloud*)m_child)->m_dist.push_back(distance);
	}

	if (hasReflections)
	{
		((COrderedPointCloud*)m_child)->m_refl.push_back(reflection);
	}
}

void ParserLoaderTXT::setSeparator(QChar sep)
{
	separator = sep;
}

void ParserLoaderTXT::skipLines(int skipL)
{
	if (skipL > 0)
	{
		int i = 0;
		while (!in.atEnd() && (i < skipL))
		{
			in.readLine();
			i++;
		}
	}
}

void ParserLoaderTXT::createChild()
{
	if (itIsOrderedCloud)
	{
		m_child = new COrderedPointCloud();
	}
	else
	{
		m_child = new CPointCloud();
	}

	if (m_child == NULL)
	{
		cancelled = true;
		emit(workCancelled());
	}
}

#include <QElapsedTimer>

void ParserLoaderTXT::doWork()
{
	if (cancelled)
	{
		if (itIsOrderedCloud)
		{
			delete (COrderedPointCloud*)m_child;
		}
		else
		{
			delete (CPointCloud*)m_child;
		}

		emit(workCancelled());
	}
	else
	{
		DWORD t1 = GetTickCount() + 1000;
		//(dp) see also: timeGeetTime() with timeBeginPeriod(1) => 1ms resolution but slower

		//QElapsedTimer tm;
		//tm.start();
		//while ((tm.elapsed() < 1000) && !in.atEnd())
		//IT IS SLOWER !!!

		while ((t1 > GetTickCount()) && !in.atEnd())
		{
			parseLine(in.readLine());
		}

		if (in.atEnd())
		{
			if (itIsOrderedCloud)
			{
				((COrderedPointCloud*)m_child)->m_cols = ((COrderedPointCloud*)m_child)->vertices().size();
				((COrderedPointCloud*)m_child)->m_rows = 1;
			}

			emit(workDone());
		}
		else
		{
			int progress = round((100.0f * (inputFile.size() - inputFile.bytesAvailable())) / inputFile.size());
			emit(sendProgress(progress));
		}
	}
}

