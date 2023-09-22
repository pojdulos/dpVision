#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "Workspace.h"
#include "MainApplication.h"

#include "AP.h"

#include "AnnotationPoint.h"

ConcretePlugin::ConcretePlugin(void) :m_cutPlane{ NULL }, m_picking{ false }
{
	setName( PLUGIN_NAME );
    CommonDLL::Instance().AddName( PLUGIN_NAME );
}

ConcretePlugin::~ConcretePlugin(void)
{
}

void ConcretePlugin::showMainPluginPanel()
{
	UI::PLUGINPANEL::clear(m_ID);

	int nextPos = 0;

	UI::PLUGINPANEL::addButton(m_ID, L"kolorki", L"kolorki", nextPos++, 0);
	UI::PLUGINPANEL::addButton(m_ID, L"LL", L"LL", nextPos++, 0);
	UI::PLUGINPANEL::addButton(m_ID, L"saveScreenShot", L"screen shot", nextPos++, 0);

	UI::PLUGINPANEL::setEnabled(m_ID, true);
}

void merge(CMesh* dst, CMesh* mesh)
{
	INDEX_TYPE vOffset = dst->vertices().size();
	INDEX_TYPE fOffset = dst->faces().size();

	dst->setSelfVisibility(false);

	for (int i = 0; i < mesh->vertices().size(); i++)
	{
		if (mesh->hasVertexColors())
		{
			if (mesh->hasVertexNormals())
			{
				dst->addVertex(mesh->vertices()[i], mesh->vcolors()[i], mesh->vnormals()[i]);
			}
			else
			{
				dst->addVertex(mesh->vertices()[i], mesh->vcolors()[i]);
			}
		}
		else
		{
			if (mesh->hasVertexNormals())
			{
				dst->addVertex(mesh->vertices()[i], mesh->vnormals()[i]);
			}
			else
			{
				dst->addVertex(mesh->vertices()[i]);
			}
		}
	}

	for (int i = 0; i < mesh->faces().size(); i++)
	{
		INDEX_TYPE a = mesh->faces()[i].A() + vOffset;
		INDEX_TYPE b = mesh->faces()[i].B() + vOffset;
		INDEX_TYPE c = mesh->faces()[i].C() + vOffset;

		CFace f(a, b, c);

		if (mesh->hasFaceColors())
		{
			CRGBA col = mesh->fcolors()[i];

			dst->addFace(f, col);
		}
		else
		{
			dst->addFace(f);
		}
	}

	dst->setSelfVisibility(true);
}

void ConcretePlugin::onLoad()
{
	UI::PLUGINPANEL::create( m_ID, L"testy2" );

	showMainPluginPanel();
}

void ConcretePlugin::onUnload()
{
}

void ConcretePlugin::onButton(std::wstring name)
{
	if (0 == name.compare(L"kolorki"))
	{
		const float phi = (1 + std::sqrt(5.0f)) / 2;

		float vertices[][3] = {
			{1, 1, 1},
			{1, 1, -1},
			{1, -1, 1},
			{1, -1, -1},
			{-1, 1, 1},
			{-1, 1, -1},
			{-1, -1, 1},
			{-1, -1, -1},
			{0, 1 / phi, phi},
			{0, 1 / phi, -phi},
			{0, -1 / phi, phi},
			{0, -1 / phi, -phi},
			{1 / phi, phi, 0},
			{1 / phi, -phi, 0},
			{-1 / phi, phi, 0},
			{-1 / phi, -phi, 0},
			{phi, 0, 1 / phi},
			{phi, 0, -1 / phi},
			{-phi, 0, 1 / phi},
			{-phi, 0, -1 / phi}
		};

		INDEX_TYPE faces5[][5] = {
			{0, 12, 14, 4,8 },
			{0, 16, 17, 1, 12 },
			{0, 8, 10, 2,16 },
			{1,9,5,14,12},
			{1,17,3,11,9},
			{2,13,3,17,16},
			{2,10,6,15,13},
			{3,13,15,7,11},
			{4,18,6,10,8},
			{4,14,5,19,18},
			{5,9,11,7,19},
			{6,18,19,7,15}
		};

		// Tworzenie generatora liczb losowych
		std::random_device rd;
		std::mt19937 gen(rd());

		// Ustalanie zakresu dla liczby losowej
		std::uniform_real_distribution<> dis(0.0, 1.0);


		CMesh* mesh = new CMesh();
		//mesh->addVertex(0, 0, 0);
		//mesh->addVertex(10, 0, 0);
		//mesh->addVertex(0, 10, 0);
		//mesh->addVertex(0, 0, 10);
		//mesh->addFace(2, 1, 0, 1.0f, 0.0f, 0.0f);
		//mesh->addFace(3, 1, 2, 0.0f, 1.0f, 0.0f);
		//mesh->addFace(0, 3, 2, 0.0f, 0.0f, 1.0f);
		//mesh->addFace(0, 1, 3, 1.0f, 1.0f, 0.0f);

		// Dodawanie wierzchołków
		for (int i = 0; i < 20; i++) {
			float r = dis(gen);
			float g = dis(gen);
			float b = dis(gen);

			mesh->addVertex(CVertex(vertices[i][0], vertices[i][1], vertices[i][2]), CRGBA(r, g, b));
		}


		// Dodawanie ścian
		for (int i = 0; i < 12; i++) {
			CRGBA col((float)dis(gen), (float)dis(gen), (float)dis(gen));
			mesh->addFaceX({ faces5[i][0], faces5[i][1], faces5[i][2], faces5[i][3], faces5[i][4] }, col);
		}

		mesh->renderFacesColors(true);

		CModel3D* obj = new CModel3D();
		obj->addChild(mesh);
		obj->importChildrenGeometry();

		for (int i = 0; i < 20; i++) {
			CPoint3d p(vertices[i][0], vertices[i][1], vertices[i][2]);
			CAnnotationPoint* ap = new CAnnotationPoint(p);
			ap->setLabel(QString::number(i));
			AP::OBJECT::addChild(mesh, ap);
		}

		AP::WORKSPACE::addModel(obj, true);
	}
	else if (0 == name.compare(L"merge"))
	{
		;
	}
	else if (0 == name.compare(L"LL"))
	{
		Eigen::VectorXd K3HyperSpot(4);
		K3HyperSpot[2] = 2.0;

		UI::MESSAGEBOX::error(L"Transformink1");
		CMesh* mesh = new CMesh;
		
		Eigen::Matrix4d K3Position( Eigen::Matrix4d::Identity() );

		K3Position(2, 3) = K3HyperSpot[2];

		mesh->addVertex(0, 0, 0);
		mesh->addVertex(1, 0, 0);
		mesh->addVertex(1, 0, 1);
		mesh->addVertex(0, 0, 1);
		mesh->addVertex(0, 1, 0);
		mesh->addVertex(1, 1, 0);
		mesh->addVertex(1, 1, 1);
		mesh->addVertex(0, 1, 1);
		
		mesh->addFace(0, 1, 2);
		mesh->addFace(0, 2, 3);
		mesh->addFace(7, 6, 5);
		mesh->addFace(7, 5, 4);


		//for (int i = 0; i < mesh->vertices().size(); i++)
		//{
		//	//UI::MESSAGEBOX::error(L"Transformink3");       // <- to wstawiłem dla kontroli, działa
		//	mesh->vertices()[i] = mesh->vertices()[i].transformByMatrix(K3Position);  // <- a to i tak nie działa
		//}

		CTransform from;
		CTransform to;
		
		to.fromEigenMatrix4d(K3Position);
		mesh->applyTransformation(from, to);

		CModel3D* obj = new CModel3D();
		obj->addChild(mesh);
		obj->importChildrenGeometry();

		AP::WORKSPACE::addModel(obj, true);
	}
	else if (0 == name.compare(L"saveScreenShot"))
	{
		UI::CAMERA::screenshot("d:\\testScreenShot.png");
	}
}

