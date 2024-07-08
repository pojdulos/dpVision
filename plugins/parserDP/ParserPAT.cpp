#include "StdAfx.h"

#include "ParserPAT.h"
#include "Utilities.h"

#include <fstream>
#include <sstream>

//#include "MainApplication.h"

CParserPAT::CParserPAT()
{
	setDescr("Patran neutral files");
	m_exts.insert("pat");
	m_save_exts.insert("pat");
	m_mesh = nullptr;
}

CParserPAT::~CParserPAT(void)
{
}


//bool CParserPAT::parseLine(char * bufor)
//{
//	unsigned int cr, cg, cb;
//	float vx, vy, vz;
//
//	CVertex vertex;
//
//	int res = sscanf(bufor, " %f %f %f %u %u %u ", &vx, &vy, &vz, &cr, &cg, &cb);
//
//	if (6 == res)
//	{
//		vertex.Set(vx, vy, vz);
//		m_cloud->vertices().push_back(vertex);
//		m_cloud->expandBoundingBox(vertex);
//
//		m_cloud->vcolors().push_back(CRGBA((unsigned char)cr, cg, cb, 255));
//
//		return true;
//	}
//	else if (3 == res)
//	{
//		vertex.Set(vx, vy, vz);
//		m_cloud->vertices().push_back(vertex);
//		m_cloud->expandBoundingBox(vertex);
//
//		m_cloud->vcolors().push_back(CRGBA((unsigned char)255, 255, 255, 255));
//
//		return true;
//	}
//
//	return false;
//}

#define getline()   \
    {if (NULL == fgets (buffer, sizeof(buffer), plik)) \
        return 0; \
    lineno++;}


size_t CParserPAT::Run()
{
	if (this->bIsNotSet) return 0;

	//UI::MESSAGEBOX::error(L"Sorry, import of PAT is not implemented yet.");
	//return 0;

	m_model->addChild(m_mesh = new CMesh(m_model));

	if (m_mesh == nullptr) return 0;

	size_t lbv = 0;
	size_t lineno = 0;
	char buffer[1000];

	FILE* plik = fopen(plikSiatki.absoluteFilePathA().c_str(), "r");


	int packet;

	getline();

	/* header - packet 25 */

	if ((packet = atoi(buffer)) == 25) {
		getline();
		fputs(buffer, stdout);
		getline();
		packet = atoi(buffer);
	}

	/* summary - packet 26 */

	if (packet == 26) {
		getline();
		getline();
		packet = atoi(buffer);
	}

	/* get remaining packets */

	while (packet != 99) {

		/* node */

		int n, nodeid;
		float xyz[3];
		char* p;

		int elemid, nlines, nnodes, nodes[8];


		if (packet == 1) {
			nodeid = atoi(&buffer[2]);
			getline();
			p = buffer + 48;
			for (n = 2; n >= 0; n--) {
				*p = 0;
				p -= 16;
				xyz[n] = atof(p);
			}
			getline();

			CVertex v(xyz[0], xyz[1], xyz[2]);

			//m_mesh->addVertex( v, CVector3d(v,CVertex(0,0,0) ).getNormalized() );
			m_mesh->addVertex( v );
		}

		/* element */

		else if (packet == 2) {
			elemid = atoi(&buffer[2]);
			n = atoi(&buffer[10]);
			nlines = atoi(&buffer[18]);
			if (n == 5 || n == 7 || n == 8) {
				getline();
				nnodes = n == 8 ? n : n - 1;
				lineno++;
				for (n = 0; n < nnodes; n++)
				{
					if (1 != fscanf(plik, "%d", &nodeid) || nodeid < 1)
					{
						//cgnsImportFatal("missing or invalid node ID");
					}
					nodes[n] = nodeid-1;
				}
				while (getc(plik) != '\n')
					;
				nlines -= 2;
				
				m_mesh->faces().push_back(CFace(nodes[0], nodes[1], nodes[2]));
				//m_mesh->faces().push_back(CFace(nodes[0], nodes[2], nodes[3]));
			}
			while (nlines-- > 0)
				getline();
		}

		/* distributed loads */

		//else if (packet == 6 && do_loads) {
		//	elemid = atoi(&buffer[2]);
		//	loadid = atoi(&buffer[10]);
		//	nlines = atoi(&buffer[18]);

		//	if (loadid != lastid) {
		//		sprintf(buffer, "PatranLoad%d", loadid);
		//		cgnsImportBegReg(buffer, cgnsREG_NODES);
		//		lastid = loadid;
		//	}
		//	getline();

		//	/* add if element load flag is set */

		//	if ('1' == buffer[0])
		//		add_face(elemid, &buffer[9]);
		//	while (--nlines > 0)
		//		getline();
		//}

		///* named component */

		//else if (packet == 21) {
		//	int cnt, type, id;
		//	elemid = atoi(&buffer[2]);
		//	nnodes = atoi(&buffer[10]) / 2;
		//	getline();

		//	/* strip leading and trailing spaces */

		//	buffer[sizeof(buffer) - 1] = 0;
		//	p = buffer + strlen(buffer);
		//	while (--p >= buffer && isspace(*p))
		//		;
		//	*++p = 0;
		//	for (p = buffer; *p && isspace(*p); p++)
		//		;
		//	cgnsImportBegReg(p, cgnsREG_NODES);

		//	/* currently only handle type 5 (nodes) in groups */

		//	for (n = 0, cnt = 0; n < nnodes; n++) {
		//		if (0 == (n % 5))
		//			lineno++;
		//		fscanf(fp, "%d%d", &type, &id);
		//		if (5 == type) {
		//			nodes[cnt++] = id;
		//			if (8 == cnt) {
		//				cgnsImportAddReg(8, nodes);
		//				cnt = 0;
		//			}
		//		}
		//	}
		//	while (getc(fp) != '\n')
		//		;
		//	if (cnt)
		//		cgnsImportAddReg(cnt, nodes);
		//	cgnsImportEndReg();
		//}

		/* all others */

		else {
			nlines = atoi(&buffer[18]);
			while (nlines--)
				getline();
		}

		if (NULL == fgets(buffer, sizeof(buffer), plik))
			break;
		lineno++;
		packet = atoi(buffer);
	}

	fclose(plik);

	CPoint3f ctr = m_mesh->getCenterOfWeight();

	for (CMesh::Faces::iterator itf = m_mesh->faces().begin(); itf != m_mesh->faces().end(); itf++)
	{
		CVector3d fn1 = (*itf).getNormal(m_mesh->vertices());
		
		CVector3d fn2 = CVector3d( ctr, m_mesh->vertices()[(*itf).A()] ).getNormalized()
					+	CVector3d( ctr, m_mesh->vertices()[(*itf).B()] ).getNormalized()
					+	CVector3d( ctr, m_mesh->vertices()[(*itf).C()] ).getNormalized();
		
		fn2.normalize();

		double dd = fn1.dotProduct(fn2);

		if (dd < 0)
			(*itf).invert();
	}

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

*/
	UI::STATUSBAR::setText("Done !");

	return lbv = m_mesh->vertices().size();
}

bool CParserPAT::save()
{
	bool result = false;
	UI::MESSAGEBOX::error(L"Sorry, export of PAT is not implemented yet.");

	//CMesh *mesh = (CMesh*) m_model->getChild();

	//bool saveAsASCII = ( 0 != UI::MESSAGEBOX::question(L"Save PLY file as Binary or ASCII ?", L"Choose format", L"Binary", L"ASCII") );

	//if ((mesh->hasType(CObject::MESH)) || (mesh->hasType(CObject::CLOUD)) || (mesh->hasType(CObject::ORDEREDCLOUD)))
	//{
	//	std::ofstream plik;

	//	if (saveAsASCII)
	//	{
	//		plik.open(plikSiatki.absoluteFilePathW(), std::ios::out);
	//	}
	//	else
	//	{
	//		plik.open(plikSiatki.absoluteFilePathW(), std::ios::out | std::ios::binary);
	//	}

	//	char buf[1000];

	//	if (plik.is_open())
	//	{
	//		plik << "ply" << std::endl;
	//		
	//		if (saveAsASCII)
	//		{
	//			plik << "format ascii 1.0" << std::endl;
	//		}
	//		else
	//		{
	//			plik << "format binary_little_endian 1.0" << std::endl;
	//		}

	//		plik << "comment Exported by dpVision" << std::endl;
	//		plik << "element vertex " << std::to_string(mesh->vertices().size()) << std::endl;
	//		plik << "property float x" << std::endl;
	//		plik << "property float y" << std::endl;
	//		plik << "property float z" << std::endl;
	//		plik << "property uchar red" << std::endl;
	//		plik << "property uchar green" << std::endl;
	//		plik << "property uchar blue" << std::endl;

	//		if ( mesh->hasType(CObject::MESH) && mesh->hasFaces() )
	//		{
	//			plik << "element face " << std::to_string(mesh->faces().size()) << std::endl;
	//			plik << "property list uchar int vertex_indices" << std::endl;
	//		}

	//		plik << "end_header" << std::endl;

	//		for (int i = 0; i < mesh->vertices().size(); i++)
	//		{
	//			UI::STATUSBAR::printfTimed(500, L"%d points have been saved", i);

	//			CVertex &v = mesh->vertices()[i];

	//			if (mesh->vcolors().size() == mesh->vertices().size())
	//			{
	//				CRGBA &c = mesh->vcolors()[i];

	//				if (saveAsASCII)
	//				{
	//					sprintf(buf, "%f %f %f %u %u %u", v.X(), v.Y(), v.Z(), c.R(), c.G(), c.B());
	//					plik << buf << std::endl;
	//				}
	//				else
	//				{
	//					plik.write( (char*)&v, sizeof(CVertex));
	//					plik.write( (char*)&c, 3*sizeof(uchar));
	//					//plik << v.X() << v.Y() << v.Z() << c.R() << c.G() << c.B();
	//				}
	//			}
	//			else
	//			{
	//				if (saveAsASCII)
	//				{
	//					sprintf(buf, "%f %f %f 192 192 192", v.X(), v.Y(), v.Z());
	//					plik << buf << std::endl;
	//				}
	//				else
	//				{
	//					CRGBA c((uchar)192,192,192);
	//					plik.write((char*)&v, sizeof(CVertex));
	//					plik.write((char*)&c, 3 * sizeof(uchar));
	//					//plik << v.X() << v.Y() << v.Z() << 192 << 192 << 192;
	//				}
	//			}
	//		}

	//		if (mesh->hasType(CObject::MESH) && mesh->hasFaces())
	//		{
	//			for ( auto &f : mesh->faces() )
	//			{
	//				if (saveAsASCII)
	//				{
	//					sprintf(buf, "3 %d %d %d", f.A(), f.B(), f.C());
	//					plik << buf << std::endl;
	//				}
	//				else
	//				{
	//					uchar c = 3;
	//					plik.write((char*)&c, sizeof(uchar));
	//					//uint t = f.A();
	//					//plik.write((char*)&t, sizeof(uint));
	//					//t = f.B();
	//					//plik.write((char*)&t, sizeof(uint));
	//					//t = f.C();
	//					//plik.write((char*)&t, sizeof(uint));

	//					plik.write((char*)&f, sizeof(CFace));
	//					//plik << f.A() << f.B() << f.C();
	//				}
	//			}
	//		}

	//		plik.close();
	//		result = true;
	//	}
	//	UI::STATUSBAR::setText(L"DONE !");
	//}
	//else
	//{
	//	UI::MESSAGEBOX::error("Sorry, but only models containing vertices,\nlike clouds of points or meshes may be saved in .PTS format.");
	//}

	return result;
}

