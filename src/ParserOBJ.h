#pragma once
#include "Parser.h"
#include <QtCore>


struct _predata {
	int f;
	int fn;
	int ft;
	int v;
	int vn;
	int vt;
	int vc;

	_predata(void) { f = fn = ft = v = vn = vt = vc = 0; };
};

struct Meshinfo {
	bool hasVC;
	bool hasVN;
	bool hasVT;
};


class MYDECL CParserOBJ :public QObject, public CParser
{
	Q_OBJECT

	bool cancelled;
	bool m_firstFace;
	bool m_bN, m_bT;

	void ParseObjMTLFile( const wchar_t *npl );
	inline void parseF(std::string& bufor, size_t& lbf, size_t& lbti);

	std::map<QString, CMaterial*> mats;
	QString currentName;
	CMaterial* currentMaterial;

	std::map<int, Meshinfo> meshinfo;

public:
	CParserOBJ(void);
	~CParserOBJ(void);

	virtual bool inPlugin() override { return false; };

	virtual size_t Run() override;

	void setChildrenVertices(CObject* obj, CPointCloud::Vertices& tmpV, CPointCloud::Colors& tmpC, CPointCloud::Normals& tmpN, CMaterial::TextureCoordinates& tmpTC);

	//virtual CModel3D* load(std::wstring path) override;

	void saveMTL(QTextStream& mtlStream, CMaterial& material, QString matName);
	void saveMesh(QTextStream& objFile, CMesh* src, size_t& vStart);
	void saveCloud(QTextStream& objFile, CPointCloud* src, size_t& vStart);

	void saveChildren(QTextStream& objStream, QTextStream& mtlStream, CObject* obj, QString prefix, size_t& vStart);

	virtual bool save() override;
signals:
	void sendProgress(int);

public slots:
	void onLoadCancelled();
};
