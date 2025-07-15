#include "stdafx.h"

#include "ParserDPV.h"
#include "Utilities.h"

#include "Objects"
#include "AnnotationSetOfVertices.h"
#include "AnnotationSetOfFaces.h"

#include <fstream>

#include "quazip.h"
#include "quazipfile.h"
#include "quazipdir.h"
#include "StatusBarManager.h"

void CParserDPVISION::copyFileToZip(QuaZip& zip, QString pathInZip, QString inFileName)
{
	QuaZipFile zipFile(&zip);

	if (!zipFile.open(QIODevice::WriteOnly, QuaZipNewInfo(pathInZip)))
	{
		std::cout << "nie otwar�o pliku w archiwum zip\n";
		return;
	}

	QFile inFile(inFileName);
	if (!inFile.open(QFile::ReadOnly))
	{
		std::cout << "nie otwar�o pliku wej�ciowego\n";
		return;
	}

	zipFile.write(inFile.readAll());

	inFile.close();
	zipFile.close();
}

void CParserDPVISION::writeByteArrayToZip(QuaZip& zip, QString pathInZip, const QByteArray& ba)
{
	QuaZipFile file(&zip);

	if (!file.open(QIODevice::WriteOnly, QuaZipNewInfo(pathInZip)))
	{
		std::cout << "nie otwarto pliku " << pathInZip.toStdString().c_str() << "\n";
		return;
	}

	file.write(ba);

	file.close();

	if (file.getZipError() != 0)
	{
		std::cout << "zipFile.getZipError != 0\n";
		return;
	}
}




void CParserDPVISION::saveMesh(QTextStream& objStream, std::shared_ptr<CObject> src, size_t& vStart)
{
	int vCnt = 0;

	if (src->hasType(CObject::Type::MESH) || src->hasType(CObject::Type::CLOUD) || src->hasType(CObject::Type::ORDEREDCLOUD))
	{
		auto cloud = std::static_pointer_cast<CPointCloud>(src);

		if (cloud->hasVertexColors())
		{
			CMesh::Colors::iterator itcol = cloud->vcolors().begin();
			for (CMesh::Vertices::iterator itv = cloud->vertices().begin(); itv != cloud->vertices().end(); itv++)
			{
				StatusBarManager::printfTimed(500, QString("Saving vertices: %1").arg(vCnt++));

				objStream << "v " << itv->X() << " "
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
			for (CMesh::Vertices::iterator itv = cloud->vertices().begin(); itv != cloud->vertices().end(); itv++)
			{
				StatusBarManager::printfTimed(500, QString("Saving vertices: %1").arg(vCnt++));

				objStream << "v " << itv->X() << " "
					<< itv->Y() << " "
					<< itv->Z() << "\n";
			}
		}

		if (cloud->hasVertexNormals())
		{
			int vnCnt = 0;
			for (CMesh::Normals::iterator itv = cloud->vnormals().begin(); itv != cloud->vnormals().end(); itv++)
			{
				StatusBarManager::printfTimed(500, QString("Saving normals : %1").arg(vnCnt++));

				objStream << "vn " << itv->X() << " "
					<< itv->Y() << " "
					<< itv->Z() << "\n";
			}
		}


		if (src->hasType(CObject::Type::MESH))
		{
			auto mesh = std::static_pointer_cast<CMesh>(src);

			for (CMesh::Materials::iterator im = mesh->materials().begin(); im != mesh->materials().end(); im++)
			{
				if (!(*im)->texcoord.empty())
				{
					for (CMaterial::TextureCoordinates::iterator itc = (*im)->texcoord.begin(); itc != (*im)->texcoord.end(); itc++)
					{
						objStream << "vt " << (*itc)[0] << " "
							<< (*itc)[1] << "\n";
					}
				}
			}

			CMesh::Faces::iterator itf = mesh->faces().begin();

			for (CMesh::Materials::iterator im = mesh->materials().begin(); im != mesh->materials().end(); im++)
			{
				for (CMaterial::TextureIndexes::iterator iti = (*im)->texindex.begin(); iti != (*im)->texindex.end(); iti++)
				{
					objStream << "f " << vStart + itf->A() << "/" << vStart + iti->a << " "
						<< vStart + itf->B() << "/" << vStart + iti->b << " "
						<< vStart + itf->C() << "/" << vStart + iti->c << "\n";

					itf++;
				}
			}

			while (itf != mesh->faces().end())
			{
				objStream << "f " << vStart + itf->A() << " "
					<< vStart + itf->B() << " "
					<< vStart + itf->C() << "\n";
				itf++;
			}
		}
	}

	objStream << Qt::flush;

	vStart += vCnt;
}


void CParserDPVISION::saveMTL(QuaZip& zip, CMaterial& material, QString path, QString matName)
{
	QByteArray mtlBA;

	QTextStream mtlStream(&mtlBA);
	mtlStream.setCodec("UTF-8");

	
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

		mtlStream << "map_Ka " << matName + ".png\n";// +orgTexFile.completeSuffix() << "\n";
		mtlStream << "map_Kd " << matName + ".png\n";// + orgTexFile.completeSuffix() << "\n";
		mtlStream << "map_Ks " << matName + ".png\n";// + orgTexFile.completeSuffix() << "\n";
	}

	mtlStream << Qt::flush;

	writeByteArrayToZip(zip, path + matName + ".mtl", mtlBA);

	if (material.hasTexture())
	{
		QByteArray ba;
		QDataStream stream(ba);

		QTemporaryFile tmpFile;
		tmpFile.open();
		
		std::cout << tmpFile.fileName().toStdString() << Qt::endl;

		material.tekstura->toImage().save(&tmpFile, "png");
		
		copyFileToZip(zip, path + matName + ".png", tmpFile.fileName());
	}

}

void CVolTKtoZIP(CVolTK* volTK, QuaZip& zip, QString pathInZip)
{
	int layerSize = volTK->kostka.m_cols * volTK->kostka.m_rows * sizeof(NowaKostka1::Type);

	QuaZipFile file(&zip);

	if (file.open(QIODevice::OpenModeFlag::WriteOnly, QuaZipNewInfo(pathInZip)))
	{
		UI::PROGRESSBAR::init(0, volTK->kostka.m_lays, 0);
		UI::PROGRESSBAR::setText("saving volumetric data to zip file");
		for (int l = 0; l < volTK->kostka.m_lays; l++)
		{
			file.write(QByteArray::fromRawData((const char*)volTK->kostka.layer(l), layerSize));
			UI::PROGRESSBAR::setValue(l);
		}
		UI::PROGRESSBAR::hide();

		file.close();

		if (file.getZipError() != 0)
		{
			std::cout << "zipFile.getZipError != 0\n";
			return;
		}
	}
	else
	{
		std::cout << "nie otwarto pliku " << pathInZip.toStdString().c_str() << "\n";
		return;
	}
}

void CParserDPVISION::saveObject(QuaZip& zip, std::shared_ptr<CBaseObject> obj, QString path)
{
	if (obj == nullptr) return;

	QString objBaseName = IdToString(obj->id());

	if (obj->hasCategory(CBaseObject::Category::OBJECT))
	{
		if (obj->hasType(CObject::MESH) || obj->hasType(CObject::CLOUD) || obj->hasType(CObject::ORDEREDCLOUD))
		{
			QByteArray objBA;

			QTextStream objStream(&objBA);
			objStream.setCodec("UTF-8");
			objStream.setRealNumberNotation(QTextStream::FixedNotation);
			objStream.setRealNumberPrecision(6);

			objStream
				<< "mtllib " << objBaseName + ".mtl"
				<< "\n\ng " << objBaseName
				<< "\nusemtl " << objBaseName
				<< "\ns\n\n";

			size_t vStart = 1;

			saveMesh(objStream, std::static_pointer_cast<CObject>(obj), vStart);

			writeByteArrayToZip(zip, path + objBaseName + ".obj", objBA);

			saveMTL(zip, std::static_pointer_cast<CPointCloud>(obj)->getMaterial(), path, objBaseName);
		}
		else if (obj->hasType(CObject::VOLTK))
		{
			CVolTKtoZIP((CVolTK*)obj.get(), zip, path + objBaseName + ".raw");
		}


		for (const auto& c : std::static_pointer_cast<CObject>(obj)->children())
		{
			saveObject(zip, c.second, path + objBaseName + "/");
		}
	}
}

void CParserDPVISION::createXmlNodeObject(std::shared_ptr<CObject> obj, QDomElement& root, QDomDocument& docu, QString path)
{
	QDomElement child = docu.createElement("object");

	child.setAttribute("id", IdToString(obj->id()));
	child.setAttribute("class", "object");

	CBaseObject* parent = obj->getParent();
	if (parent != nullptr)
		child.setAttribute("parent", IdToString(parent->id()));
	else
		child.setAttribute("parent", "");

	child.setAttribute("label", obj->getLabel());
	
	if (!obj->getDescr().isEmpty()) {
		QDomNode description = docu.createElement("descr");
		description.appendChild(docu.createTextNode(obj->getDescr()));
		child.appendChild(description);
	}

	child.setAttribute("path", path);

	if (obj->hasType(CObject::Type::MODEL))
	{
		child.setAttribute("type", "transformation");

		QDomNode transform = docu.createElement("matrix");
		transform.appendChild(docu.createTextNode(std::static_pointer_cast<CModel3D>(obj)->transform().toString()));
		child.appendChild(transform);
	}
	else if ((obj->hasType(CObject::Type::MESH)) || (obj->hasType(CObject::Type::CLOUD)))
	{
		child.setAttribute("type", "mesh");

		QDomElement file = docu.createElement("file");
		child.appendChild(file);

		file.setAttribute("format", "obj");
		file.setAttribute("data", IdToString(obj->id()) + ".obj");

		CMaterial& mat = std::static_pointer_cast<CMesh>(obj)->getMaterial();
		file.setAttribute("material", IdToString(obj->id()) + ".mtl");

		if (mat.hasTexture())
		{
			file.setAttribute("texture", IdToString(obj->id()) + ".png");
			//file.setAttribute("texture", IdToString(obj->id()) + "." + QFileInfo(mat.TexInfo).suffix());
		}
	}
	else if (obj->hasType(CObject::Type::VOLTK))
	{
		std::shared_ptr<CVolTK> volTK = std::static_pointer_cast<CVolTK>(obj);

		child.setAttribute("type", "volumetric");
		child.setAttribute("layers", (qulonglong) volTK->kostka.m_lays);
		child.setAttribute("rows", (qulonglong) volTK->kostka.m_rows);
		child.setAttribute("columns", (qulonglong) volTK->kostka.m_cols);
		QDomElement voxel = docu.createElement("voxelSize");
		voxel.setAttribute("x", volTK->getVoxelSize().x);
		voxel.setAttribute("y", volTK->getVoxelSize().y);
		voxel.setAttribute("z", volTK->getVoxelSize().z);
		child.appendChild(voxel);

		QDomElement file = docu.createElement("file");
		file.setAttribute("format", "raw");
		file.setAttribute("data", IdToString(volTK->id()) + ".raw");
		child.appendChild(file);
	}

	root.appendChild(child);

	for (const auto& c : obj->children())
	{
		if (c.second->hasCategory(CBaseObject::Category::OBJECT))
		{
			createXmlNodeObject(std::static_pointer_cast<CObject>(c.second), root, docu, path + IdToString(obj->id()) + "/");
		}
	}
}


void CParserDPVISION::createXmlNodeAnnotation(std::shared_ptr<CAnnotation> obj, QDomElement& root, QDomDocument& docu)
{
	QDomElement child = docu.createElement("object");

	child.setAttribute("class", "annotation");
	child.setAttribute("id", IdToString(obj->id()));
	child.setAttribute("parent", IdToString(obj->getParent()->id()));
	child.setAttribute("label", obj->getLabel());

	if (obj->hasType(CAnnotation::Type::SETOFVERTICES))
	{
		child.setAttribute("type", "setOfVertices");

		auto an = std::dynamic_pointer_cast<CAnnotationSetOfVertices>(obj);

		QDomElement dataset = docu.createElement("dataset");
		child.appendChild(dataset);

		dataset.setAttribute("destination", IdToString(an->m_cloud->id()));
		dataset.setAttribute("pointSize", an->m_pointSize);

		dataset.appendChild(docu.createTextNode(an->toString()));
	}
	else if (obj->hasType(CAnnotation::Type::SETOFFACES))
	{
		child.setAttribute("type", "setOfFaces");

		auto an = std::dynamic_pointer_cast<CAnnotationSetOfFaces>(obj);

		QDomElement dataset = docu.createElement("dataset");
		child.appendChild(dataset);

		dataset.setAttribute("destination", IdToString(an->dstMesh()->id()));

		dataset.appendChild(docu.createTextNode(an->toString()));
	}

	QDomElement properties = docu.createElement("properties");
	properties.setAttribute("color", obj->m_color.asHexRGBA().c_str());
	properties.setAttribute("selcolor", obj->m_selcolor.asHexRGBA().c_str());
	child.appendChild(properties);

	root.appendChild(child);

	for (const auto& a : obj->annotations())
	{
		createXmlNodeAnnotation(a.second, root, docu);
	}
}


void CParserDPVISION::createAnnotationsXml(std::shared_ptr<CObject> obj, QDomElement& root, QDomDocument& docu)
{
	for (const auto& a : obj->annotations())
	{
		createXmlNodeAnnotation(a.second, root, docu);
	}

	for (const auto& c : obj->children())
	{
		if (c.second->hasCategory(CBaseObject::Category::OBJECT))
		{
			createAnnotationsXml(std::static_pointer_cast<CObject>(c.second), root, docu);
		}
	}
}

void CParserDPVISION::createStructureXml(QByteArray& ba, QVector<std::shared_ptr<CBaseObject>> vobj)
{
	QDomDocument doc;

	QDomElement wksp = doc.createElement("workspace");
	doc.appendChild(wksp);

	wksp.setAttribute("mode", "current");

	for (const auto& obj : vobj)
	{
		createXmlNodeObject(std::static_pointer_cast<CObject>(obj), wksp, doc, "");
	}

	for (const auto& obj : vobj)
	{
		createAnnotationsXml(std::static_pointer_cast<CObject>(obj), wksp, doc);
	}

	wksp.setAttribute("count", wksp.childNodes().size());

	QTextStream stream(&ba);
	stream.setCodec("UTF-8");

	stream << doc.toString();
	stream << Qt::flush;
}
