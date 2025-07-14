#pragma once
#include "Parser.h"
#include "Movement.h"

class CParserATMDL :public CParser
{
	static QMap<QString,QString> defs;
	QMap<QString, QString> back_defs;

	QFile atmdlFile;

	std::shared_ptr<CObject> clean_model(std::shared_ptr<CModel3D> obj);
	std::shared_ptr<CObject> loadShellFile(const QString path, QFile& mainFile, bool cleanIt = true);

	void add_kid(std::shared_ptr<CObject> obj, std::shared_ptr<CBaseObject> kid);

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


	std::shared_ptr<CBaseObject> parseObject_shell(QTextStream& in);
	std::shared_ptr<CBaseObject> parseObject_transformation(QTextStream& in);
	std::shared_ptr<CBaseObject> parseObject_point(QTextStream& in);
	std::shared_ptr<CBaseObject> parseObject_sphere(QTextStream& in);
	std::shared_ptr<CBaseObject> parseObject_plane(QTextStream& in);
	std::shared_ptr<CBaseObject> parseObject_triangle(QTextStream& in);
	std::shared_ptr<CBaseObject> parseObject_animation(QTextStream& in);
	std::shared_ptr<CBaseObject> parseObject(QTextStream& in, QString slowo);

public:
	CParserATMDL(void);
	~CParserATMDL(void) {}

	virtual std::shared_ptr<CModel3D> load(const QString path, bool wait, std::shared_ptr<IProgressListener> prg = nullptr) override;
	QString obj2atmdl(std::shared_ptr<CBaseObject> obj, const QString &wciecie="");
	virtual bool save(QVector<std::shared_ptr<CBaseObject>> objects, const QString path) override;
	virtual bool save(std::shared_ptr<CModel3D> obj, const QString path) override;
};
