#include "stdafx.h"

#include "ParserVCN.h"
#include "Utilities.h"
#include "PointCloud.h"

#include <fstream>

CParserVCN::CParserVCN()
{
	setDescr("Fast PointCloud format by DP");
	m_exts.insert( "vcn" ); 
	m_save_exts.insert("vcn");
}

CParserVCN::~CParserVCN(void)
{
}

CModel3D* CParserVCN::load(const QString path, bool wait)
{
	CPointCloud* m_cloud = loadVCN(path);

	if (m_cloud != nullptr)
	{
		if (!m_cloud->vertices().empty())
		{
			CModel3D* obj;

			// allocate
			try {
				obj = new CModel3D();
			}
			catch (std::bad_alloc & e) {
				delete m_cloud;
				return nullptr;
			}

			obj->addChild(m_cloud);

			//m_cloud->afterLoad(obj);

			obj->setPath(path);
			obj->setLabel(QFileInfo( path ).fileName());

			obj->importChildrenGeometry();

			UI::STATUSBAR::setText("Done !");

			return obj;
		}
	}

	UI::MESSAGEBOX::error(L"Nie udało sie wczytać pliku");
	return nullptr;
}

bool CParserVCN::save(CModel3D* obj, const QString path)
{
	CPointCloud* mesh = (CPointCloud*)obj->getChild();

	if ((mesh->hasType(CObject::MESH)) || (mesh->hasType(CObject::CLOUD)) || (mesh->hasType(CObject::ORDEREDCLOUD)))
	{
		saveVCN(mesh, path);

		UI::STATUSBAR::setText(L"DONE !");
		return true;
	}

	UI::MESSAGEBOX::error("Sorry, but only models containing vertices,\nlike clouds of points or meshes may be saved in .VC2 format.");

	return false;
}


CPointCloud* CParserVCN::loadVCN(const QString fname)
{
	CPointCloud* mesh = new CPointCloud(nullptr);

	if (nullptr != mesh)
	{
		size_t vsize = 0;
		size_t csize = 0;
		size_t nsize = 0;


		std::ifstream plik(fname.toStdString(), std::ios::binary);

		if (plik.is_open())
		{
			UI::STATUSBAR::setText("Reading points...");
			UI::PROGRESSBAR::init(0, 3, 0);
			plik.read((char*)&vsize, sizeof(size_t));

			mesh->vertices().resize(vsize);

			plik.read((char*)mesh->vertices().data(), vsize * sizeof(CVertex));


			plik.read((char*)&csize, sizeof(size_t));

			UI::PROGRESSBAR::setValue(1);
			if (0 != csize)
			{
				UI::STATUSBAR::setText("Reading colors...");

				mesh->vcolors().resize(csize);

				plik.read((char*)&mesh->vcolors()[0], csize * sizeof(CRGBA));
			}

			plik.read((char*)&nsize, sizeof(size_t));

			UI::PROGRESSBAR::setValue(2);
			if (0 != nsize)
			{
				UI::STATUSBAR::setText("Reading normal vectors...");

				mesh->vnormals().resize(nsize);

				plik.read((char*)&mesh->vnormals()[0], nsize * sizeof(CVector3f));
			}

			plik.close();
		}

		mesh->resetBoundingBox();
		for (const CVertex& v : mesh->vertices())
		{
			mesh->expandBoundingBox(v);
		}

		UI::STATUSBAR::setText("Done !");
		UI::PROGRESSBAR::hide();
	}

	return mesh;
}

void CParserVCN::saveVCN(CPointCloud* mesh, const QString fname)
{
	//	unsigned long t1 = GetTickCount();

	std::ofstream plik(fname.toStdString(), std::ios::binary);

	if (plik.is_open())
	{
		size_t size = mesh->vertices().size();

		plik.write((const char*)&size, sizeof(size_t));

		plik.write((const char*)&mesh->vertices()[0], size * sizeof(CVertex));

		UI::STATUSBAR::printf("%d points have been saved", size);


		size_t csize = mesh->vcolors().size();

		plik.write((const char*)&csize, sizeof(size_t));

		if (0 != csize)
		{
			plik.write((const char*)&mesh->vcolors()[0], csize * sizeof(CRGBA));
		}

		UI::STATUSBAR::printf("%d colors have been saved", csize);

		size_t nsize = mesh->vnormals().size();

		plik.write((const char*)&nsize, sizeof(size_t));

		if (0 != nsize)
		{
			plik.write((const char*)&mesh->vnormals()[0], nsize * sizeof(CVector3f));
		}

		UI::STATUSBAR::printf("%d normals have been saved", nsize);

		plik.close();
	}

	//	unsigned long t2 = GetTickCount();

	//	UI::MESSAGEBOX::information(L"Czas zapisu: " + std::to_wstring(t2 - t1) + L"ms");
}
