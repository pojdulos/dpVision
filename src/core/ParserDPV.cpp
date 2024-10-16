#include "stdafx.h"

#include "ParserDPV.h"
#include "Utilities.h"

#include "AP.h"

#include <fstream>

#include "quazip.h"
#include "quazipfile.h"
#include "quazipdir.h"


CParserDPVISION::CParserDPVISION()
{
	setDescr("experimental multimodal archive");
	m_exts.insert( "dpvision" ); 
	m_save_exts.insert("dpvision");
}

CParserDPVISION::~CParserDPVISION(void)
{
}

CModel3D* CParserDPVISION::load(const QString path, bool wait)
{
	QuaZip zip(path);
	
	mapaObiektow.clear();

	UI::PROGRESSBAR::init(0, 100, 0);
	int progress = 0;

	if (zip.open(QuaZip::mdUnzip))
	{
		zip.setCurrentFile("structure.xml");

		if (zip.hasCurrentFile())
		{
			QuaZipFile file(&zip);

			if (file.open(QIODevice::ReadOnly))
			{
				QDomDocument doc;

				doc.setContent(file.readAll());

				file.close();

				QDomElement wksp = doc.firstChildElement("workspace");
				//QDomElement wksp = doc.firstChildElement("children");

				QDomElement child = wksp.firstChildElement("object");

				while (!child.isNull())
				{
					UI::PROGRESSBAR::setValue(progress++);

					parseObject(child, zip);
					child = child.nextSiblingElement("object");
				}
			}

		}
	}

	int step = 1;
	if (! mapaObiektow.empty())
	{
		step = (100 - progress) / mapaObiektow.size();
	}
	

	QList<CModel3D*> modele;
	for (const auto& o : mapaObiektow)
	{
		UI::PROGRESSBAR::setValue(progress+=step);

		if (o.ptr->hasCategory(CBaseObject::Category::OBJECT))
		{
			AddressMap::iterator it = mapaObiektow.find(o.parentID);

			if (it==mapaObiektow.end())
			{
				if (o.ptr->hasType(CBaseObject::Type::MODEL))
				{
					modele.push_back((CModel3D*)o.ptr);
				}
				else
				{
					/* ZMODYFIKOWAĆ GDY BEDZIE MOŻLIWOŚĆ DODAWANIA INNYCH OBIEKTOW DO WORKSPACE */

					CModel3D* tmp = new CModel3D;
					tmp->addChild(o.ptr);
					modele.push_back(tmp);
				} 
			}
			else
			{
				((CObject*)it->ptr)->addChild(o.ptr);
			}
		}
		else if (o.ptr->hasCategory(CBaseObject::Category::ANNOTATION))
		{
			AddressMap::iterator it = mapaObiektow.find(o.parentID);

			if (it != mapaObiektow.end())
			{
				((CObject*)it->ptr)->addAnnotation((CAnnotation*)o.ptr);
			}
		}
	}

	UI::PROGRESSBAR::setValue(90);

	if (modele.empty())
	{
		UI::STATUSBAR::setText(L"Nie udało sie wczytać pliku");
		return nullptr;
	}

	for (CModel3D* m : modele)
	{
		m->importChildrenGeometry();
		AP::WORKSPACE::addModel(m);
	}

	UI::PROGRESSBAR::hide();

	UI::STATUSBAR::setText((QString("GOTOWE, liczba wczytanych modeli: ")+QString::number(modele.size())).toStdWString());
	return nullptr;
}


//bool CParserDPVISION::save(CModel3D* obj, const QString path)
//{
//	if (obj == nullptr)	return false;
//
//	QuaZip zip(path);
//
//	if (zip.open(QuaZip::mdCreate))
//	{
//		QByteArray ba;
//		createStructureXml(ba, { obj });
//		writeByteArrayToZip(zip, "structure.xml", ba);
//
//		saveObject(zip, obj, "");
//
//		zip.close();
//		return true;
//	}
//
//	qDebug() << "nie otwarło zipa" << endl;
//	return false;
//}


#include "DockWidgetWorkspace.h"

bool CParserDPVISION::save(CModel3D* obj, const QString path)
{
	//DockWidgetWorkspace *wnd = UI::DOCK::WORKSPACE::instance();

	//QVector<CBaseObject*> objects = wnd->getSelectedObjects();

	//if (objects.empty()) return false;

	QuaZip zip(path);

	if (zip.open(QuaZip::mdCreate))
	{
		QByteArray ba;
		createStructureXml(ba, { obj });
		//createStructureXml(ba, objects);
		writeByteArrayToZip(zip, "structure.xml", ba);

		//for (auto obj : objects)
		//{
			UI::STATUSBAR::setText("saving " + obj->getLabelA());
			saveObject(zip, obj, "");
		//}

		zip.close();
		UI::STATUSBAR::setText("Done!");
		return true;
	}

	qDebug() << "nie otwarto zipa" << Qt::endl;
	return false;
}

#include "Workspace.h"

bool CParserDPVISION::save(QVector<CBaseObject*> objects, const QString path)
{
	if (objects.empty()) return false;

	QuaZip zip(path);

	if (zip.open(QuaZip::mdCreate))
	{
		QByteArray ba;
		createStructureXml(ba, objects);
		writeByteArrayToZip(zip, "structure.xml", ba);

		UI::PROGRESSBAR::init(0, objects.size(), 0);
		int progress = 0;

		for (auto obj : objects)
		{
			UI::PROGRESSBAR::setValue(progress++);

			UI::STATUSBAR::setText("saving " + obj->getLabelA());
			saveObject(zip, obj, "");
		}
		UI::PROGRESSBAR::hide();

		zip.close();
		UI::STATUSBAR::setText("Done!");
		return true;
	}

	qDebug() << "nie otwarło zipa" << Qt::endl;
	return false;
}
