#include "ParserATMDL.h"

#include "AnnotationPlane.h"
#include "AnnotationPoint.h"
#include "AnnotationSphere.h"
#include "AnnotationTriangle.h"

#include "FileConnector.h"

QMap<QString, QString> CParserATMDL::defs;

CParserATMDL::CParserATMDL()
{
	setDescr("Digital patient workspace");
	m_exts.insert("dpw");
	m_exts.insert("atmdl");
	m_save_exts.insert("dpw");
	m_save_exts.insert("atmdl");
}

QString CParserATMDL::skip_comments(QTextStream& in)
{
	QString slowo;

	in >> slowo;
	while (slowo.trimmed().startsWith('#'))
	{
		qInfo() << "COMMENT: " + slowo + in.readLine() << Qt::endl;
		in >> slowo;
	}

	return slowo;
}

CObject* CParserATMDL::clean_model(CModel3D *obj)
{
	CTransform& t = obj->transform();

	if (t.toQMatrix4x4().isIdentity() && (obj->children().size() == 1) )
	{
		CObject* c = (CObject*)obj->getChild();

		obj->removeChild(c->id());

		c->setLabel(obj->getLabel());
		c->setDescr(obj->getDescr());

		if (!obj->annotations().empty())
		{
			CObject::Annotations::iterator it = obj->annotations().begin();

			while (it != obj->annotations().end())
			{
				std::shared_ptr<CAnnotation> an = (*it).second;
				it = obj->annotations().erase(it);

				c->addAnnotation(an.get());
			}
		}

		delete obj;

		return c;
	}

	return obj;
}

CObject* CParserATMDL::loadShellFile(const QString filepath, QFile &mainFile, bool cleanIt)
{
	QString myPath(filepath);

	if (!QFileInfo(myPath).exists())
	{
		QFileInfo mainfi(mainFile);

		QFileInfo fi(mainfi.absolutePath(), myPath);

		if (!fi.exists())
		{
			qWarning() << "\033[31m --- File not exists: " << myPath << " --- \033[0m" << Qt::endl;
			return nullptr;
		}

		myPath = fi.absoluteFilePath();
	}

	CParser* parser = CFileConnector::getLoadParser(myPath);
	CModel3D * result = parser->load(myPath);

	if (!parser->inPlugin()) delete parser;
	return cleanIt?clean_model(result):result;
}

void CParserATMDL::add_kid(CObject* obj, CBaseObject* kid)
{
	if (kid->hasCategory(CBaseObject::OBJECT))
		obj->addChild(kid);
	else if (kid->hasCategory(CBaseObject::ANNOTATION))
		obj->addAnnotation((CAnnotation*)kid);
}

void CParserATMDL::replaceKeysWithValues(QString& txt) {
	//QMapIterator<QString, QString> iterator(defs);
	//while (iterator.hasNext()) {
	//	iterator.next();
	//	txt.replace(iterator.key(), iterator.value());
	//}

	QString separator = "$#%#$"; // Unikalny separator
	int counter = 0;

	QMap<QString, QString> tempMap;
	for (const QString & key : defs.keys()) {
		QString tempKey = separator + QString::number(counter++) + separator;
		txt.replace(key, tempKey);
		tempMap[tempKey] = defs[key];
	}

	for (const QString & tempKey : tempMap.keys()) {
		txt.replace(tempKey, tempMap[tempKey]);
	}
}

QString CParserATMDL::parseType_string(QTextStream& in)
{
	QString slowo;

	in >> slowo;
	if (slowo.front() == "\"")
	{
		QChar znak;
		if (slowo.length() == 1)
		{
			in >> znak;
			slowo += znak;
		}
		
		while (slowo.back() != "\"")
		{
			in >> znak;
			if ((znak == '\n') || (znak == '\r'))
			{
				qInfo() << QString::fromUtf8("UWAGA: Napotkano koniec linii bez zamknięcia cudzysłowu") << Qt::endl;
				break;
			}
			slowo += znak;
		}

		slowo.remove(0, 1);
		slowo.chop(1);
	}
	else if (slowo.front() == "{")
	{
		QChar znak;
		while (slowo.back() != "}")
		{
			in >> znak;
			slowo += znak;
		}

		slowo.remove(0, 1);
		slowo.chop(1);
		slowo = slowo.trimmed();
	}

	//qInfo() << "RAW STRING: " << slowo << Qt::endl;

	replaceKeysWithValues(slowo);

	qInfo() << "STRING: " << slowo << Qt::endl;

	return slowo;
}

QString CParserATMDL::parseType_matrix(QTextStream& in)
{
	QString tekst;

	in >> tekst;
	
	if (tekst.front() == '[')
	{
		QChar znak;
		while (tekst.back() != ']')
		{
			in >> znak;
			tekst += znak;
		}

		tekst.remove(0, 1); // usuwa "["
		tekst.chop(1); // usuwa "]"
	}

	QRegExp regex("[\\s,;]+");
	tekst.replace(regex, ",");

	if (tekst.startsWith(','))
	{
		tekst.remove(0, 1);
	}

	if (tekst.endsWith(','))
	{
		tekst.chop(1);
	}

	qInfo() << "MATRIX: " << tekst << Qt::endl;
	return tekst;
}

QList<double> CParserATMDL::parseType_MatrixOfDouble(QTextStream& in, int size)
{
	QString tekst = parseType_matrix(in);

	QStringList qCoords = tekst.split(",", QString::SkipEmptyParts);

	if (qCoords.size() != size)
	{ 	//qInfo() << "\033[31m" << "To jest komunikat b��du" << "\033[0m" << Qt::endl;

		qWarning() << "\033[33mOczekiwano macierzy o rozmiarze " + QString::number(size) + " elementów. Odczytano " + QString::number(qCoords.size()) + " elementów.\033[0m" << Qt::endl;
	}

	while (qCoords.size() > size)
	{
		qCoords.erase(qCoords.end());
	}

	QList<double> qlist;
		
	for (QString& s : qCoords)
	{
		qlist.append(s.toDouble());
	}

	while (qlist.size() < size)
	{
		qlist.append(0.0);
	}

	qInfo() << "DOUBLE MATRIX: " << qlist << Qt::endl;
	return qlist;
}

CTransform CParserATMDL::parseProperty_rotation(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("B��D. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return CTransform();
	}

	QMap<QString, QString> opis;
	QList<CBaseObject*> kids;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "axis")
		{
			opis["axis"] = parseType_matrix(in);
		}
		else if (slowo == "origin")
		{
			opis["origin"] = parseType_matrix(in);
		}
		else if (slowo == "angle")
		{
			in >> slowo;
			opis["angle"] = slowo;
		}
		else if (slowo == "}")
		{
			qInfo() << "Poprawnie odczytano punkt" << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return CTransform();
		}

	}

	if (opis.contains("axis"))
	{
		QStringList axisList = opis["axis"].split(",", QString::SkipEmptyParts);

		CVector3d axis(axisList[0].toDouble(), axisList[1].toDouble(), axisList[2].toDouble());

		CPoint3d origin(0.0, 0.0, 0.0);

		if (opis.contains("origin"))
		{
			QStringList originList = opis["origin"].split(",", QString::SkipEmptyParts);
			origin.Set(originList[0].toDouble(), originList[1].toDouble(), originList[2].toDouble());
		}

		double angle = 0.0;

		if (opis.contains("angle"))
		{
			angle = deg2rad(opis["angle"].toDouble());
		}


		CTransform t;
		
		t.rotate(origin, { { axis, angle } });

		return t;
	}

	return CTransform();
}

CTransform CParserATMDL::parseProperty_translation(QTextStream& in)
{
	QString slowo =	parseType_matrix(in);
	
	QStringList axisList = slowo.split(",", QString::SkipEmptyParts);

	CVector3d axis(axisList[0].toDouble(), axisList[1].toDouble(), axisList[2].toDouble());

	CTransform t;

	t.translate(axis);

	return t;
}

CMovement::SeqList CParserATMDL::parseProperty_frameset(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return CMovement::SeqList();
	}

	QMap<QString, QString> opis;

	CTransform frameTransformation;
	bool isTransformDefined = false;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if (slowo == "matrix")
		{
			opis["matrix"] = parseType_matrix(in);
		}
		else if (slowo == "rotation")
		{
			CTransform tR = parseProperty_rotation(in);
			frameTransformation.fromEigenMatrix4d(tR.toEigenMatrix4d() * frameTransformation.toEigenMatrix4d());
			isTransformDefined = true;
		}
		else if (slowo == "translation")
		{
			CTransform tT = parseProperty_translation(in);
			frameTransformation.fromEigenMatrix4d(tT.toEigenMatrix4d() * frameTransformation.toEigenMatrix4d());
			isTransformDefined = true;
		}
		else if (slowo == "count")
		{
			in >> slowo;
			opis["count"] = slowo;
		}
		else if ((slowo == "delay") || (slowo == "time"))
		{
			in >> slowo;
			opis["time"] = slowo;
		}
		else if (slowo == "}")
		{
			qInfo() << QString::fromUtf8("Poprawnie odczytano transformację") << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return CMovement::SeqList();
		}

	}

	if (opis.contains("matrix"))
	{
		frameTransformation.fromRowMatrix(opis["matrix"], ",");
		isTransformDefined = true;
	}

	if (isTransformDefined)
	{
		int count = opis.contains("count") ? opis["count"].toInt() : 1;

		double timeS = opis.contains("time") ? opis["time"].toDouble() : 1.0;
		int msec = int(timeS * 1000.0);

		CTransform globalTransformation;

		//if (opis.contains("label")) obj->setLabel("F("+opis["time"] +"): " + opis["label"]);
		//if (opis.contains("descr")) obj->setDescr(opis["descr"]);

		CMovement::SeqList seq;

		for (int i = 0; i < count; i++)
		{
			seq.append(CMovement::FrameVal(msec, globalTransformation));

			globalTransformation.fromEigenMatrix4d(frameTransformation.toEigenMatrix4d() * globalTransformation.toEigenMatrix4d());
		}

		return seq;
	}
	return CMovement::SeqList();
}

CMovement::FrameVal CParserATMDL::parseProperty_frame(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return CMovement::FrameVal();
	}

	QMap<QString, QString> opis;

	CTransform frameTransformation;
	bool isTransformDefined = false;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if (slowo == "matrix")
		{
			opis["matrix"] = parseType_matrix(in);
		}
		else if (slowo == "rotation")
		{
			CTransform tR = parseProperty_rotation(in);
			frameTransformation.fromEigenMatrix4d(tR.toEigenMatrix4d() * frameTransformation.toEigenMatrix4d());
			isTransformDefined = true;
		}
		else if (slowo == "translation")
		{
			CTransform tT = parseProperty_translation(in);
			frameTransformation.fromEigenMatrix4d(tT.toEigenMatrix4d() * frameTransformation.toEigenMatrix4d());
			isTransformDefined = true;
		}
		else if (slowo == "order")
		{
			in >> slowo;
			opis["order"] = slowo;
		}
		else if ((slowo == "delay") || (slowo == "time"))
		{
			in >> slowo;
			opis["time"] = slowo;
		}
		else if (slowo == "}")
		{
			qInfo() << QString::fromUtf8("Poprawnie odczytano klatkę animacji") << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return CMovement::FrameVal();
		}

	}

	if (opis.contains("matrix"))
	{
		frameTransformation.fromRowMatrix(opis["matrix"], ",");
		isTransformDefined = true;
	}

	if (isTransformDefined)
	{
		double timeS = opis.contains("time") ? opis["time"].toDouble() : 1.0;
		int msec = int(timeS * 1000.0);

		QString label = (opis.contains("label")) ? opis["label"] : "frame";
		//if (opis.contains("descr")) obj->setDescr(opis["descr"]);

		return CMovement::FrameVal(msec, label, frameTransformation);
	}

	return CMovement::FrameVal();
}

CMovement::SeqList CParserATMDL::parseProperty_sequence(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return CMovement::SeqList();
	}

	CMovement::SeqList seq;

	int count = 0;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "frame")
		{
			CMovement::FrameVal frame = parseProperty_frame(in);

			seq.append( frame );
		}
		else if (slowo == "frameset")
		{
			CMovement::SeqList frameset = parseProperty_frameset(in);

			seq += frameset;
		}
		else if (slowo == "}")
		{
			qInfo() << "Poprawnie odczytano sekwencje, rozmiar: " << seq.size() << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return CMovement::SeqList();
		}

	}

	return seq;
}

QSet<QString> CParserATMDL::parseProperty_keywords_v1(QString keywords)
{
	QSet<QString> out = keywords.split(";", QString::SkipEmptyParts).toSet();
	return out;
}

CBaseObject* CParserATMDL::parseObject_shell(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	qInfo() << "Rozpoczeto interpretacje obiektu shell" << Qt::endl;

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return nullptr;
	}

	QMap<QString, QString> opis;
	QList<CBaseObject*> kids;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if (slowo == "keywords")
		{
			opis["keywords"] = parseType_string(in);
		}
		else if (slowo == "file")
		{
			opis["file"] = parseType_string(in);
		}
		else if (CBaseObject* tmp = parseObject(in, slowo))
		{
			kids.append(tmp);
		}
		else if (slowo == "}")
		{
			qInfo() << "znaleziono klamre zamykajaca" << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BLAD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return nullptr;
		}

	}

	CObject* obj = nullptr;

	if (opis.contains("file"))
	{
		obj = loadShellFile(opis["file"], atmdlFile);
		if (obj==nullptr)
		{
			obj = new CObject();
		}

		if (opis.contains("label")) obj->setLabel(opis["label"]);
		if (opis.contains("descr")) obj->setDescr(opis["descr"]);

		if (opis.contains("keywords")) obj->keywords() = parseProperty_keywords_v1(opis["keywords"]);

		for (CBaseObject* kid : kids) add_kid(obj, kid);
	}

	qInfo() << "Zakonczono interpretacje obiektu shell" << Qt::endl;

	return obj;
}

CBaseObject* CParserATMDL::parseObject_transformation(QTextStream& in)
{
	QString slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return nullptr;
	}

	QMap<QString, QString> opis;
	QList<CBaseObject*> kids;
	//QStringList matrix;

	CTransform frameTransformation;
	bool isTransformDefined = false;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if (slowo == "keywords")
		{
			opis["keywords"] = parseType_string(in);
		}
		else if (slowo == "matrix")
		{
			opis["matrix"] = parseType_matrix(in);
		}
		else if (slowo == "rotation")
		{
			CTransform tR = parseProperty_rotation(in);
			frameTransformation.fromEigenMatrix4d(tR.toEigenMatrix4d() * frameTransformation.toEigenMatrix4d());
			isTransformDefined = true;
		}
		else if (slowo == "translation")
		{
			CTransform tT = parseProperty_translation(in);
			frameTransformation.fromEigenMatrix4d(tT.toEigenMatrix4d() * frameTransformation.toEigenMatrix4d());
			isTransformDefined = true;
		}
		else if (CBaseObject* tmp = parseObject(in, slowo))
		{
			kids.append(tmp);
		}
		else if (slowo == "}")
		{
			qInfo() << QString::fromUtf8("Poprawnie odczytano transformację").toStdU16String() << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return nullptr;
		}

	}

	CModel3D* obj = new CModel3D;

	if (obj)
	{
		if (opis.contains("label")) obj->setLabel(opis["label"]);
		if (opis.contains("descr")) obj->setDescr(opis["descr"]);

		if (opis.contains("keywords")) obj->keywords() = parseProperty_keywords_v1(opis["keywords"]);

		if (opis.contains("matrix"))
		{
			frameTransformation.fromRowMatrix(opis["matrix"], ",");
			isTransformDefined = true;
		}

		if (isTransformDefined)
		{
			obj->setTransform(frameTransformation);
		}
		else
		{
			obj->setTransform(CTransform());
		}

		for (CBaseObject* kid : kids) add_kid(obj, kid);
	}

	obj->importChildrenGeometry();

	return obj;
}

CBaseObject* CParserATMDL::parseObject_point(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return nullptr;
	}

	QMap<QString, QString> opis;
	QList<CBaseObject*> kids;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if (slowo == "coords")
		{
			opis["coords"] = parseType_matrix(in);
		}
		else if (slowo == "normal")
		{
			opis["normal"] = parseType_matrix(in);
		}
		else if (slowo == "color")
		{
			in >> slowo;
			opis["color"] = slowo;
		}
		else if (slowo == "}")
		{
			qInfo() << "Poprawnie odczytano punkt" << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return nullptr;
		}

	}

	if (opis.contains("coords"))
	{
		QStringList qCoords = opis["coords"].split(",", QString::SkipEmptyParts);

		CAnnotationPoint* obj = new CAnnotationPoint();

		if (obj)
		{
			obj->Set(qCoords[0].toDouble(), qCoords[1].toDouble(), qCoords[2].toDouble());

			if (opis.contains("normal"))
			{
				QStringList qNormal = opis["normal"].split(",", QString::SkipEmptyParts);
				qInfo() << qNormal;
				obj->setDirection(qNormal[0].toDouble(), qNormal[1].toDouble(), qNormal[2].toDouble());
			}

			if (opis.contains("color")) obj->setColor(QColor(opis["color"]));
			if (opis.contains("label")) obj->setLabel(opis["label"]);
			if (opis.contains("descr")) obj->setDescr(opis["descr"]);

			return obj;
		}
	}

	return nullptr;
}


CBaseObject* CParserATMDL::parseObject_plane(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return nullptr;
	}

	QMap<QString, QString> opis;
	QList<CBaseObject*> kids;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if (slowo == "keywords")
		{
			opis["keywords"] = parseType_string(in);
		}
		else if (slowo == "center")
		{
			opis["center"] = parseType_matrix(in);
		}
		else if (slowo == "normal")
		{
			opis["normal"] = parseType_matrix(in);
		}
		else if (slowo == "size")
		{
			opis["size"] = parseType_string(in);
		}
		else if (slowo == "color")
		{
			in >> slowo;
			opis["color"] = slowo;
		}
		else if (slowo == "}")
		{
			qInfo() << "Poprawnie odczytano plaszczyzne" << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return nullptr;
		}

	}

	if (opis.contains("center"))
	{
		QStringList qCoords = opis["center"].split(",", QString::SkipEmptyParts);

		CAnnotationPlane* obj = new CAnnotationPlane();

		if (obj)
		{
			obj->setCenter(CPoint3d(qCoords[0].toDouble(), qCoords[1].toDouble(), qCoords[2].toDouble()));

			if (opis.contains("normal"))
			{
				QStringList qNormal = opis["normal"].split(",", QString::SkipEmptyParts);
				qInfo() << qNormal;
				obj->setNormal(CVector3d(qNormal[0].toDouble(), qNormal[1].toDouble(), qNormal[2].toDouble()));
			}
			else {
				obj->setNormal(CVector3d(0, 1, 0));
			}

			if (opis.contains("size"))
			{
				double size = opis["size"].toDouble();
				qInfo() << "size = " << size;
				obj->setSize(size);
			}

			if (opis.contains("color")) obj->setColor(QColor(opis["color"]));
			if (opis.contains("label")) obj->setLabel(opis["label"]);
			if (opis.contains("descr")) obj->setDescr(opis["descr"]);
			if (opis.contains("keywords")) obj->keywords() = parseProperty_keywords_v1(opis["keywords"]);

			return obj;
		}
	}

	return nullptr;
}


CBaseObject* CParserATMDL::parseObject_sphere(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return nullptr;
	}

	QMap<QString, QString> opis;
	QList<CBaseObject*> kids;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if (slowo == "coords")
		{
			opis["coords"] = parseType_matrix(in);
		}
		else if (slowo == "radius")
		{
			in >> slowo;
			opis["radius"] = slowo;
		}
		else if (slowo == "color")
		{
			in >> slowo;
			opis["color"] = slowo;
		}
		else if (slowo == "}")
		{
			qInfo() << "Poprawnie odczytano punkt" << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return nullptr;
		}

	}

	CPoint3d coords(0.0, 0.0, 0.0);

	if (opis.contains("coords"))
	{
		QStringList qCoords = opis["coords"].split(",", QString::SkipEmptyParts);
		coords.Set(qCoords[0].toDouble(), qCoords[1].toDouble(), qCoords[2].toDouble());
	}

	double radius = 1.0;

	if (opis.contains("radius"))
	{
		radius = opis["radius"].toDouble();
	}

	Kula k(coords, radius);

	CAnnotationSphere* obj = new CAnnotationSphere(k);

	if (obj)
	{
		obj->m_lats = 32;
		obj->m_longs = 32;

		
		if (opis.contains("color")) obj->setColor(QColor(opis["color"]));
		if (opis.contains("label")) obj->setLabel(opis["label"]);
		if (opis.contains("descr")) obj->setDescr(opis["descr"]);

		return obj;
	}

	return nullptr;
}

CBaseObject* CParserATMDL::parseObject_triangle(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return nullptr;
	}

	QMap<QString, QString> opis;
	QList<CBaseObject*> kids;

	QList<double> cA, cB, cC;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if (slowo == "coordsA")
		{
			opis["coordsA"] = parseType_matrix(in);
			//cA = parseType_MatrixOfDouble(in, 3);
		}
		else if (slowo == "coordsB")
		{
			opis["coordsB"] = parseType_matrix(in);
		}
		else if (slowo == "coordsC")
		{
			opis["coordsC"] = parseType_matrix(in);
		}
		else if (slowo == "color")
		{
			in >> slowo;
			opis["color"] = slowo;
		}
		else if (slowo == "}")
		{
			qInfo() << "Poprawnie odczytano trojkat" << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return nullptr;
		}

	}

	if (opis.contains("coordsA")&& opis.contains("coordsB")&& opis.contains("coordsC"))
//	if (opis.contains("coordsB") && opis.contains("coordsC"))
	{
		QStringList qCoordsA = opis["coordsA"].split(",", QString::SkipEmptyParts);
		QStringList qCoordsB = opis["coordsB"].split(",", QString::SkipEmptyParts);
		QStringList qCoordsC = opis["coordsC"].split(",", QString::SkipEmptyParts);

		//CPoint3d vA(cA[0], cA[1], cA[2]);
		CPoint3d vA(qCoordsA[0].toDouble(), qCoordsA[1].toDouble(), qCoordsA[2].toDouble());
		CPoint3d vB(qCoordsB[0].toDouble(), qCoordsB[1].toDouble(), qCoordsB[2].toDouble());
		CPoint3d vC(qCoordsC[0].toDouble(), qCoordsC[1].toDouble(), qCoordsC[2].toDouble());

		CAnnotationTriangle* obj = new CAnnotationTriangle(vA,vB,vC);

		if (obj)
		{
			if (opis.contains("color")) obj->setColor(QColor(opis["color"]));
			if (opis.contains("label")) obj->setLabel(opis["label"]);
			if (opis.contains("descr")) obj->setDescr(opis["descr"]);

			return obj;
		}
	}

	return nullptr;
}

CBaseObject* CParserATMDL::parseObject_animation(QTextStream& in)
{
	QString slowo;

	slowo = skip_comments(in);

	if (slowo != "{")
	{
		qInfo() << QString::fromUtf8("BŁĄD. Oczekiwano znaku { odczytano: ") << slowo << Qt::endl;

		return nullptr;
	}

	QMap<QString, QString> opis;
	QList<CBaseObject*> kids;
	//QStringList matrix;
	CMovement::SeqList seq;

	while (slowo != "}")
	{
		slowo = skip_comments(in);

		if (slowo == "label")
		{
			opis["label"] = parseType_string(in);
		}
		else if (slowo == "descr")
		{
			opis["descr"] = parseType_string(in);
		}
		else if ((slowo == "sequence") || (slowo == "sequention"))
		{
			seq = parseProperty_sequence(in);
		}
		else if (CBaseObject* tmp = parseObject(in, slowo))
		{
			kids.append(tmp);
		}
		else if (slowo == "}")
		{
			qInfo() << QString::fromUtf8("Poprawnie odczytano ruch") << Qt::endl;
		}
		else
		{
			qInfo() << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << Qt::endl;
			return nullptr;
		}

	}

	CMovement* obj = new CMovement(seq);

	if (obj)
	{
		if (opis.contains("label"))
			obj->setLabel(opis["label"]);

		if (opis.contains("descr")) obj->setDescr(opis["descr"]);

		for (CBaseObject* kid : kids) add_kid(obj, kid);
	}

	return obj;
}

CBaseObject* CParserATMDL::parseObject(QTextStream& in, QString slowo)
{
	//QMap<QString, std::function<CBaseObject* (QTextStream&)>> mapa;

	//mapa["shell"] = [this](QTextStream& in) -> CBaseObject* { return this->parseObject_shell(in); };

	//if (mapa.contains(slowo)) return mapa[slowo](in);

	if (slowo == "shell")
	{
		return parseObject_shell(in);
	}
	else if ((slowo == "transformation") || (slowo == "trans"))
	{
		return parseObject_transformation(in);
	}
	else if (slowo == "point")
	{
		return parseObject_point(in);
	}
	else if (slowo == "plane")
	{
		return parseObject_plane(in);
	}
	else if (slowo == "sphere")
	{
		return parseObject_sphere(in);
	}
	else if (slowo == "triangle")
	{
		return parseObject_triangle(in);
	}
	else if ((slowo == "animation") || (slowo == "motion") || (slowo == "movement"))
	{
		return parseObject_animation(in);
	}

	return nullptr;
}

void dpTest(const char* msg)
{

}

CModel3D* CParserATMDL::load(const QString path, bool wait)
{
	atmdlFile.setFileName(path);

	if (!atmdlFile.open(QIODevice::ReadOnly | QFile::Text))
	{
		UI::STATUSBAR::setText("Can't open ATML file.");
		return nullptr;
	}

	QTextStream in(&atmdlFile);
	in.setCodec("UTF-8");

	QStack<CBaseObject*> stos;

	CModel3D* root = new CModel3D;

	CObject* currentObject = root;

	QString slowo;

	//qInfo() << "\033[31m" << "To jest komunikat b��du" << "\033[0m" << Qt::endl;
	
	while (!in.atEnd())
	{
		slowo = skip_comments(in);

		if ( (slowo == "define") || (slowo == "replace") )
		{
			in >> slowo;
			//slowo = "$$" + slowo + "$$";

			if (defs.find(slowo) != defs.end()) back_defs[slowo] = defs[slowo];
			
			defs[slowo] = parseType_string(in);
			qInfo() << "\033[33m" << "DEFINICJA: " << slowo << " : " << defs[slowo] << "\033[0m" << Qt::endl;
		}
		else if (CBaseObject* tmp = parseObject(in, slowo))
		{
			add_kid(currentObject, tmp);
		}
		else if (slowo.trimmed().isEmpty())
		{
			//NEUTRALIZUJE PUSTE ZNAKI NA KONCU PLIKU
		}
		else
		{
			qCritical() << "\033[33m" << QString::fromUtf8("BŁĄD. Nierozpoznany symbol ") << slowo << "\033[0m" << Qt::endl;
			break;
		}
	}

	//for (auto d : defs.keys()) qInfo() << d << " : " << defs[d] << Qt::endl;

	for (auto d : back_defs.keys()) defs[d] = back_defs[d];

	if (root->children().size() == 1)
	{
		CBaseObject* tmp = (*root->children().begin()).second.get();

		if (tmp->hasType(CBaseObject::Type::MODEL))
		{
			root->children().clear();
			delete root;
			return (CModel3D*)tmp;
		}
	}

	return root;
}

QString CParserATMDL::obj2atmdl(CBaseObject* obj, const QString& wciecie)
{
	QString text("");

	QString wciecie2 = wciecie + "\t";
	QString wciecie3 = wciecie2 + "\t";

	switch (obj->type())
	{
		case CObject::MODEL:
			text += wciecie + "transformation {\n";
			text += wciecie2 + "label \"" + obj->getLabel() + "\"\n";

			if (!obj->keywords().isEmpty()) {
				QString keywords = "";
				for (const auto& k : obj->keywords()) keywords += ";" + k;
				keywords.remove(0, 1);
				text += wciecie2 + "keywords \"" + keywords + "\"\n";
			}

			text += wciecie2 + "matrix " + ((CModel3D*)obj)->transform().toString("[", "]", ",") + "\n";
			break;
		case CObject::MESH:
		case CObject::CLOUD:
		case CObject::VOLUMETRIC_NEW:
			text += wciecie + "shell {\n";
			text += wciecie2 + "label \"" + obj->getLabel() + "\"\n";
			if (!obj->keywords().isEmpty()) {
				QString keywords = "";
				for (const auto& k : obj->keywords()) keywords += ";" + k;
				keywords.remove(0, 1);
				text += wciecie2 + "keywords \"" + keywords + "\"\n";
			}
			text += wciecie2 + "file \"" + obj->path() + "\"\n";
			break;
		case CObject::MOVEMENT:
			text += wciecie + "animation {\n";
			text += wciecie2 + "label \"" + obj->getLabel() + "\"\n";
			text += wciecie2 + "sequence {\n";
			for (CMovement::FrameVal& fv : ((CMovement*)obj)->m_seqlist)
			{
				text += wciecie3 + "frame { label \"" + fv.getLabel() + "\" delay " + QString::number(double(fv.msec) / 1000) + " matrix " + fv.t.toString("[", "]", ",") + " }\n";
			}
			text += wciecie2 + "}\n";
			break;
		case CBaseObject::POINT:
			text += wciecie + "point {\n";
			text += wciecie2 + "label \"" + obj->getLabel() + "\"\n";

			if (!obj->keywords().isEmpty()) {
				QString keywords = "";
				for (const auto& k : obj->keywords()) keywords += ";" + k;
				keywords.remove(0, 1);
				text += wciecie2 + "keywords \"" + keywords + "\"\n";
			}

			{
				CAnnotationPoint pt = *(CAnnotationPoint*)obj;

				text += QString(wciecie2 + "coords [ %1, %2, %3 ]\n").arg(pt.x).arg(pt.y).arg(pt.z);
				text += QString(wciecie2 + "normal [ %1, %2, %3 ]\n").arg(pt.getRay().x).arg(pt.getRay().y).arg(pt.getRay().z);
			}
			break;
		case CBaseObject::PLANE:
			text += wciecie + "plane {\n";
			text += wciecie2 + "label \"" + obj->getLabel() + "\"\n";

			if (!obj->keywords().isEmpty()) {
				QString keywords = "";
				for (const auto& k : obj->keywords()) keywords += ";" + k;
				keywords.remove(0, 1);
				text += wciecie2 + "keywords \"" + keywords + "\"\n";
			}

			{
				CAnnotationPlane plane = *(CAnnotationPlane*)obj;

				text += QString(wciecie2 + "center [ %1, %2, %3 ]\n").arg(plane.getCenter().x).arg(plane.getCenter().y).arg(plane.getCenter().z);
				text += QString(wciecie2 + "normal [ %1, %2, %3 ]\n").arg(plane.getNormal().x).arg(plane.getNormal().y).arg(plane.getNormal().z);
				text += QString(wciecie2 + "size %1\n").arg(plane.getSize());
			}
			break;
		default:
			return "";
	}

	if (obj->hasCategory(CBaseObject::OBJECT)) {
		if (obj->hasChildren()) {
			for (const auto& k : ((CObject*)obj)->children())
			{
				text += obj2atmdl(k.second.get(), wciecie2);
			}
		}

		for (const auto& k : ((CObject*)obj)->annotations()) {
			text += obj2atmdl(k.second.get(), wciecie2);
		}
	}
	else if (obj->hasCategory(CBaseObject::ANNOTATION)) {
		for (const auto& k : ((CAnnotation*)obj)->annotations()) {
			text += obj2atmdl(k.second.get(), wciecie2);
		}
	}

	text += wciecie + "}\n";
	
	return text;
}

bool CParserATMDL::save(QVector<CBaseObject*> objects, const QString path)
{
	QFile objFile(path);
	if (!objFile.open(QIODevice::WriteOnly | QFile::Text))
	{
		UI::STATUSBAR::setText("Can't create file.");
		return false;
	}
	QTextStream objStream(&objFile);
	objStream.setCodec("UTF-8");
	objStream.setRealNumberNotation(QTextStream::FixedNotation);
	objStream.setRealNumberPrecision(6);

	QString text("# Digital patient workspace file generated by dpVision\n\n");

	for (CBaseObject* obj : objects)
	{
		text += obj2atmdl(obj, "");
	}

	//qInfo() << text;

	objStream << text;

	objFile.close();

	return true;
}

bool CParserATMDL::save(CModel3D* obj, const QString path)
{
	QFile objFile(path);
	if (!objFile.open(QIODevice::WriteOnly | QFile::Text))
	{
		UI::STATUSBAR::setText("Can't create file.");
		return false;
	}

	QTextStream objStream(&objFile);
	objStream.setCodec("UTF-8");
	objStream.setRealNumberNotation(QTextStream::FixedNotation);
	objStream.setRealNumberPrecision(6);

	QString text("# Digital patient workspace file generated by dpVision\n\n");
	text += obj2atmdl(obj, "");

	objStream << text;

	objFile.close();

	return true;
}
