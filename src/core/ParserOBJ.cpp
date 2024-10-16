#include "ParserOBJ.h"
#include "Utilities.h"

#include <fstream>

CParserOBJ::CParserOBJ()
{
	setDescr("OBJ File");
	m_exts.insert("obj");
	m_save_exts.insert("obj");
}

CParserOBJ::~CParserOBJ(void)
{
}


void CParserOBJ::ParseObjMTLFile( const wchar_t *npl )
{
	float alpha = 1.0;

	CFileInfo info(npl);
	info.setDir(plikSiatki.absolutePath());

	QFile inputFile(info.absoluteFilePath());

	if (!inputFile.open(QIODevice::ReadOnly | QFile::Text))
	{
		UI::STATUSBAR::setText("Can't open file.");
		return;
	}

	QTextStream in(&inputFile);
	in.setCodec("UTF-8");

	for (auto& m : mats) delete m.second;
	mats.clear();

	currentName = "default";
	currentMaterial = nullptr;

	while (!cancelled && !in.atEnd())
	{
		QString qline = in.readLine();
		std::string line = qline.toStdString();


		if (qline.startsWith("newmtl", Qt::CaseInsensitive))
		{
			//# material name :
			//newmtl KZdeb_1

			QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
			
			if (currentMaterial != nullptr)
			{
				mats[currentName] = currentMaterial;
			}

			if (mats.find(currentName) != mats.end())
				currentMaterial = new CMaterial(*mats[currentName]);
			else
				currentMaterial = new CMaterial;

			if (qlist.size() > 1) currentName = qlist[1];
			else currentName = "default";
		}
		else if (qline.startsWith("Ka", Qt::CaseInsensitive))
		{
			//# Ka = ambient color
			//Ka 0.540000 0.540000 0.540000
			char* ptr = (char*)&qline.toStdString().c_str()[2];
			float r = std::strtof(ptr, &ptr);
			float g = std::strtof(ptr, &ptr);
			float b = std::strtof(ptr, &ptr);
			currentMaterial->FrontColor.ambient.setRGBAf(r, g, b);
		}
		else if (qline.startsWith("Kd", Qt::CaseInsensitive))
		{
			//# Kd = diffuse color
			//Kd 0.550000 0.550000 0.550000
			char* ptr = (char*)&qline.toStdString().c_str()[2];
			float r = std::strtof(ptr, &ptr);
			float g = std::strtof(ptr, &ptr);
			float b = std::strtof(ptr, &ptr);
			currentMaterial->FrontColor.diffuse.setRGBAf(r, g, b);
		}
		else if (qline.startsWith("Ks", Qt::CaseInsensitive))
		{
			//# Ks = specular color
			//Ks 0.500000 0.500000 0.500000
			char* ptr = (char*)&qline.toStdString().c_str()[2];
			float r = std::strtof(ptr, &ptr);
			float g = std::strtof(ptr, &ptr);
			float b = std::strtof(ptr, &ptr);
			currentMaterial->FrontColor.specular.setRGBAf(r, g, b);
		}
		else if (qline.startsWith("Ns", Qt::CaseInsensitive))
		{
			//# Ns = specular coefficient.
			//Ns 0.000000
		}
		else if (qline.startsWith("map_Ka", Qt::CaseInsensitive))
		{
			//map_Ka KZdeb_1.bmp
		}
		else if (qline.startsWith("map_Kd", Qt::CaseInsensitive))
		{
			//map_Kd KZdeb_1.bmp
			QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
			if (qlist.size() > 1)
			{
				currentMaterial->TexInfo = qlist[1];
			}
		}
		else if (qline.startsWith("map_Ks", Qt::CaseInsensitive))
		{
			//map_Ks KZdeb_1.bmp
		}
		else if (qline.startsWith("d", Qt::CaseInsensitive))
		{
			char* ptr = (char*)&qline.toStdString().c_str()[2];
			float alpha = std::strtof(ptr, &ptr);
			currentMaterial->FrontColor.ambient.setAlpha(alpha * 255);
			currentMaterial->FrontColor.diffuse.setAlpha(alpha * 255);

		}
		else if (qline.startsWith("Tr", Qt::CaseInsensitive))
		{
			//# Tr = transparency, some implementations use 'd' others use 'Tr'
			// Tr = 1.0 - d;
			char* ptr = (char*)&qline.toStdString().c_str()[2];
			float alpha = std::strtof(ptr, &ptr);
			currentMaterial->FrontColor.ambient.setAlpha((1.0 - alpha) * 255);
			currentMaterial->FrontColor.diffuse.setAlpha((1.0 - alpha) * 255);
		}
		else if (qline.startsWith("illum", Qt::CaseInsensitive))
		{
			//# illum:
			//# 0. Color on and Ambient off
			//# 1. Color on and Ambient on
			//# 2. Highlight on
			//# 3. Reflection on and Ray trace on
			//# 4. Transparency: Glass on, Reflection : Ray trace on
			//# 5. Reflection: Fresnel on and Ray trace on
			//# 6. Transparency: Refraction on, Reflection : Fresnel offand Ray trace on
			//# 7. Transparency: Refraction on, Reflection : Fresnel onand Ray trace on
			//# 8. Reflection on and Ray trace off
			//# 9. Transparency: Glass on, Reflection : Ray trace off
			//#10. Casts shadows onto invisible surfaces

			//illum 0
		}
	}

	if (currentMaterial != nullptr)
	{
		mats[currentName] = currentMaterial;
	}

	return;
}

std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter)
{
	std::vector<std::wstring> tokens;
	std::wstring token;
	std::wistringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}


void string_split_optim(std::vector<std::string>& output, const std::string& s, const char delimiter)
{
	output.clear();

	size_t start = 0;
	size_t end = s.find_first_of(delimiter);


	while (end <= std::string::npos)
	{
		output.emplace_back(s.substr(start, end - start));

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = s.find_first_of(delimiter, start);
	}
}


void string_split_to_ints(std::vector<int>& output, const std::string& s, const char delimiter)
{
	output.clear();

	size_t start = 0;
	size_t end = s.find_first_of(delimiter);


	while (end <= std::string::npos)
	{
		output.emplace_back(atoi(s.substr(start, end - start).c_str()));

		if (end == std::string::npos)
			break;

		start = end + 1;
		end = s.find_first_of(delimiter, start);
	}
}



void CParserOBJ::parseF(std::string &line, size_t& lbf, size_t& lbti)
{
	int v1, v2, v3;
	int n1, n2, n3;
	int t1, t2, t3;

	char* ptr = &line[2];
	char* tst;


	if ( m_firstFace )
	{
		m_bN = false;
		m_bT = false;

		v1 = std::strtol(ptr, &ptr, 10);
		if (*ptr == '/')
		{
			tst = ++ptr; // skip slash
			t1 = std::strtol(tst, &ptr, 10);
			if (tst < ptr) m_bT = true;
			if (*ptr == '/')
			{
				tst = ++ptr;  // skip slash
				n1 = std::strtol(tst, &ptr, 10);
				if (tst < ptr) m_bN = true;
			}
		}

		if (!m_bN && !m_bT)
		{
			v2 = std::strtol(ptr, &ptr, 10);
			v3 = std::strtol(ptr, NULL, 10);
		}
		else if (m_bN && m_bT)
		{
			v2 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t2 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			n2 = std::strtol(ptr, &ptr, 10);

			v3 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t3 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			n3 = std::strtol(ptr, NULL, 10);
		}
		else if (m_bT)
		{
			v2 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t2 = std::strtol(ptr, &ptr, 10);

			v3 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t3 = std::strtol(ptr, NULL, 10);
		}
		else if (m_bN)
		{
			v2 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			ptr++; // skip slash
			n2 = std::strtol(ptr, &ptr, 10);

			v3 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			ptr++; // skip slash
			n3 = std::strtol(ptr, NULL, 10);
		}

		m_firstFace = false;
	}
	else
	{
		if (!m_bN && !m_bT)
		{
			v1 = std::strtol(ptr, &ptr, 10); 
			v2 = std::strtol(ptr, &ptr, 10);
			v3 = std::strtol(ptr, NULL, 10);
		}
		else if (m_bN && m_bT)
		{
			v1 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t1 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			n1 = std::strtol(ptr, &ptr, 10);

			v2 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t2 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			n2 = std::strtol(ptr, &ptr, 10);

			v3 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t3 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			n3 = std::strtol(ptr, NULL, 10);
		}
		else if (m_bT)
		{
			v1 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t1 = std::strtol(ptr, &ptr, 10);

			v2 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t2 = std::strtol(ptr, &ptr, 10);

			v3 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			t3 = std::strtol(ptr, NULL, 10);
		}
		else if (m_bN)
		{
			v1 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			ptr++; // skip slash
			n1 = std::strtol(ptr, &ptr, 10);

			v2 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			ptr++; // skip slash
			n2 = std::strtol(ptr, &ptr, 10);

			v3 = std::strtol(ptr, &ptr, 10);
			ptr++; // skip slash
			ptr++; // skip slash
			n3 = std::strtol(ptr, NULL, 10);
		}
	}
	pMeshData->faces().emplace_back( v1 - 1, v2 - 1, v3 - 1 );
	lbf++;

	//if (bIsFN)
	//{
		//pMeshData->fnormals().push_back(CFace(f1v - 1, f2v - 1, f3v - 1).getNormal(pMeshData->vertices()));
	//}

	if (m_bT)
	{
		pMeshData->getMaterial().texindex.emplace_back( t1 - 1, t2 - 1, t3 - 1 );
		lbti++;
	}
}

#include "ProgressIndicator.h"
#include <QtCore/QFile>
#include "QtCore/QTextStream"




size_t CParserOBJ::Run()
{
	if (this->bIsNotSet) return 0;

	m_model->addChild(pMeshData = new CMesh(m_model));
	if (pMeshData == NULL) return 0;

	meshinfo.clear();

	size_t lbv = 0, lbf = 0, lbti = 0;
	float nx, ny, nz, ax, ay, az, tcs, tct;

	m_firstFace = true;

	CVertex vertex;
	CVector3f WNorm;
	CFace face;
	CTIndex tidx;
	CTCoord tcoord;

	ProgressIndicator* progressIndicator = UI::PROGRESSBAR::instance();

	progressIndicator->init(0, 100, 0, "Reading " + plikSiatki.fileName() + "...");

	connect(this, SIGNAL(sendProgress(int)), progressIndicator, SLOT(setValue(int)));
	connect(progressIndicator->cancelButton(), SIGNAL(clicked()), this, SLOT(onLoadCancelled()));
	progressIndicator->cancelButton()->show();

	cancelled = false;
	DWORD t1 = GetTickCount() + 1000;

	QFile inputFile( plikSiatki.absoluteFilePath() );

	if (!inputFile.open(QIODevice::ReadOnly | QFile::Text))
	{
		UI::STATUSBAR::setText("Can't open file.");
		m_model->removeChild(pMeshData->id());
		delete pMeshData;
		return 0;
	}

	pMeshData->setPath(plikSiatki.absoluteFilePath());
	m_model->setPath(plikSiatki.absoluteFilePath());
	m_model->setLabel(plikSiatki.fileName());

	QTextStream in(&inputFile);
	in.setCodec("UTF-8");

	CPointCloud::Vertices tmpV;
	CPointCloud::Colors tmpC;
	CPointCloud::Normals tmpN;
	CMaterial::TextureCoordinates tmpTC;
	CBoundingBox tmpBB;

	bool hasGroups = false;
	bool hasMoreTrans = false;
	CModel3D* currentModel = m_model;

	meshinfo[pMeshData->id()] = { true,true,true };

	//pMeshData->setSelfVisibility(false);

	while (!cancelled && !in.atEnd())
	{
		QString qline = in.readLine();
		std::string line = qline.toStdString();

		//UWAGA: split() bardzo spowalnia wczytywanie
		//QStringList qlist = qline.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		//if (qlist.empty()) continue;

		if (qline.startsWith("#@trans", Qt::CaseInsensitive))
		{
			QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

			while (qlist.size() > 3)
			{
				qlist[qlist.size() - 2] += qlist[qlist.size() - 1];
				qlist.pop_back();
			}

			if (hasMoreTrans)
			{
				CModel3D* tmpModel = new CModel3D;
				tmpModel->setLabel(qlist[1]);
				tmpModel->transform().fromRowMatrix(qlist[2], ",");

				for (auto& child : currentModel->children())
				{
					tmpModel->addChild(child.second);
				}
				currentModel->removeChild(0); //remove all

				currentModel->addChild(tmpModel);
				currentModel = tmpModel;
			}
			else
			{
				m_model->setLabel(qlist[1]);
				m_model->transform().fromRowMatrix(qlist[2], ",");
				hasMoreTrans = true;
			}
		}
		else if (qline.startsWith("#@objDescr begin", Qt::CaseInsensitive))
		{
			QString descr("");
			QString descrLine = in.readLine();
			while (!descrLine.startsWith("#@objDescr end", Qt::CaseInsensitive))
			{
				descr.append(descrLine.midRef(1) + "\n");
				descrLine = in.readLine();
			}
			m_model->setDescr(descr);
		}
		else if (qline.startsWith("#@gDescr begin", Qt::CaseInsensitive))
		{
			QString descr("");
			QString descrLine = in.readLine();
			while (!descrLine.startsWith("#@gDescr end", Qt::CaseInsensitive))
			{
				descr.append(descrLine.midRef(1) + "\n");
				descrLine = in.readLine();
			}
			pMeshData->setDescr(descr);
		}
		else if (qline.startsWith("#@novc", Qt::CaseInsensitive))
		{
			meshinfo[pMeshData->id()].hasVC = false;
		}
		else if (qline.startsWith("vn", Qt::CaseInsensitive))
		{
			// Normals in (x,y,z) form; normals might not be unit.
			// vn 0.707 0.000 0.707

			char* ptr = &line[2];
			//char* ptr = (char*) & qline.toStdString().c_str()[2];
			nx = std::strtof(ptr, &ptr);
			ny = std::strtof(ptr, &ptr);
			nz = std::strtof(ptr, &ptr);
			//pMeshData->vnormals().push_back(CVector3f(nx, ny, nz));
			tmpN.push_back(CVector3f(nx, ny, nz));
		}
		else if (qline.startsWith("vt", Qt::CaseInsensitive))
		{
			// Texture coordinates, in (u,v[,w]) coordinates, w is optional.
			// vt 0.500 -1.352 [0.234]

			//qline
			char* ptr = &line[2];
			tcs = std::strtof(ptr, &ptr);
			tct = std::strtof(ptr, &ptr);
			//pMeshData->getMaterial(0).texcoord.push_back(CTCoord(tcs, tct));
			tmpTC.push_back(CTCoord(tcs, tct));
		}
		else if (qline.startsWith("v", Qt::CaseInsensitive))
		{
			float cr, cg, cb;

			char* ptr = &line[2];
			//char* ptr = (char*)&qline.toStdString().c_str()[2];
			ax = std::strtof(ptr, &ptr);
			ay = std::strtof(ptr, &ptr);
			az = std::strtof(ptr, &ptr);

			char* tst = ptr;
			cr = std::strtof(ptr, &ptr);

			if (ptr > tst)
			{
				cg = std::strtof(ptr, &ptr);
				cb = std::strtof(ptr, NULL);

				//pMeshData->addVertex(CVertex(ax, ay, az), CRGBA(cr, cg, cb));
				tmpV.push_back(CVertex(ax, ay, az));
				tmpC.push_back(CRGBA(cr, cg, cb));
				lbv++;
			}
			else
			{
				//pMeshData->addVertex(CVertex(ax, ay, az));
				tmpV.push_back(CVertex(ax, ay, az));
				lbv++;
			}
			tmpBB.expand(CVertex(ax, ay, az));
		}
		else if (qline.startsWith("f", Qt::CaseInsensitive))
		{
			parseF(line, lbf, lbti);
		}
		else if (qline.startsWith("mtllib", Qt::CaseInsensitive))
		{
			//mtllib ./duck.obj.mtlStream
			std::wstring wline = qline.toStdWString();
			// Wszystko od 8 znaku do koñca linii
			std::wstring ws = &wline[7];

			ParseObjMTLFile(ws.c_str());
		}
		//else if (qline.startsWith("#@part", Qt::CaseInsensitive))
		//{
		//	QStringList qlist = qline.split(QRegExp("\\s+"), QString::SkipEmptyParts);

		//	QString name = "mesh";
		//	CObject* parent = m_model;
		//	for (const QString &ql : qlist)
		//	{
		//		QStringList qlist2 = ql.split(QRegExp("="), QString::SkipEmptyParts);
		//		if (qlist2[0].startsWith("#@part", Qt::CaseInsensitive))
		//		{
		//			name = qlist2[1];
		//		}
		//		else if (qlist2[0].startsWith("parent", Qt::CaseInsensitive))
		//		{
		//			if (parent->getLabel() != qlist2[1])
		//			{
		//				for (auto c : parent->children())
		//				{

		//				}
		//			}
		//		}
		//	}

		//	if (hasGroups) m_model->addChild(pMeshData = new CMesh);
		//	hasGroups = true;
		//	if (qlist.size() > 1)
		//	{
		//		pMeshData->setLabel(qlist[1]);
		//	}
		//}
		else if (qline.startsWith("g", Qt::CaseInsensitive))
		{
			QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
			
			CObject* parent = currentModel;
			QString name = "mesh";

			if (qlist.size() > 1)
			{
				QStringList qlist2 = qlist[1].split(":", Qt::SkipEmptyParts);

				name = qlist2.last();

				while (qlist2.length() > 1)
				{
					CBaseObject* hit = parent->getChild(qlist2.first());

					if (hit!=nullptr)
					{
						parent = (CObject*)hit;
						qlist2.pop_front();
					}
					else
					{
						break;
					}
				}
			}

			if (hasGroups)
			{
				parent->addChild(pMeshData = new CMesh);
			}
			hasGroups = true;
			pMeshData->setLabel(name);
			pMeshData->setPath(plikSiatki.absoluteFilePath());
			meshinfo[pMeshData->id()] = { true,true,true };
		}
		else if (qline.startsWith("usemtl", Qt::CaseInsensitive))
		{
			//usemtl [material name]

			QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
			if (qlist.size() > 1)
			{
				if (mats.find(qlist[1])!=mats.end())
					pMeshData->getMaterial(0) = *mats[qlist[1]];
			}
		}
		//else if (qline.startsWith("o", Qt::CaseInsensitive))
		//{
		//}
		//else if (qline.startsWith("s", Qt::CaseInsensitive))
		//{
		//}
		//else if (qline.startsWith("#", Qt::CaseInsensitive))
		//{
		//}
		//else
		//{
		//}

		if ( t1 < GetTickCount() )
		{
			int progress = round((100.0f * (inputFile.size() - inputFile.bytesAvailable())) / inputFile.size());
			emit(sendProgress(progress));
			t1 = GetTickCount() + 1000;
		}
	}

	progressIndicator->hide();

	if (cancelled)
	{
		for (const CObject::Children::value_type &c : m_model->children())
		{
			m_model->removeChild(c.second->id());
			delete c.second;
		}
		UI::STATUSBAR::setText("File loading cancelled!");
		return 0;
	}
	else
	{
		setChildrenVertices(m_model, tmpV, tmpC, tmpN, tmpTC);

		m_model->importChildrenGeometry();

		UI::STATUSBAR::setText("File loading done!");
		return lbv;
	}
}

void CParserOBJ::setChildrenVertices(CObject* obj, CPointCloud::Vertices& tmpV, CPointCloud::Colors& tmpC, CPointCloud::Normals& tmpN, CMaterial::TextureCoordinates& tmpTC)
{
	for (auto& c : obj->children())
	{
		if (c.second->hasType(CObject::Type::MESH))
		{
			CMesh* m = (CMesh*)c.second;

			m->vertices() = tmpV;
			if (meshinfo[m->id()].hasVC) m->vcolors() = tmpC;
			m->vnormals() = tmpN;
			m->getMaterial(0).texcoord = tmpTC;

			m->attachTexture(plikSiatki.path());
			m->renderWithTexture(m->hasTexture() && m->getMaterial().hasTextureCoords() && m->getMaterial().hasTextureIndexes());

			if (m->hasFaces())
			{
				m->removeUnusedVertices();
				m->renderSmoothVertex(!m->vnormals().empty());
			}
			else
			{
				m->rebuildBoundingBox();
				m->renderSmoothVertex(false);
			}
		}

		setChildrenVertices((CObject*)c.second, tmpV, tmpC, tmpN, tmpTC);
	}
}

void CParserOBJ::saveMTL(QTextStream& mtlStream, CMaterial& material, QString matName)
{
	mtlStream << "\nnewmtl " << matName << "\n";
	
	mtlStream << "Ka " << material.FrontColor.ambient.fR() << " "
		               << material.FrontColor.ambient.fG() << " "
		               << material.FrontColor.ambient.fB() << "\n";

	mtlStream << "Kd " << material.FrontColor.diffuse.fR() << " "
		               << material.FrontColor.diffuse.fG() << " "
		               << material.FrontColor.diffuse.fB() << "\n";

	mtlStream << "Ks " << material.FrontColor.specular.fR() << " " 
		               << material.FrontColor.specular.fG() << " "
		               << material.FrontColor.specular.fB() << "\n";

	mtlStream << "illum 0\n";

	if (material.hasTexture())
	{
		QFileInfo orgTexFile(material.TexInfo);
		QString orgName = orgTexFile.fileName();

		mtlStream << "map_Ka " << orgName << "\n";
		mtlStream << "map_Kd " << orgName << "\n";
		mtlStream << "map_Ks " << orgName << "\n";

		//QImage texImage(orgTexFile.absoluteFilePath());
		//QFileInfo newTexFile(orgTexFile.absoluteDir(), matName + ".bmp");
		//texImage.save(newTexFile.absoluteFilePath());
		//QString newName = newTexFile.fileName();
		//mtlStream << "map_Ka " << newName << "\n";
		//mtlStream << "map_Kd " << newName << "\n";
		//mtlStream << "map_Ks " << newName << "\n";
	}
}

void  CParserOBJ::saveMesh(QTextStream& objFile, CMesh* src, size_t& vStart)
{
	int vCnt = 0;

	if (src->hasVertexColors())
	{
		CMesh::Colors::iterator itcol = src->vcolors().begin();
		for (CMesh::Vertices::iterator itv = src->vertices().begin(); itv != src->vertices().end(); itv++)
		{
			UI::STATUSBAR::printfTimed(500, L"Saving vertices: %d", vCnt++);
			
			objFile << "v " << itv->X() << " "
				            << itv->Y() << " "
				            << itv->Z() << " "
				            << itcol->fR() << " " 
				            << itcol->fG() << " " 
				            << itcol->fB() << "\n";
			
			itcol++;
		}
	}
	else
	{
		for (CMesh::Vertices::iterator itv = src->vertices().begin(); itv != src->vertices().end(); itv++)
		{
			UI::STATUSBAR::printfTimed(500, L"Saving vertices: %d", vCnt++);
			
			//objFile << "v " << itv->X() << " "
			//	            << itv->Y()	<< " "
			//	            << itv->Z() << "\n";
			
			objFile << "v " << itv->X() << " "
				<< itv->Y() << " "
				<< itv->Z() << " "
				<< "\n";
		}
	}

	if (src->hasVertexNormals())
	{
		int vnCnt = 0;
		for (CMesh::Normals::iterator itv = src->vnormals().begin(); itv != src->vnormals().end(); itv++)
		{
			UI::STATUSBAR::printfTimed(500, L"Saving normals: %d", vnCnt++);

			objFile << "vn " << itv->X() << " "
				             << itv->Y() << " "
				             << itv->Z() << "\n";
		}
	}

	for (CMesh::Materials::iterator im = src->materials().begin(); im != src->materials().end(); im++)
	{
		if (!(*im)->texcoord.empty())
		{
			for (CMaterial::TextureCoordinates::iterator itc = (*im)->texcoord.begin(); itc != (*im)->texcoord.end(); itc++)
			{
				objFile << "vt " << (*itc)[0] << " "
					<< (*itc)[1] << "\n";
			}
		}
		else
		{
			for (int i = 0; i < src->vertices().size(); i++)
			{
				objFile << "vt 0.0 0.0\n";
			}
		}
	}

	CMesh::Faces::iterator itf = src->faces().begin();
	CMesh::Normals::iterator itn = src->fnormals().begin();

	for (CMesh::Materials::iterator im = src->materials().begin(); im != src->materials().end(); im++)
	{
		for (CMaterial::TextureIndexes::iterator iti = (*im)->texindex.begin(); iti != (*im)->texindex.end(); iti++)
		{
			//if (src->hasFaceNormals())
			//{
			//	fprintf(objFile, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", itf->A() + 1, iti->a + 1, itn->X() + 1, itf->B() + 1, iti->b + 1, itn->Y() + 1, itf->C() + 1, iti->c + 1, itn->Z() + 1);
			//	itf++;
			//	itn++;
			//}
			//else
			//{

			objFile << "f " << vStart + itf->A() << "/" << vStart + iti->a << " "
				            << vStart + itf->B() << "/" << vStart + iti->b << " "
				            << vStart + itf->C() << "/" << vStart + iti->c << "\n";
			
			itf++;
			//}
		}
	}

	while (itf != src->faces().end())
	{
		objFile << "f " << vStart + itf->A() << " "
			            << vStart + itf->B() << " "
			            << vStart + itf->C() << "\n";

		//fprintf(objFile, "f %d//%f %d//%f %d//%f\n", itf->A() + 1, itn->X() + 1, itf->B() + 1, itn->Y() + 1, itf->C() + 1, itn->Z() + 1);

		itf++;
		//itn++;
	}
	
	vStart += vCnt;
}

void  CParserOBJ::saveCloud(QTextStream& objFile, CPointCloud* src, size_t& vStart)
{
	int vCnt = 0;

	if (src->hasVertexColors())
	{
		CPointCloud::Colors::iterator itcol = src->vcolors().begin();
		for (CPointCloud::Vertices::iterator itv = src->vertices().begin(); itv != src->vertices().end(); itv++)
		{
			UI::STATUSBAR::printfTimed(500, L"Saving vertices: %d", vCnt++);
			
			objFile << "v " << itv->X() << " "
				            << itv->Y() << " "
				            << itv->Z() << " "
				            << itcol->fR() << " "
				            << itcol->fG() << " "
				            << itcol->fB() << "\n";

			itcol++;
		}
	}
	else
	{
		for (CPointCloud::Vertices::iterator itv = src->vertices().begin(); itv != src->vertices().end(); itv++)
		{
			UI::STATUSBAR::printfTimed(500, L"Saving vertices: %d", vCnt++);
			
			objFile << "v " << itv->X() << " "
				            << itv->Y() << " "
				            << itv->Z() << "\n";

		}
	}

	if (src->hasVertexNormals())
	{
		int vnCnt = 0;
		for (CPointCloud::Normals::iterator itv = src->vnormals().begin(); itv != src->vnormals().end(); itv++)
		{
			UI::STATUSBAR::printfTimed(500, L"Saving normals: %d", vnCnt++);
			
			objFile << "vn " << itv->X() << " "
				             << itv->Y() << " "
				             << itv->Z() << "\n";

		}
	}

	vStart += vCnt;
}


void CParserOBJ::saveChildren(QTextStream& objStream, QTextStream& mtlStream, CObject* obj, QString prefix, size_t& vStart)
{
	QSet<QString> labels;

	for (const auto& src : obj->children())
	{
		QString label(src.second->getLabel());

		int lblCnt = 0;
		while (labels.find(label) != labels.end())
		{
			lblCnt++;
			label = src.second->getLabel() + "_" + QString::number(lblCnt);
		}
		labels.insert(label);

		label = prefix + label;

		if (src.second->hasType(CBaseObject::Type::MODEL))
		{
			CModel3D* obj = (CModel3D*)src.second;
			objStream << "#@trans " << obj->getLabel() << " " << obj->transform().toString("", "", ",") << Qt::endl;
		}
		else if (src.second->hasType(CBaseObject::Type::MESH))
		{
			CMesh* mesh = (CMesh*)src.second;

			objStream
			//	<< "\n#@part=" << label
			//	<< " parent=" << obj->getLabel()
			//	<< " type=mesh vertices=" << mesh->vertices().size()
			//	<< " faces=" << mesh->faces().size()
				<< "\ng " << label;

			if (!src.second->getDescr().isEmpty()) {
				QStringList lines = src.second->getDescr().split("\n");
				objStream << "\n#@gDescr begin";
				for (const QString &l : lines) objStream << "\n#" << l;
				objStream << "\n#@gDescr end";
			}

			if (mesh->vcolors().empty()) objStream << "\n#@novc\n";

			if (!mesh->materials().empty())
			{
				objStream << "\nusemtl " << label;

				saveMTL(mtlStream, mesh->getMaterial(), label);
			}

			objStream << "\ns\n";

			saveMesh(objStream, mesh, vStart);

			objStream << Qt::flush; // zapisuje dane z bufora na dysk i czysci bufor dla kolejnego mesha
		}
		else if (src.second->hasType(CBaseObject::Type::CLOUD) || src.second->hasType(CBaseObject::Type::ORDEREDCLOUD))
		{
			CPointCloud* cloud = (CPointCloud*)src.second;

			objStream
			//	<< "\n#@part=" << label
			//	<< " parent=" << obj->getLabel()
			//	<< " type=cloud vertices=" << cloud->vertices().size()
				<< "\ng " << label;

			if (!src.second->getDescr().isEmpty()) {
				QStringList lines = src.second->getDescr().split("\n");
				objStream << "\n#@gDescr begin";
				for (const QString &l : lines) objStream << "\n#" << l;
				objStream << "\n#@gDescr end";
			}

			if (cloud->vcolors().empty()) objStream << "\n#@novc\n";

			if (!cloud->materials().empty())
			{
				objStream << "\nusemtl " << label;

				saveMTL(mtlStream, cloud->getMaterial(), label);
			}

			objStream << "\ns\n";

			saveCloud(objStream, cloud, vStart);

			objStream << Qt::flush;
		}

		saveChildren(objStream, mtlStream, (CObject*)src.second, label + ":", vStart);
	}

	labels.clear();
}

bool CParserOBJ::save()
{
	UI::STATUSBAR::setText("CParserOBJ::save() is writing to file, please wait...");

	QFile objFile(plikSiatki.absoluteFilePath());
	if (!objFile.open(QIODevice::WriteOnly | QFile::Text))
	{
		UI::STATUSBAR::setText("Can't create .obj file.");
		return false;
	}


	QFileInfo mtlFInfo(plikSiatki.absoluteDir(), plikSiatki.baseName() + ".mtl");
	QFile mtlFile(mtlFInfo.absoluteFilePath());
	if (!mtlFile.open(QIODevice::WriteOnly | QFile::Text))
	{
		objFile.close();
		UI::STATUSBAR::setText("Can't create .mtl file.");
		return false;
	}

	QTextStream objStream(&objFile);
	objStream.setCodec("UTF-8");
	objStream.setRealNumberNotation(QTextStream::FixedNotation);
	objStream.setRealNumberPrecision(6);

	QTextStream mtlStream(&mtlFile);
	mtlStream.setCodec("UTF-8");

	objStream << "#\n# OBJ File Generated by dpVision\n"
	          << "#\n# Object name: " << plikSiatki.fileName()
	          << "\n\nmtllib " << mtlFInfo.fileName()
		      << Qt::endl;  // << endl oprócz zapisania koñca linii czyœci bufor, jest równowa¿ne: << "\n" << flush

	objStream << "#@trans " << m_model->getLabel() << " " << m_model->transform().toString("", "", ",") << Qt::endl;

	if (!m_model->getDescr().isEmpty()) {
		QStringList lines = m_model->getDescr().split("\n");
		objStream << "\n#@objDescr begin";
		for (const QString &l : lines) objStream << "\n#" << l;
		objStream << "\n#@objDescr end";
	}

	size_t vStart = 1;

	saveChildren(objStream, mtlStream, m_model, "", vStart);

	mtlFile.close();
	objFile.close();

	return true;
}


void CParserOBJ::onLoadCancelled()
{
	cancelled = true;
}
