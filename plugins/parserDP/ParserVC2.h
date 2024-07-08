#pragma once
#include "Parser.h"

class COrderedPointCloud;

class CParserVC2 :public CParser
{
	COrderedPointCloud* loadVC2(const QString fname);
	void saveVC2(COrderedPointCloud* mesh, const QString fname);

public:
	CParserVC2(void);
	~CParserVC2(void);

	virtual CModel3D* load(const QString path, bool wait) override;
	virtual bool save(CModel3D* obj, const QString path) override;
};
