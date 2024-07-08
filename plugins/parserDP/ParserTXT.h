#ifndef NEWTXTPARSERWORKER_H
#define NEWTXTPARSERWORKER_H
#include "dll_global.h"

#include <ThreadParser.h>

#include "TextFileDialog.h"

class DPVISION_DLL_API CParserTXT : public ThreadParser
{
	Q_OBJECT

public:
	CParserTXT(void);
	~CParserTXT(void);

	virtual CModel3D* load(const QString path, bool wait = false) override;

private:
	TextFileDialog dialog;

public slots:
	virtual void onLoadDone() override;
	//virtual void onLoadCancelled() override;
};

#endif