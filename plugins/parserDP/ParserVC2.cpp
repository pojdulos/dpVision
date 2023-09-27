#include "stdafx.h"

#include "ParserVC2.h"
#include "Utilities.h"
#include "OrderedPointCloud.h"

#include <fstream>

CParserVC2::CParserVC2()
{
	setDescr("Fast 3d file format by DP");
	m_exts.insert( "vc2" ); 
	m_save_exts.insert("vc2");
}

CParserVC2::~CParserVC2(void)
{
}


COrderedPointCloud* CParserVC2::loadVC2(const QString fname)
{
	COrderedPointCloud* mesh = new COrderedPointCloud(nullptr);

	if (nullptr != mesh)
	{
		//		unsigned long t1 = GetTickCount();

		size_t rows = 0, cols = 0;

		size_t vsize = 0;
		size_t csize = 0;
		size_t nsize = 0;
		size_t dsize = 0;
		size_t rsize = 0;
		char buf[8];

		std::ifstream plik(fname.toStdString(), std::ios::binary);

		if (plik.is_open())
		{
			plik.read((char*)buf, 8);
			plik.read((char*)&cols, sizeof(size_t));
			plik.read((char*)&rows, sizeof(size_t));

			mesh->m_cols = cols;
			mesh->m_rows = rows;

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

			plik.read((char*)&dsize, sizeof(size_t));

			UI::PROGRESSBAR::setValue(3);
			if (0 != dsize)
			{
				UI::STATUSBAR::setText("Reading distances...");

				mesh->m_dist.resize(dsize);

				plik.read((char*)&mesh->m_dist[0], dsize * sizeof(float));
			}

			plik.read((char*)&rsize, sizeof(size_t));

			UI::PROGRESSBAR::setValue(4);
			if (0 != rsize)
			{
				UI::STATUSBAR::setText("Reading reflections...");

				mesh->m_refl.resize(rsize);

				plik.read((char*)&mesh->m_refl[0], rsize * sizeof(unsigned char));
			}

			plik.close();
		}

		for (const CVertex& v : mesh->vertices())
		{
			mesh->expandBoundingBox(v);
		}

		//		unsigned long t2 = GetTickCount();

		//		UI::MESSAGEBOX::information(L"Czas odczytu: " + std::to_wstring(t2 - t1) + L"ms");
		UI::STATUSBAR::setText("Done !");
		UI::PROGRESSBAR::hide();
	}

	return mesh;
}


CModel3D* CParserVC2::load(const QString path, bool wait)
{
	COrderedPointCloud* m_cloud = loadVC2( path );

	if (m_cloud != nullptr)
	{
		if ( ! m_cloud->vertices().empty() )
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

			obj->addChild( m_cloud );

			//m_cloud->afterLoad(obj);

			obj->setPath( path );
			obj->setLabel( QFileInfo(path).fileName());

			obj->importChildrenGeometry();

			UI::STATUSBAR::setText("Done !");

			return obj;
		}
	}

	UI::MESSAGEBOX::error(L"Nie udało sie wczytać pliku");
	return nullptr;
}


void CParserVC2::saveVC2(COrderedPointCloud* mesh, const QString fname)
{
	//	unsigned long t1 = GetTickCount();
	const char* pre = "vcn_v002";
	std::ofstream plik(fname.toStdString(), std::ios::binary);

	if (plik.is_open())
	{
		//mesh->m_cols = 10014;
		//mesh->m_rows = 4444;

		plik.write((const char*)pre, 8);
		plik.write((const char*)&mesh->m_cols, sizeof(size_t));
		plik.write((const char*)&mesh->m_rows, sizeof(size_t));

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

		size_t dsize = mesh->m_dist.size();

		plik.write((const char*)&dsize, sizeof(size_t));

		if (0 != dsize)
		{
			plik.write((const char*)&mesh->m_dist[0], dsize * sizeof(float));
		}

		UI::STATUSBAR::printf("%d distances have been saved", dsize);

		size_t rsize = mesh->m_refl.size();

		plik.write((const char*)&rsize, sizeof(size_t));

		if (0 != rsize)
		{
			plik.write((const char*)&mesh->m_refl[0], rsize * sizeof(unsigned char));
		}

		UI::STATUSBAR::printf("%d reflections have been saved", rsize);

		plik.close();
	}

	//	unsigned long t2 = GetTickCount();

	//	UI::MESSAGEBOX::information(L"Czas zapisu: " + std::to_wstring(t2 - t1) + L"ms");
}


bool CParserVC2::save(CModel3D* obj, const QString path)
{
	COrderedPointCloud* mesh = (COrderedPointCloud*) obj->getChild();

	//	if ((mesh->hasType(CObject::MESH)) || (mesh->hasType(CObject::CLOUD)) || (mesh->hasType(CObject::ORDEREDCLOUD)))
	if ( mesh->hasType(CObject::ORDEREDCLOUD))
	{
		saveVC2( mesh, path );

		UI::STATUSBAR::setText(L"DONE !");
		return true;
	}

	UI::MESSAGEBOX::error("Sorry, but only models containing vertices,\nlike clouds of points or meshes may be saved in .VC2 format.");

	return false;
}
