#include "ParserOBJ.h"
#include "Utilities.h"

#include <fstream>

#include "interfaces/IProgressListener.h"
#include "StatusBarManager.h"

CParserOBJ::CParserOBJ(std::shared_ptr<IProgressListener> prg) : CParser(prg)
{
	setDescr("OBJ File");
	m_exts.insert("obj");
	m_save_exts.insert("obj");
}

CParserOBJ::~CParserOBJ(void)
{
}

void CParserOBJ::ParseObjMTLFile(const wchar_t* npl)
{
    float alpha = 1.0;

    CFileInfo info(npl);
    info.setDir(plikSiatki.absolutePath());

    QFile inputFile(info.absoluteFilePath());

    if (!inputFile.open(QIODevice::ReadOnly | QFile::Text))
    {
		StatusBarManager::setText("Can't open file.");
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
        QString qline = in.readLine().trimmed();
        std::string line = qline.toStdString();

        if (qline.startsWith("newmtl", Qt::CaseInsensitive))
        {
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
            std::istringstream iss(line.substr(2));
            float r, g, b;
            iss >> r >> g >> b;
            currentMaterial->FrontColor.ambient.setRGBAf(r, g, b);
        }
        else if (qline.startsWith("Kd", Qt::CaseInsensitive))
        {
            std::istringstream iss(line.substr(2));
            float r, g, b;
            iss >> r >> g >> b;
            currentMaterial->FrontColor.diffuse.setRGBAf(r, g, b);
        }
        else if (qline.startsWith("Ks", Qt::CaseInsensitive))
        {
            std::istringstream iss(line.substr(2));
            float r, g, b;
            iss >> r >> g >> b;
            currentMaterial->FrontColor.specular.setRGBAf(r, g, b);
        }
        else if (qline.startsWith("Ns", Qt::CaseInsensitive))
        {
            // Ns 0.000000
        }
        else if (qline.startsWith("map_Ka", Qt::CaseInsensitive))
        {
            // map_Ka
        }
        else if (qline.startsWith("map_Kd", Qt::CaseInsensitive))
        {
            QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (qlist.size() > 1)
            {
                currentMaterial->TexInfo = qlist[1];
            }
        }
        else if (qline.startsWith("map_Ks", Qt::CaseInsensitive))
        {
            // map_Ks
        }
        else if (qline.startsWith("d", Qt::CaseInsensitive))
        {
            std::istringstream iss(line.substr(2));
            float a;
            iss >> a;
            currentMaterial->FrontColor.ambient.setAlpha(a * 255);
            currentMaterial->FrontColor.diffuse.setAlpha(a * 255);
        }
        else if (qline.startsWith("Tr", Qt::CaseInsensitive))
        {
            std::istringstream iss(line.substr(2));
            float a;
            iss >> a;
            currentMaterial->FrontColor.ambient.setAlpha((1.0 - a) * 255);
            currentMaterial->FrontColor.diffuse.setAlpha((1.0 - a) * 255);
        }
        else if (qline.startsWith("illum", Qt::CaseInsensitive))
        {
            // illum
        }
    }

    if (currentMaterial != nullptr)
    {
        mats[currentName] = currentMaterial;
    }
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

//#include <QtCore/QFile>
//#include "QtCore/QTextStream"


size_t CParserOBJ::Run()
{
    if (this->bIsNotSet) return 0;

	pMeshData = std::make_shared<CMesh>();
    if (pMeshData == nullptr) return 0;

	m_model->addChild(pMeshData);

    meshinfo.clear();

    size_t lbv = 0, lbf = 0, lbti = 0;
    float nx, ny, nz, ax, ay, az, tcs, tct;

    m_firstFace = true;

    CVertex vertex;
    CVector3f WNorm;
    CFace face;
    CTIndex tidx;
    CTCoord tcoord;

	StatusBarManager::setText("Loading in progress...");

	if (progress_) {
		progress_->useCancelButton([this]() { this->cancelled = true; });
		progress_->init(0, 100, 0, "Reading " + plikSiatki.fileName().toStdString() + "...");
	}

    cancelled = false;
    auto t1 = std::chrono::steady_clock::now();

    QFile inputFile(plikSiatki.absoluteFilePath());
    if (!inputFile.open(QIODevice::ReadOnly | QFile::Text))
    {
		StatusBarManager::setText("Can't open file.");
        m_model->removeChild(pMeshData->id());
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
    std::shared_ptr<CModel3D> currentModel = m_model;

    meshinfo[pMeshData->id()] = { true,true,true };

    while (!cancelled && !in.atEnd())
    {
        QString qline = in.readLine().trimmed();
        std::string line = qline.toStdString();

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
                std::shared_ptr<CModel3D> tmpModel = std::make_shared<CModel3D>();
                tmpModel->setLabel(qlist[1]);
                tmpModel->transform().fromRowMatrix(qlist[2], ",");

                for (auto& child : currentModel->children())
                {
                    tmpModel->addChild(child.second);
                }
                currentModel->removeChild(0);
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
            QString descr;
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
            QString descr;
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
            std::istringstream iss(line.substr(2));
            iss >> nx >> ny >> nz;
            tmpN.push_back(CVector3f(nx, ny, nz));
        }
        else if (qline.startsWith("vt", Qt::CaseInsensitive))
        {
            std::istringstream iss(line.substr(2));
            iss >> tcs >> tct;
            tmpTC.push_back(CTCoord(tcs, tct));
        }
        else if (qline.startsWith("v", Qt::CaseInsensitive))
        {
            std::istringstream iss(line.substr(2));
            float cr, cg, cb;
            if (!(iss >> ax >> ay >> az)) continue;

            if (iss >> cr >> cg >> cb)
            {
                tmpV.push_back(CVertex(ax, ay, az));
                tmpC.push_back(CRGBA(cr, cg, cb));
            }
            else
            {
                tmpV.push_back(CVertex(ax, ay, az));
            }

            lbv++;
            tmpBB.expand(CVertex(ax, ay, az));
        }
        else if (qline.startsWith("f", Qt::CaseInsensitive))
        {
            parseF(line, lbf, lbti);
        }
        else if (qline.startsWith("mtllib", Qt::CaseInsensitive))
        {
            std::wstring wline = qline.toStdWString();
            std::wstring ws = &wline[7];
            ParseObjMTLFile(ws.c_str());
        }
        else if (qline.startsWith("g", Qt::CaseInsensitive))
        {
            QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            std::shared_ptr<CObject> parent = currentModel;
            QString name = "mesh";

            if (qlist.size() > 1)
            {
                QStringList qlist2 = qlist[1].split(":", Qt::SkipEmptyParts);
                name = qlist2.last();

                while (qlist2.length() > 1)
                {
                    std::shared_ptr<CBaseObject> hit = parent->getChild(qlist2.first());
                    if (hit != nullptr)
                    {
                        parent = std::dynamic_pointer_cast<CObject>(hit);
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
                parent->addChild(pMeshData = std::make_shared<CMesh>());
            }
            hasGroups = true;
            pMeshData->setLabel(name);
            pMeshData->setPath(plikSiatki.absoluteFilePath());
            meshinfo[pMeshData->id()] = { true,true,true };
        }
        else if (qline.startsWith("usemtl", Qt::CaseInsensitive))
        {
            QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (qlist.size() > 1)
            {
                if (mats.find(qlist[1]) != mats.end())
                    pMeshData->getMaterial(0) = *mats[qlist[1]];
            }
        }

        auto t2 = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

        if (duration > 500)
        {
            int progress = round((100.0f * (inputFile.size() - inputFile.bytesAvailable())) / inputFile.size());
            
			if (progress_) progress_->setValue(progress);

			t1 = std::chrono::steady_clock::now();
        }
    }

	if (progress_) progress_->hide();

    if (cancelled)
    {
		m_model->removeAllChilds();
		StatusBarManager::setText("File loading cancelled!");
        return 0;
    }
    else
    {
        setChildrenVertices(m_model.get(), tmpV, tmpC, tmpN, tmpTC);
        m_model->importChildrenGeometry();

		StatusBarManager::setText("File loading done!");
        return lbv;
    }
}




void CParserOBJ::setChildrenVertices(CObject* obj, CPointCloud::Vertices& tmpV, CPointCloud::Colors& tmpC, CPointCloud::Normals& tmpN, CMaterial::TextureCoordinates& tmpTC)
{
	for (auto& c : obj->children())
	{
		if (c.second->hasType(CObject::Type::MESH))
		{
			CMesh* m = (CMesh*)c.second.get();

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
				
				m->renderSmoothVertex(false);
			}
			m->rebuildBoundingBox();
		}

		setChildrenVertices((CObject*)c.second.get(), tmpV, tmpC, tmpN, tmpTC);
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
			StatusBarManager::setTextTimed(500, QString("Saving vertices: %1").arg(vCnt++));
			
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
			StatusBarManager::setTextTimed(500, QString("Saving vertices: %1").arg(vCnt++));
			
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
			StatusBarManager::setTextTimed(500, QString("Saving normals: %1").arg(vnCnt++));

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
			StatusBarManager::setTextTimed(500, QString("Saving vertices: %1").arg(vCnt++));
			
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
			StatusBarManager::setTextTimed(500, QString("Saving vertices: %1").arg(vCnt++));
			
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
			StatusBarManager::setTextTimed(500, QString("Saving normals: %1").arg(vnCnt++));
			
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
			CModel3D* obj = (CModel3D*)src.second.get();
			objStream << "#@trans " << obj->getLabel() << " " << obj->transform().toString("", "", ",") << Qt::endl;
		}
		else if (src.second->hasType(CBaseObject::Type::MESH))
		{
			CMesh* mesh = (CMesh*)src.second.get();

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
			CPointCloud* cloud = (CPointCloud*)src.second.get();

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

		saveChildren(objStream, mtlStream, (CObject*)src.second.get(), label + ":", vStart);
	}

	labels.clear();
}

bool CParserOBJ::save()
{
	StatusBarManager::setText("CParserOBJ::save() is writing to file, please wait...");

	QFile objFile(plikSiatki.absoluteFilePath());
	if (!objFile.open(QIODevice::WriteOnly | QFile::Text))
	{
		StatusBarManager::setText("Can't create .obj file.");
		return false;
	}


	QFileInfo mtlFInfo(plikSiatki.absoluteDir(), plikSiatki.baseName() + ".mtl");
	QFile mtlFile(mtlFInfo.absoluteFilePath());
	if (!mtlFile.open(QIODevice::WriteOnly | QFile::Text))
	{
		objFile.close();
		StatusBarManager::setText("Can't create .mtl file.");
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
		      << Qt::endl;  // << endl opr�cz zapisania ko�ca linii czy�ci bufor, jest r�wnowa�ne: << "\n" << flush

	objStream << "#@trans " << m_model->getLabel() << " " << m_model->transform().toString("", "", ",") << Qt::endl;

	if (!m_model->getDescr().isEmpty()) {
		QStringList lines = m_model->getDescr().split("\n");
		objStream << "\n#@objDescr begin";
		for (const QString &l : lines) objStream << "\n#" << l;
		objStream << "\n#@objDescr end";
	}

	size_t vStart = 1;

	saveChildren(objStream, mtlStream, m_model.get(), "", vStart);

	mtlFile.close();
	objFile.close();

	return true;
}

