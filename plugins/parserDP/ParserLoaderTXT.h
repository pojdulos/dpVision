#pragma once
#include "dll_global.h"

#include <ThreadParserLoader.h>

#include "TextFileDialogFieldType.h"

#include "QtCore/QFile"
#include "QtCore/QTextStream"

class DPVISION_DLL_API ParserLoaderTXT : public ThreadParserLoader
{
	Q_OBJECT

public:
	ParserLoaderTXT(QString filename, QObject* parent = 0);
	~ParserLoaderTXT();

	void addField(FieldType type);
	void setSeparator(QChar sep);
	void skipLines(int skip);

private:
	QVector<FieldType> types;
	QFile inputFile;
	QTextStream in;
	QChar separator;


	bool itIsOrderedCloud;
	bool hasReflections;
	bool hasAngles;
	bool hasDistances;

	bool hasFloatXYZ;

	bool hasColor;

	bool hasFloatRGB;
	bool hasFloatAlpha;
	bool hasByteRGB;
	bool hasByteAlpha;
	bool hasByteGrayscale;
	bool hasWordGrayscale;

	void parseLine(QString myline);


public slots:
	virtual void createChild() override;
	virtual void doWork() override;
//	virtual void cancelWork() override;
};

