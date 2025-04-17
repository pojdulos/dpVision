#pragma once
#include "Parser.h"
#include "Movement.h"

class CParserATMDL :public CParser
{
	static QMap<QString,QString> defs;
	QMap<QString, QString> back_defs;

	QFile atmdlFile;

	CObject* clean_model(CModel3D* obj);
	CObject* loadShellFile(const QString path, QFile& mainFile, bool cleanIt = true);

	void add_kid(CObject* obj, CBaseObject* kid);

	void replaceKeysWithValues(QString& txt);

	QString skip_comments(QTextStream& in);

	QString parseType_string(QTextStream& in);
	QString parseType_matrix(QTextStream& in);

	QList<double> parseType_MatrixOfDouble(QTextStream& in, int size);

	CTransform parseProperty_rotation(QTextStream& in);
	CTransform parseProperty_translation(QTextStream& in);

	CMovement::FrameVal parseProperty_frame(QTextStream& in);
	CMovement::SeqList parseProperty_frameset(QTextStream& in);
	CMovement::SeqList parseProperty_sequence(QTextStream& in);

	QSet<QString> parseProperty_keywords_v1(QString keywords);


	CBaseObject* parseObject_shell(QTextStream& in);
	CBaseObject* parseObject_transformation(QTextStream& in);
	CBaseObject* parseObject_point(QTextStream& in);
	CBaseObject* parseObject_sphere(QTextStream& in);
	CBaseObject* parseObject_plane(QTextStream& in);
	CBaseObject* parseObject_triangle(QTextStream& in);
	CBaseObject* parseObject_animation(QTextStream& in);
	CBaseObject* parseObject(QTextStream& in, QString slowo);

public:
	CParserATMDL(void);
	~CParserATMDL(void) {}

	virtual CModel3D* load(const QString path, bool wait) override;
	QString obj2atmdl(CBaseObject* obj, const QString &wciecie="");
	virtual bool save(QVector<CBaseObject*> objects, const QString path) override;
	virtual bool save(CModel3D* obj, const QString path) override;
};
