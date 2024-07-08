#include "StdAfx.h"

#include "ParserM.h"
#include "Utilities.h"

#include <fstream>
#include <sstream>

//#include "MainApplication.h"

CParserM::CParserM()
{
	setDescr("Matlab .m file");
	m_exts.insert("m");
	m_save_exts.insert("m");
	m_cloud = nullptr;
}

CParserM::~CParserM(void)
{
}


bool CParserM::parseLine(char * bufor)
{
	unsigned int cr, cg, cb;
	float vx, vy, vz;

	CVertex vertex;

	int res = sscanf(bufor, " %f %f %f %u %u %u ", &vx, &vy, &vz, &cr, &cg, &cb);

	if (6 == res)
	{
		vertex.Set(vx, vy, vz);
		m_cloud->vertices().push_back(vertex);
		m_cloud->expandBoundingBox(vertex);

		m_cloud->vcolors().push_back(CRGBA((unsigned char)cr, cg, cb, 255));

		return true;
	}
	else if (3 == res)
	{
		vertex.Set(vx, vy, vz);
		m_cloud->vertices().push_back(vertex);
		m_cloud->expandBoundingBox(vertex);

		m_cloud->vcolors().push_back(CRGBA((unsigned char)255, 255, 255, 255));

		return true;
	}

	return false;
}


size_t CParserM::Run()
{
	if (this->bIsNotSet) return 0;

	UI::MESSAGEBOX::error(L"Sorry, import of M is not implemented yet.");
	return 0;
/*
	m_model->addChild( m_cloud = new CPointCloud(m_model) );
	
	if ( m_cloud == nullptr ) return 0;

 	size_t lbv = 0;
	char bufor[1000];

	FILE *plik = fopen(plikSiatki.absoluteFilePathA().c_str(), "r");

	while (! feof(plik) )
	{
		UI::STATUSBAR::printfTimed(2000, "%d points have been read", lbv);
		bufor[0] = '\0';

		fgets(bufor, 1000, plik);
		
		if ( parseLine(bufor) ) lbv++;
	}

	fclose(plik);

	UI::STATUSBAR::setText("Done !");

	return lbv;
*/
}

bool CParserM::save()
{
	bool result = false;

	CMesh *mesh = (CMesh*) m_model->getChild();

	if ((mesh->hasType(CObject::MESH)) || (mesh->hasType(CObject::CLOUD)) || (mesh->hasType(CObject::ORDEREDCLOUD)))
	{
		std::ofstream plik;

		plik.open( plikSiatki.absoluteFilePathA(), std::ios::out );

		char buf[1000];

		if (plik.is_open())
		{
			plik << "V = [";
			
			for (int i = 0; i < mesh->vertices().size(); i++)
			{
				UI::STATUSBAR::printfTimed(500, L"%d points have been saved", i);

				CVertex &v = mesh->vertices()[i];

				sprintf(buf, "\t%f %f %f;", v.X(), v.Y(), v.Z());
				plik << buf << std::endl;
			}
			plik << "\t];" << std::endl;

			if (mesh->hasType(CObject::MESH) && mesh->hasFaces())
			{
				plik << "T = [";
				for ( auto &f : mesh->faces() )
				{
					sprintf(buf, "\t%d, %d, %d;", f.A()+1, f.B()+1, f.C()+1);
					plik << buf << std::endl;
				}
				plik << "\t];" << std::endl;
			}

			plik.close();
			result = true;
		}
		UI::STATUSBAR::setText(L"DONE !");
	}
	else
	{
		UI::MESSAGEBOX::error("Sorry, but only models containing vertices,\nlike clouds of points or meshes may be saved in .PTS format.");
	}

	return result;
}

