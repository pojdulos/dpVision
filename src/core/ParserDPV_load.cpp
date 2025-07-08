#include "stdafx.h"

#include "ParserDPV.h"
#include "Utilities.h"

#include "Objects"
#include "Annotations"
#include "AP.h"

#include <fstream>

#include "quazip.h"
#include "quazipfile.h"
#include "quazipdir.h"

CObject* CParserDPVISION::readZippedFileObj(QuaZip& zip, QString pathInZip)
{
	QString objPath = pathInZip + ".obj";
	
	qDebug() << "Reading " << objPath << Qt::endl;
	
	zip.setCurrentFile(objPath);

	QuaZipFile file(&zip);

	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray ba = file.readAll();

		file.close();

		QTextStream objStream(ba);

		return parseOBJ(objStream, zip, pathInZip);
	}


	qDebug() << "B£¥D! nie otwarto pliku" << Qt::endl;
	return nullptr;
}




CBaseObject* CParserDPVISION::parseObject(const QDomElement& currentElement, QuaZip& zip)
{
	QString currentId = currentElement.attribute("id");
	QString path = currentElement.attribute("path");

	CBaseObject* obj = nullptr;

	QString typ = currentElement.attribute("type");
	QString cls = currentElement.attribute("class");

	if (cls == "object")
	{
		if (typ == "transformation")
		{
			obj = new CModel3D;
			QString matrix = currentElement.firstChildElement("matrix").text();
			qDebug() << matrix << Qt::endl;

			((CModel3D*)obj)->transform().fromRowMatrix(matrix);
		}
		else if (typ == "mesh")
		{
			obj = readZippedFileObj(zip, path + currentId);
		}
		else if (typ == "volumetric")
		{
			size_t lays = currentElement.attribute("layers").toULong();
			size_t rows = currentElement.attribute("rows").toULong();
			size_t cols = currentElement.attribute("columns").toULong();

			QDomElement voxel = currentElement.firstChildElement("voxelSize");

			CPoint3d voxelSize;
			voxelSize.x = voxel.attribute("x").toDouble();
			voxelSize.y = voxel.attribute("y").toDouble();
			voxelSize.z = voxel.attribute("z").toDouble();

			QString objPath = path + currentId + ".raw";

			qDebug() << "Reading " << objPath << Qt::endl;

			zip.setCurrentFile(objPath);

			QuaZipFile file(&zip);

			if (file.open(QIODevice::ReadOnly))
			{
				QByteArray ba = file.readAll();

				file.close();

				QDataStream rawStream(&ba, QIODevice::ReadOnly);

				CVolTK* volTK = new CVolTK(nullptr, cols, rows, lays, 16);
				
				int dataSize = volTK->kostka.size() * sizeof(NowaKostka1::Type);
				int read = rawStream.readRawData((char*)volTK->kostka.layer(0), dataSize);
				
				//volTK->m_voxelSize = voxelSize;
				volTK->createDisplayData();

				obj = volTK;
			}
			else
			{

				qDebug() << "B£¥D! nie otwarto pliku" << Qt::endl;
				//return nullptr;
			}
		}
	}
	else if (cls == "annotation")
	{
		if (typ == "setOfVertices")
		{
			obj = new CAnnotationSetOfVertices;

			QDomElement dataset = currentElement.firstChildElement("dataset");

			QString destination = dataset.attribute("destination");

			AddressMap::iterator it = mapaObiektow.find(destination);
			if (it != mapaObiektow.end()) ((CAnnotationSetOfVertices*)obj)->setDest((CPointCloud*)it->ptr);

			((CAnnotationSetOfVertices*)obj)->m_pointSize = dataset.attribute("pointSize","5").toInt();

			QStringList sl = dataset.text().split(" ");
			for (const auto s : sl)
			{
				((CAnnotationSetOfVertices*)obj)->insert(s.toInt());
			}
		}
		else if (typ == "setOfFaces")
		{
			obj = new CAnnotationSetOfFaces;

			QDomElement dataset = currentElement.firstChildElement("dataset");

			QString destination = dataset.attribute("destination");

			AddressMap::iterator it = mapaObiektow.find(destination);
			if (it != mapaObiektow.end()) ((CAnnotationSetOfFaces*)obj)->setDest(std::shared_ptr<CMesh>((CMesh*)it->ptr));

			QStringList sl = dataset.text().split(" ");
			for (const auto s : sl)
			{
				((CAnnotationSetOfFaces*)obj)->insert(s.toInt());
			}
		}

		QDomElement properties = currentElement.firstChildElement("properties");

		QString colorTXT = properties.attribute("color", ((CAnnotation*)obj)->m_color.asHexRGBA().c_str());
		QString selcolorTXT = properties.attribute("selcolor", ((CAnnotation*)obj)->m_selcolor.asHexRGBA().c_str());

		((CAnnotation*)obj)->setColor(CRGBA::fromHexRGBA(colorTXT.toStdString()));
		((CAnnotation*)obj)->setSelColor(CRGBA::fromHexRGBA(selcolorTXT.toStdString()));
	}

	if (obj != nullptr)
	{
		QString parentId = currentElement.attribute("parent");

		obj->setLabel(currentElement.attribute("label"));
		obj->setDescr(currentElement.firstChildElement("descr").text());

		mapaObiektow[currentId] = { obj, parentId, path };
	}

	return obj;
}


//#include "ProgressIndicator.h"


void CParserDPVISION::parseF(QString& qline, CMesh::Faces& faces, CMaterial& material, size_t& lbf, size_t& lbti)
{
	int v1, v2, v3;
	int n1, n2, n3;
	int t1, t2, t3;

	QByteArray ba = qline.toUtf8();
	char* ptr = &ba.data()[2];

	char* tst;

	static bool m_bN, m_bT;

	if (lbf==0)
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
	
	faces.emplace_back(v1 - 1, v2 - 1, v3 - 1);
	lbf++;

	//if (bIsFN)
	//{
		//mesh->fnormals().push_back(CFace(f1v - 1, f2v - 1, f3v - 1).getNormal(mesh->vertices()));
	//}

	if (m_bT)
	{
		material.texindex.emplace_back(t1 - 1, t2 - 1, t3 - 1);
		lbti++;
	}
}


void CParserDPVISION::ParseObjMTLFile(QTextStream& in, std::map<QString, CMaterial*>& mats, QuaZip& zip, QString pathInZip)
{
	float alpha = 1.0;

	in.setCodec("UTF-8");

	for (auto& m : mats) delete m.second;
	mats.clear();

	QString currentName = "default";
	CMaterial* currentMaterial = nullptr;

	while (!in.atEnd())
	{
		QString qline = in.readLine();

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
			QByteArray ba = qline.toUtf8();
			char* ptr = &ba.data()[2];

			float r = std::strtof(ptr, &ptr);
			float g = std::strtof(ptr, &ptr);
			float b = std::strtof(ptr, &ptr);
			currentMaterial->FrontColor.ambient.setRGBAf(r, g, b);
		}
		else if (qline.startsWith("Kd", Qt::CaseInsensitive))
		{
			//# Kd = diffuse color
			//Kd 0.550000 0.550000 0.550000
			QByteArray ba = qline.toUtf8();
			char* ptr = &ba.data()[2];

			float r = std::strtof(ptr, &ptr);
			float g = std::strtof(ptr, &ptr);
			float b = std::strtof(ptr, &ptr);
			currentMaterial->FrontColor.diffuse.setRGBAf(r, g, b);
		}
		else if (qline.startsWith("Ks", Qt::CaseInsensitive))
		{
			//# Ks = specular color
			//Ks 0.500000 0.500000 0.500000
			QByteArray ba = qline.toUtf8();
			char* ptr = &ba.data()[2];

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

				QTemporaryFile tmpFile(QDir::tempPath() + "/dpVision.XXXXXX.png");
				
				if (tmpFile.open())
				{
					//tmpFile.setAutoRemove(false);
					//std::cout << tmpFile.fileName().toStdString() << "\n";

					zip.setCurrentFile(pathInZip + ".png");

					if (!zip.hasCurrentFile())
					{
						QStringList pList = pathInZip.split("/", Qt::SkipEmptyParts);

						pList.removeLast();
						pList.push_back(qlist.last());

						QString path = pList.join("/");

						zip.setCurrentFile(path);
					}

					if (zip.hasCurrentFile())
					{
						QuaZipFile file(&zip);

						if (file.open(QIODevice::ReadOnly))
						{
							tmpFile.write(file.readAll());

							file.close();

							currentMaterial->tekstura = new CTexture(tmpFile.fileName());
						}
					}

					tmpFile.close();
				}
			}
		}
		else if (qline.startsWith("map_Ks", Qt::CaseInsensitive))
		{
			//map_Ks KZdeb_1.bmp
		}
		else if (qline.startsWith("d", Qt::CaseInsensitive))
		{
			QByteArray ba = qline.toUtf8();
			char* ptr = &ba.data()[2];

			float alpha = std::strtof(ptr, &ptr);
			currentMaterial->FrontColor.ambient.setAlpha(alpha * 255);
			currentMaterial->FrontColor.diffuse.setAlpha(alpha * 255);

		}
		else if (qline.startsWith("Tr", Qt::CaseInsensitive))
		{
			//# Tr = transparency, some implementations use 'd' others use 'Tr'
			// Tr = 1.0 - d;
			QByteArray ba = qline.toUtf8();
			char* ptr = &ba.data()[2];

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


CObject* CParserDPVISION::parseOBJ(QTextStream& in, QuaZip& zip, QString pathInZip)
{
	in.setCodec("UTF-8");

	CMesh* mesh = new CMesh;
	//CObject* parentObj = new CObject;
	
	QVector<CObject*> vec;

	if (mesh == NULL) return nullptr;

	vec.push_back(mesh);

	size_t lbv = 0, lbf = 0, lbti = 0;
	float nx, ny, nz, ax, ay, az, tcs, tct;

	CVertex vertex;
	CVector3f WNorm;
	CFace face;
	CTIndex tidx;
	CTCoord tcoord;

	int res;

	CPointCloud::Vertices tmpV;
	CPointCloud::Colors tmpC;
	CPointCloud::Normals tmpN;
	CMaterial::TextureCoordinates tmpTC;
	CBoundingBox tmpBB;

	std::map<QString, CMaterial*> mats;

	bool hasGroups = false;

	while (!in.atEnd())
	{
		QString qline = in.readLine();

		if (qline.startsWith("vn", Qt::CaseInsensitive))
		{
			// Normals in (x,y,z) form; normals might not be unit.
			// vn 0.707 0.000 0.707

			QByteArray ba = qline.toUtf8();
			char* ptr = &ba.data()[2];

			nx = std::strtof(ptr, &ptr);
			ny = std::strtof(ptr, &ptr);
			nz = std::strtof(ptr, &ptr);

			//mesh->vnormals().push_back(CVector3f(nx, ny, nz));
			tmpN.push_back(CVector3f(nx, ny, nz));
		}
		else if (qline.startsWith("vt", Qt::CaseInsensitive))
		{
			// Texture coordinates, in (u,v[,w]) coordinates, w is optional.
			// vt 0.500 -1.352 [0.234]

			QByteArray ba = qline.toUtf8();
			char* ptr = &ba.data()[2];

			tcs = std::strtof(ptr, &ptr);
			tct = std::strtof(ptr, &ptr);

			//mesh->getMaterial(0).texcoord.push_back(CTCoord(tcs, tct));
			tmpTC.push_back(CTCoord(tcs, tct));
		}
		else if (qline.startsWith("v", Qt::CaseInsensitive))
		{
			float cr, cg, cb;

			QByteArray ba = qline.toUtf8();
			char* ptr = &ba.data()[2];

			ax = std::strtof(ptr, &ptr);
			ay = std::strtof(ptr, &ptr);
			az = std::strtof(ptr, &ptr);

			char* tst = ptr;
			cr = std::strtof(ptr, &ptr);

			if (ptr > tst)
			{
				cg = std::strtof(ptr, &ptr);
				cb = std::strtof(ptr, NULL);

				//mesh->addVertex(CVertex(ax, ay, az), CRGBA(cr, cg, cb));
				tmpV.push_back(CVertex(ax, ay, az));
				tmpC.push_back(CRGBA(cr, cg, cb));
				lbv++;
			}
			else
			{
				//mesh->addVertex(CVertex(ax, ay, az));
				tmpV.push_back(CVertex(ax, ay, az));
				lbv++;
			}
			tmpBB.expand(CVertex(ax, ay, az));
		}
		else if (qline.startsWith("f", Qt::CaseInsensitive))
		{
			parseF(qline, mesh->faces(), mesh->getMaterial(), lbf, lbti);
		}
		else if (qline.startsWith("mtllib", Qt::CaseInsensitive))
		{
			zip.setCurrentFile(pathInZip+".mtl");
			
			if (!zip.hasCurrentFile())
			{
				QStringList qList = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
				QStringList pList = pathInZip.split("/", Qt::SkipEmptyParts);

				pList.removeLast();
				pList.push_back(qList.last());

				QString path = pList.join("/");

				zip.setCurrentFile(path);
			}

			if (zip.hasCurrentFile())
			{
				QuaZipFile file(&zip);

				if (file.open(QIODevice::ReadOnly))
				{
					QByteArray ba = file.readAll();

					file.close();

					QTextStream mtlStream(ba);

					ParseObjMTLFile(mtlStream, mats, zip, pathInZip);
				}
			}
		}
		else if (qline.startsWith("g", Qt::CaseInsensitive))
		{
			if (hasGroups) vec.push_back(mesh = new CMesh);
			
			hasGroups = true;

			QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

			if (qlist.size() > 1)
			{
				mesh->setLabel(qlist[1]);
			}
		}
		else if (qline.startsWith("usemtl", Qt::CaseInsensitive))
		{
			//usemtl [material name]

			QStringList qlist = qline.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
			if (qlist.size() > 1)
			{
				if (mats.find(qlist[1]) != mats.end())
					mesh->getMaterial(0) = *mats[qlist[1]];
			}
		}
	}

	setChildrenVertices(vec, tmpV, tmpC, tmpN, tmpTC);

	if (vec.size() > 1)
	{
		CModel3D* parentObj = new CModel3D;
		for (const auto& c : vec)
		{
			parentObj->addChild(c);
		}
		parentObj->importChildrenGeometry();

		UI::STATUSBAR::setText("File loading done!");
		return parentObj;
	}
	else
	{
		UI::STATUSBAR::setText("File loading done!");
		return vec.first();
	}
}

void CParserDPVISION::setChildrenVertices(QVector<CObject*> vec, CPointCloud::Vertices& tmpV, CPointCloud::Colors& tmpC, CPointCloud::Normals& tmpN, CMaterial::TextureCoordinates& tmpTC)
{
	for (const auto& c : vec)
	{
		if (c->hasType(CObject::Type::MESH))
		{
			CMesh* m = (CMesh*)c;

			m->vertices() = tmpV;
			m->vcolors() = tmpC;
			m->vnormals() = tmpN;
			m->getMaterial(0).texcoord = tmpTC;

			//m->attachTexture(plikSiatki.path());
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
		//setChildrenVertices((CObject*)c.second, tmpV, tmpC, tmpN, tmpTC);
	}
}
