#include "StdAfx.h"

#include "ParserPLY.h"
#include "Utilities.h"

#include <fstream>
#include <sstream>

//#include "MainApplication.h"

CParserPLY::CParserPLY()
{
	setDescr("CyberWare ASCII .ply readable for Rapid Form");
	m_exts.insert("ply");
	m_save_exts.insert("ply");

	verticesToRead = 0;
	facesToRead = 0;
	colorFormat = NoColors;
	propertyIsFor = 0;
}

CParserPLY::~CParserPLY(void)
{
}

void CParserPLY::parseVproperty(QString line) {
	QVector<QStringRef> sp = line.splitRef(' ');
	
	if (sp[2].startsWith("alpha")) {
		colorFormat = UcharRGBA;
	}
	else if (sp[2].startsWith("red") || sp[2].startsWith("green") || sp[2].startsWith("blue")) {
		if (colorFormat < UcharRGB) colorFormat = UcharRGB;
	}
}

void CParserPLY::parseFproperty(QString line) {

}

bool CParserPLY::parseHeader(QString line) {

	colorFormat = ColorFormat::NoColors;

	if (line.startsWith("ply"))
		return true;
	else if (line.startsWith("comment"))
		return true;
	else if (line.startsWith("format")) {
		//		uchar fileFormat; //0 = ascii 1.0, 1 = binary_little_endian 1.0
		QVector<QStringRef> sp = line.splitRef(' ');
		if (sp[1].startsWith("ascii")) {
			fileFormat = Ascii;
		}
		else {
			fileFormat = BinLE;
		}
		return true;
	}
	else if (line.startsWith("property"))
	{
		if (propertyIsFor == 1) {
			parseVproperty(line);
		}
		else if (propertyIsFor == 2) {
			parseFproperty(line);
		}
		return true;
	}
	else if (line.startsWith("end_header"))
		return false;
	else if (line.startsWith("element")) {
		QVector<QStringRef> sp = line.splitRef(' ');
		if (sp[1].startsWith("vertex")) {
			verticesToRead = sp[2].toUInt();
			propertyIsFor = 1;
		}
		else if (sp[1].startsWith("face")) {
			facesToRead = sp[2].toUInt();
			propertyIsFor = 2;
		}
		return true;
	}
}

bool CParserPLY::parseVertexTXT(QString line, CVertex& v, CRGBA& c) {
	QVector<QStringRef> sp = line.splitRef(' ');

	v.x = sp[0].toFloat();
	v.y = sp[1].toFloat();
	v.z = sp[2].toFloat();

	if (colorFormat == UcharRGB) {
		c.R( sp[3].toUShort() );
		c.G( sp[4].toUShort() );
		c.B( sp[5].toUShort() );
		c.A( 255 );
	}
	else if (colorFormat == UcharRGBA) {
		c.R(sp[3].toUShort());
		c.G(sp[4].toUShort());
		c.B(sp[5].toUShort());
		c.A(sp[6].toUShort());
	}

	return true;
}

bool CParserPLY::parseFaceTXT(QString line, CFace& f) {
	QVector<QStringRef> sp = line.splitRef(' ');
	//sp[0] - liczba wierzcho³ków = 3 ignorujê
	f.A(sp[1].toUInt());
	f.B(sp[2].toUInt());
	f.C(sp[3].toUInt());

	return true;
}

void CParserPLY::readVerticesBIN(QFile& inputFile, CPointCloud* destPC) {
	while (!inputFile.atEnd() && (verticesToRead > 0)) {
		CVertex v;
		unsigned char rgb[3];
		CRGBA c;

		int res1 = inputFile.read((char*)&v, sizeof(CVertex));

		if (colorFormat == 0) {
			destPC->addVertex(v);
		}
		else if (colorFormat == UcharRGB) {
			inputFile.read((char*)&c, 3 * sizeof(unsigned char));
			destPC->addVertex(v, c);
		}
		else if (colorFormat == UcharRGBA) {
			inputFile.read((char*)&c, 4 * sizeof(unsigned char));
			destPC->addVertex(v, c);
		}

		verticesToRead--;
	}
}


void CParserPLY::readFacesBIN(QFile& inputFile, CMesh* destM) {
	while (!inputFile.atEnd() && (facesToRead > 0)) {
		unsigned char s[1];
		INDEX_TYPE tmp[3];
		CFace f;

		inputFile.read((char*)&s[0], 1);
		inputFile.read((char*)tmp, 3 * sizeof(INDEX_TYPE));

		facesToRead--;

		destM->faces().push_back(CFace(tmp[0], tmp[1], tmp[2]));
	}
}

void CParserPLY::readFacesTXT(QFile& inputFile, CMesh* destM) {
	while (!inputFile.atEnd() && (facesToRead > 0)) {
		CFace f;
		parseFaceTXT(inputFile.readLine(), f);
		facesToRead--;

		destM->faces().push_back(f);
	}
}

void CParserPLY::readVerticesTXT(QFile& inputFile, CPointCloud* destPC) {
	while (!inputFile.atEnd() && (verticesToRead > 0)) {
		CVertex v;
		CRGBA c;
		parseVertexTXT(inputFile.readLine(), v, c);
		verticesToRead--;

		if (colorFormat == 0)
			destPC->addVertex(v);
		else
			destPC->addVertex(v, c);
	}
}

CModel3D* CParserPLY::load(const QString path, bool wait)
{
	QFileInfo fileinfo(path);

	if (fileinfo.exists()) {
		QFile inputFile(path);

		if ( inputFile.open(QIODevice::ReadOnly) )
		{
			//QTextStream in(&inputFile);
			//QDataStream data(&inputFile);
			//data.setByteOrder(QDataStream::ByteOrder::LittleEndian);
			
			while (!inputFile.atEnd() && parseHeader(inputFile.readLine())) {};

			CPointCloud* destPC = new CPointCloud();

			if (fileFormat == Ascii)
			{
				readVerticesTXT(inputFile, destPC);
			}
			else
			{
				readVerticesBIN(inputFile, destPC);
			}

			if (destPC->size() > 0) {
				CModel3D* model = new CModel3D();

				if (facesToRead > 0) {
					CMesh* destM = CMesh::fromPointCloud(*destPC);

					delete destPC;

					if (fileFormat == Ascii) {
						readFacesTXT(inputFile, destM);
					}
					else {
						readFacesBIN(inputFile, destM);
					}



					model->addChild(destM);
				}
				else 
				{
					model->addChild(destPC);
				}

				model->setPath(fileinfo.absoluteFilePath());
				model->setLabel(fileinfo.fileName());

				return model;
			}
		}
		else {
			UI::STATUSBAR::setText("Can't open file.");
		}
	}

	return nullptr;
}


bool CParserPLY::save()
{
	bool result = false;

	CMesh *mesh = (CMesh*) m_model->getChild();

	bool saveAsASCII = ( 0 != UI::MESSAGEBOX::question(L"Save PLY file as Binary or ASCII ?", L"Choose format", L"Binary", L"ASCII") );

	if ((mesh->hasType(CObject::MESH)) || (mesh->hasType(CObject::CLOUD)) || (mesh->hasType(CObject::ORDEREDCLOUD)))
	{
		std::ofstream plik;

		if (saveAsASCII)
		{
			plik.open(plikSiatki.absoluteFilePathA(), std::ios::out);
		}
		else
		{
			plik.open(plikSiatki.absoluteFilePathA(), std::ios::out | std::ios::binary);
		}

		char buf[1000];

		if (plik.is_open())
		{
			plik << "ply" << std::endl;
			
			if (saveAsASCII)
			{
				plik << "format ascii 1.0" << std::endl;
			}
			else
			{
				plik << "format binary_little_endian 1.0" << std::endl;
			}

			plik << "comment Exported by dpVision" << std::endl;
			plik << "element vertex " << std::to_string(mesh->vertices().size()) << std::endl;
			plik << "property float x" << std::endl;
			plik << "property float y" << std::endl;
			plik << "property float z" << std::endl;
			plik << "property uchar red" << std::endl;
			plik << "property uchar green" << std::endl;
			plik << "property uchar blue" << std::endl;
			plik << "property uchar alpha" << std::endl;

			if ( mesh->hasType(CObject::MESH) && mesh->hasFaces() )
			{
				plik << "element face " << std::to_string(mesh->faces().size()) << std::endl;
				plik << "property list uchar int vertex_indices" << std::endl;
			}

			plik << "end_header" << std::endl;

			for (int i = 0; i < mesh->vertices().size(); i++)
			{
				UI::STATUSBAR::printfTimed(500, L"%d points have been saved", i);

				CVertex &v = mesh->vertices()[i];

				if (mesh->vcolors().size() == mesh->vertices().size())
				{
					CRGBA &c = mesh->vcolors()[i];

					if (saveAsASCII)
					{
						sprintf(buf, "%f %f %f %u %u %u %u", v.X(), v.Y(), v.Z(), c.R(), c.G(), c.B(), c.A());
						plik << buf << std::endl;
					}
					else
					{
						plik.write( (char*)&v, sizeof(CVertex));
						plik.write( (char*)&c, 4*sizeof(unsigned char));
						//plik << v.X() << v.Y() << v.Z() << c.R() << c.G() << c.B();
					}
				}
				else
				{
					if (saveAsASCII)
					{
						sprintf(buf, "%f %f %f 192 192 192 255", v.X(), v.Y(), v.Z());
						plik << buf << std::endl;
					}
					else
					{
						CRGBA c((unsigned char)192,192,192);
						plik.write((char*)&v, sizeof(CVertex));
						plik.write((char*)&c, 4 * sizeof(unsigned char));
						//plik << v.X() << v.Y() << v.Z() << 192 << 192 << 192;
					}
				}
			}

			if (mesh->hasType(CObject::MESH) && mesh->hasFaces())
			{
				for ( auto &f : mesh->faces() )
				{
					if (saveAsASCII)
					{
						sprintf(buf, "3 %d %d %d", f.A(), f.B(), f.C());
						plik << buf << std::endl;
					}
					else
					{
						unsigned char c = 3;
						plik.write((char*)&c, 1);
						plik.write((char*)&f, sizeof(CFace));
					}
				}
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

