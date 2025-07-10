#pragma once
#include "Parser.h"
#include "quazip.h"

class CPointCloud;
//class QuaZip;
class CWorkspace;

#include <QtXml>

#define IdToString(id) QString::number((id),16)

#include "dll_global.h"

class DPVISION_EXPORT CParserDPVISION :public QObject, public CParser
{
	Q_OBJECT

	void copyFileToZip(QuaZip& zip, QString pathInZip, QString inFileName);
	void writeByteArrayToZip(QuaZip& zip, QString pathInZip, const QByteArray& ba);
	void saveMTL(QuaZip& zip, CMaterial& material, QString pathInZip, QString matName);
	void saveMesh(QTextStream& objFile, std::shared_ptr<CObject> src, size_t& vStart);

	void saveObject(QuaZip& zip, std::shared_ptr<CBaseObject> obj, QString path);

	void createXmlNodeObject(std::shared_ptr<CObject> obj, QDomElement& root, QDomDocument& docu, QString path);

	void createXmlNodeAnnotation(std::shared_ptr<CAnnotation> obj, QDomElement& root, QDomDocument& docu);
	void createAnnotationsXml(std::shared_ptr<CObject> obj, QDomElement& root, QDomDocument& docu);

	void createStructureXml(QByteArray& ba, QVector<std::shared_ptr<CBaseObject>> vobj);



	void parseF(QString& line, CMesh::Faces& faces, CMaterial& material, size_t& lbf, size_t& lbti);
	void ParseObjMTLFile(QTextStream& in, std::map<QString, CMaterial*>& mats, QuaZip& zip, QString pathInZip);
	std::shared_ptr<CObject> parseOBJ(QTextStream& in, QuaZip& zip, QString pathInZip);
	void setChildrenVertices(QVector<std::shared_ptr<CObject>> vec, CPointCloud::Vertices& tmpV, CPointCloud::Colors& tmpC, CPointCloud::Normals& tmpN, CMaterial::TextureCoordinates& tmpTC);

	std::shared_ptr<CObject> readZippedFileObj(QuaZip& zip, QString pathInZip);
	std::shared_ptr<CBaseObject> parseObject(const QDomElement& currentElement, QuaZip& zip);


	typedef struct {
		std::shared_ptr<CBaseObject> ptr;
		QString parentID;
		QString path;
	} Address;
	typedef QMap<QString, Address> AddressMap;
	AddressMap mapaObiektow;

public:
	CParserDPVISION(void);
	~CParserDPVISION(void);

	virtual std::shared_ptr<CModel3D> load(const QString path, bool wait) override;
	virtual bool save(std::shared_ptr<CModel3D> obj, const QString path) override;
	virtual bool save(QVector<std::shared_ptr<CBaseObject>> objects, const QString path) override;
};
