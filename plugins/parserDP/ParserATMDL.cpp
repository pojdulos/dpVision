#include "StdAfx.h"

#include "ParserATMDL.h"
#include "Utilities.h"

#include "AnnotationPoint.h"
#include "AP.h"
#include "FileConnector.h"

#include <fstream>
#include <sstream>

//#include "MainApplication.h"

CParserATMDL::CParserATMDL()
{
	setDescr("Generic text files format for AT experiments");
	m_exts.insert("atmdl");
	//m_save_exts.insert("atmdl");
}

CParserATMDL::~CParserATMDL(void)
{
}


CModel3D* CParserATMDL::load(const QString path, bool wait)
{
	QFile inputFile(path);

	if (!inputFile.open(QIODevice::ReadOnly | QFile::Text))
	{
		UI::STATUSBAR::setText("Can't open file.");
		return nullptr;
	}

	QTextStream in(&inputFile);
	in.setCodec("UTF-8");

	CModel3D* currentTransform = new CModel3D;
	CModel3D* topTransform = currentTransform;

	CObject* currentObject = currentTransform;
	CBaseObject* currentBaseObject = currentTransform;
	
	bool hasMoreTrans = false;

	QStack<CModel3D*> stos;
	QStack<CObject*> stos2;

	while (!in.atEnd())
	{
		QString qline = in.readLine().trimmed();
		std::string line = qline.toStdString();

		//UWAGA: split() bardzo spowalnia wczytywanie
		//QStringList qlist = qline.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		//if (qlist.empty()) continue;

		if (qline.startsWith("trans ", Qt::CaseInsensitive))
		{
			QStringList qlist = qline.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			if (qlist.empty()) continue;

			while (qlist.size() > 3)
			{
				qlist[qlist.size() - 2] += qlist[qlist.size() - 1];
				qlist.pop_back();
			}

			if (hasMoreTrans)
			{
				CModel3D* tmpTransform = new CModel3D;
				tmpTransform->setLabel(qlist[1]);
				tmpTransform->transform().fromRowMatrix(qlist[2], ",");

				//for (auto& child : currentTransform->children())
				//{
				//	tmpTransform->addChild(child.second);
				//}
				//currentTransform->removeChild(0); //remove all

				currentObject->addChild(tmpTransform);
				
				currentObject = tmpTransform;
				currentBaseObject = tmpTransform;
			}
			else
			{
				currentTransform->setLabel(qlist[1]);
				currentTransform->transform().fromRowMatrix(qlist[2], ",");
				hasMoreTrans = true;
			}
		}
		else if (qline.startsWith("descr <%", Qt::CaseInsensitive))
		{
			QString descr = qline.mid(8);
			while (!descr.endsWith("%>", Qt::CaseInsensitive))
			{
				QString descrLine = in.readLine().trimmed();
				descr.append("\n"+descrLine);
			}
			descr.chop(2);
			currentBaseObject->setDescr(descr);
		}
		else if (qline.startsWith("model ", Qt::CaseInsensitive) || qline.startsWith("shell ", Qt::CaseInsensitive))
		{
			QStringList qlist = qline.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			if (qlist.empty()) continue;

			CParser* parser = CFileConnector::getLoadParser(qlist[1]);

			QString myPath(qlist[1]);

			if (! QFileInfo(myPath).exists())
			{
				QFileInfo mainfi(inputFile);

				QFileInfo fi(mainfi.absolutePath(), myPath);
				
				if (!fi.exists())
				{
					qWarning() << "File not exists: " << qlist[1] << endl;
					continue;
				}

				myPath = fi.absoluteFilePath();
			}

			CModel3D* tempTransform = parser->load(myPath);

			if (!parser->inPlugin()) delete parser;

			CTransform& t = tempTransform->getTransform();
			if (t.toQMatrix4x4().isIdentity())
			{
				for (const auto& o : tempTransform->children())
				{
					AP::OBJECT::addChild(currentObject, o.second);
					currentObject = (CObject*)o.second;
					currentBaseObject = o.second;
				}
				for (const auto& o : tempTransform->annotations())
				{
					AP::OBJECT::addChild(currentObject, o.second);
				}
			}
			else
			{
				AP::OBJECT::addChild(currentObject, tempTransform);
			}

			if (tempTransform->children().size() > 1)
			{
				currentObject = tempTransform;
				currentBaseObject = tempTransform;
			}

			//for (auto& child : tempTransform->children())
			//{
			//	currentTransform->addChild(child.second);
			//}
			//tempTransform->removeChild(0); //remove all

			//delete tempTransform;
			hasMoreTrans = true;
		}
		else if (qline.startsWith("label ", Qt::CaseInsensitive))
		{
			QStringList qlist = qline.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			if (qlist.size()<2) continue;

			currentObject->setLabel(qlist[1]);
		}
		else if (qline.startsWith("point ", Qt::CaseInsensitive))
		{
			QStringList qlist = qline.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			if (qlist.empty()) continue;

			QStringList qCoords = qlist[2].split(",", QString::SkipEmptyParts);

			CAnnotationPoint* pt = new CAnnotationPoint();

			pt->Set(qCoords[0].toDouble(), qCoords[1].toDouble(), qCoords[2].toDouble());
			pt->setLabel(qlist[1]);

			currentObject->addAnnotation(pt);
			currentBaseObject = pt;
		}
		else if (qline.startsWith("normal ", Qt::CaseInsensitive))
		{
			QStringList qlist = qline.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			if (qlist.empty()) continue;

			for (const auto& a1 : currentObject->annotations())
			{
				if (a1.second->hasType(CAnnotation::POINT))
				{
					CAnnotationPoint* pt = (CAnnotationPoint*)a1.second;
					if (pt->getLabel() == qlist[1])
					{
						//qInfo() << "Wektor normalny w punkcie " << pt->x << "," << pt->y << "," << pt->z << " wynosi " << qlist[2] << endl;

						QStringList qCoords = qlist[2].split(",", QString::SkipEmptyParts);
						pt->setDirection(qCoords[0].toDouble(), qCoords[1].toDouble(), qCoords[2].toDouble());
					}
				}
			}
		}
		else if (qline.startsWith("{", Qt::CaseInsensitive))
		{
			//stos.push(currentTransform);
			stos2.push(currentObject);
		}
		else if (qline.startsWith("}", Qt::CaseInsensitive))
		{
			//currentTransform = stos.pop();
			currentObject = stos2.pop();
			//currentObject = currentTransform;
			currentBaseObject = currentTransform;
		}
		else if (qline.startsWith("#", Qt::CaseInsensitive))
		{
			// skip comments
		}
	}

	return topTransform;
}

//bool CParserATMDL::save(CModel3D* obj, const QString path)
//{
//	return false;
//}


//bool CParserATMDL::save()
//{
//	bool result = false;
//
//	CMesh *mesh = (CMesh*) m_model->getChild();
//
//	if ((mesh->hasType(CObject::MESH)) || (mesh->hasType(CObject::CLOUD)) || (mesh->hasType(CObject::ORDEREDCLOUD)))
//	{
//		std::ofstream plik( plikSiatki.absoluteFilePathW(), std::ios::out );
//
//		char buf[1000];
//
//		if (plik.is_open())
//		{
//			for (int i = 0; i < mesh->vertices().size(); i++)
//			{
//				UI::STATUSBAR::printfTimed(500, L"%d points have been saved", i);
//
//				CVertex &v = mesh->vertices()[i];
//
//				if (mesh->vcolors().size() == mesh->vertices().size())
//				{
//					CRGBA &c = mesh->vcolors()[i];
//					sprintf(buf, "%f %f %f %u %u %u\n", v.X(), v.Y(), v.Z(), c.R(), c.G(), c.B());
//				}
//				else
//				{
//					sprintf(buf, "%f %f %f\n", v.X(), v.Y(), v.Z());
//				}
//				plik.write(buf, strlen(buf));
//			}
//
//			plik.close();
//			result = true;
//		}
//		UI::STATUSBAR::setText(L"DONE !");
//	}
//	else
//	{
//		UI::MESSAGEBOX::error("Sorry, but only models containing vertices,\nlike clouds of points or meshes may be saved in .PTS format.");
//	}
//
//	return result;
//}

