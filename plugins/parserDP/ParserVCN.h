#pragma once
#include "Parser.h"

class CPointCloud;

class CParserVCN :public CParser
{
	CPointCloud* loadVCN(const QString fname);
	void saveVCN(CPointCloud* mesh, const QString fname);

public:
	CParserVCN(void);
	~CParserVCN(void);

	virtual CModel3D* load(const QString path, bool wait) override;
	virtual bool save(CModel3D* obj, const QString path) override;

};
