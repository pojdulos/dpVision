#include "StdAfx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "Workspace.h"
#include "MainApplication.h"

#include "AP.h"
#include "AnnotationPoint.h"
#include "AnnotationPlane.h"
#include "AnnotationVertex.h"
// ĄŚĆĘęńłó
ConcretePlugin::ConcretePlugin(void)
{
	setName(PLUGIN_NAME);
	CommonDLL::Instance().AddName(PLUGIN_NAME);

	//UI::MENU::addUserAction( "Test menu", myAction );
	//UI::MESSAGEBOX::error( L"I'm LOADED" );
	objRuchomy = objNieruchomy = nullptr;

	m_picking = false;
}

ConcretePlugin::~ConcretePlugin(void)
{
	//UI::MESSAGEBOX::error( L"I'm UNLOADED" );
}

void ConcretePlugin::onLoad()
{
	UI::PLUGINPANEL::create(m_ID, QString::fromUtf8("Składanie"));

	showMainPanel();

	m_picking = false;
	bNegNormal = false;
	bRegrCalculated = false;
	ptCnt = 0;

	selPoints.resize(5);
	resNorm.resize(5);
	rotDir = 2;   // default vertical axis: Y
}


void ConcretePlugin::readPointsDefs()
{
	points = {
		{"AAA",new CAnnotationPoint()},
		{"BBB",new CAnnotationPoint()},
		{"CCC",new CAnnotationPoint()},
		{"DDD",new CAnnotationPoint()},
		{"EEE",new CAnnotationPoint()},
		{"FFF",new CAnnotationPoint()},
	};

	for (const QString &key : points.keys())
	{
		points[key]->setLabel(key);
	}
}

void ConcretePlugin::showShellLine(int shellIdx)
{
	UI::PLUGINPANEL::addLabel(m_ID, QString::number(shellIdx).toStdString(), QString::number(shellIdx).toStdString(), gridPosY, 0, 1, 1);
	std::string parentLabel = shells[shellIdx]->getParent()->getLabelA();
	UI::PLUGINPANEL::addLabel(m_ID, "s_"+QString::number(shellIdx).toStdString(), parentLabel + ">" + shells[shellIdx]->getLabelA(), gridPosY++, 1, 1, gridWidth - 1);
}

void ConcretePlugin::showPointLine(QString ptLabel)
{
	if (points.find(ptLabel) != points.end())
	{
		CPoint3d pt = * points[ptLabel];
	
		QString ptStr( QString::number(pt.x) + ", " + QString::number(pt.y) + ", " + QString::number(pt.z) );
		UI::PLUGINPANEL::addLabel(m_ID, ptLabel.toStdString(), ptLabel.toStdString(), gridPosY, 0, 1, 1);
		UI::PLUGINPANEL::addLabel(m_ID, "s_" + ptLabel.toStdString(), ptStr.toStdString(), gridPosY++, 1, 1, gridWidth - 1);
	}
}

void ConcretePlugin::showMainPanel()
{
	UI::PLUGINPANEL::clear(m_ID);

	gridPosX = 0;
	gridPosY = 0;

	UI::PLUGINPANEL::addButton(m_ID, "pickShells", "wybierz siatki", gridPosY++, gridPosX, 1, gridWidth);
	for (int i = 0; i < shells.size(); i++)
		showShellLine(i);

	UI::PLUGINPANEL::addButton(m_ID, "pickPoints", "zdefiniuj punkty", gridPosY++, gridPosX, 1, gridWidth);
	for (const QString &key : points.keys())
		showPointLine(key);

	UI::PLUGINPANEL::addButton(m_ID, "tryICP", "spasuj", gridPosY++, gridPosX, 1, gridWidth);

	UI::PLUGINPANEL::setEnabled(m_ID, true);
}


void ConcretePlugin::showIcpPanel()
{
	UI::PLUGINPANEL::clear(m_ID);

	gridPosY = 0;

	if ((objRuchomy == nullptr) && (objNieruchomy == nullptr))
	{
		UI::PLUGINPANEL::addLabel(m_ID, L"combo1label", L"Obiekt ruchomy:", gridPosY++, 0);
		UI::PLUGINPANEL::addComboBox(m_ID, "combo1", gridPosY++, 0, 1, gridWidth);
		UI::PLUGINPANEL::addLabel(m_ID, L"combo2label", L"Obiekt nieruchomy:", gridPosY++, 0);
		UI::PLUGINPANEL::addComboBox(m_ID, "combo2", gridPosY++, 0, 1, gridWidth);

		NameList items;
		for (int i = 0; i < shells.size(); i++)
		{
			QString itemStr = QString::number(i) + ":" + shells[i]->getParent()->getLabel() + ":" + shells[i]->getLabel();
			items.push_back(itemStr.toStdWString());
		}

		UI::PLUGINPANEL::setComboBoxItems(m_ID, L"combo1", items);
		UI::PLUGINPANEL::setComboBoxItems(m_ID, L"combo2", items);
	}

	UI::PLUGINPANEL::addLabel(m_ID, L"label01", L"Min delta:", gridPosY, 0);
	UI::PLUGINPANEL::addEditBox(m_ID, L"editBoxDelta", L"Min delta", L"0.0001", gridPosY++, 1);
	UI::PLUGINPANEL::addLabel(m_ID, L"label02", L"Max iter:", gridPosY, 0);
	UI::PLUGINPANEL::addEditBox(m_ID, L"editBoxIter", L"Max iter", L"500", gridPosY++, 1);
	//UI::PLUGINPANEL::addLabel(m_ID, L"label02a", L"Kąt:", 31, 0);
	//UI::PLUGINPANEL::addEditBox(m_ID, L"editBoxKat", L"Kąt", L"90", 31, 1);
	//UI::PLUGINPANEL::addButton(m_ID, L"filtracja1", L"Filtracja B na bazie A", 30, 0, 1, 2);
	//UI::PLUGINPANEL::addButton(m_ID, L"parowanie", L"Parowanie A z B", 31, 0, 1, 2);
	//UI::PLUGINPANEL::addButton(m_ID, L"dist_calc", L"Odległości A z B", 32, 0, 1, 2);

	//UI::PLUGINPANEL::addButton(m_ID, L"testICP", L"Test ICP", 34, 0, 1, 2);

	UI::PLUGINPANEL::addLabel(m_ID, L"label03", L"Last delta:", gridPosY, 0);
	UI::PLUGINPANEL::addLabel(m_ID, L"label04", L"???", gridPosY++, 1);
	UI::PLUGINPANEL::addLabel(m_ID, L"label05", L"Last iter:", gridPosY, 0);
	UI::PLUGINPANEL::addLabel(m_ID, L"label06", L"???", gridPosY++, 1);
	UI::PLUGINPANEL::addLabel(m_ID, L"label07", L"Best fit:", gridPosY, 0);
	UI::PLUGINPANEL::addLabel(m_ID, L"label08", L"???", gridPosY++, 1);

	//UI::PLUGINPANEL::addButton(m_ID, L"rot1", L"testRot 1", 40, 0, 1, 2);
	//UI::PLUGINPANEL::addButton(m_ID, L"rot2", L"testRot 2", 41, 0, 1, 2);

	UI::PLUGINPANEL::addButton(m_ID, L"fit", L"spasuj siatki", gridPosY++, 0, 1, gridWidth);
	UI::PLUGINPANEL::addButton(m_ID, L"clearPanelICP", L"nowe spasowanie", gridPosY++, 0, 1, gridWidth);

	UI::PLUGINPANEL::setEnabled(m_ID, true);
}



void ConcretePlugin::onUnload()
{
}

void ConcretePlugin::onRun()
{
}

bool ConcretePlugin::onMousePick(Plugin::PickEvent pickEvent)
{
	// getting handle to the vertex (not neccessary here)
	//CModel3D *model = AP::WORKSPACE::getModel(pickEvent.objId);
	//CMesh *mesh = (CMesh*) model->getChild();
	//CVertex v = mesh->vertices()[pickEvent.vertexId];

	if (m_picking)
	{
		if (pickEvent.facePicked) {
			CAnnotationPoint* pt = new CAnnotationPoint(CPoint3f(pickEvent.x, pickEvent.y, pickEvent.z));
			pt->setLabel(L"Punkt" + std::to_wstring(ptCnt + 1));
			//AP::addAnnotation( objId, new CAnnotationPoint(pt) );
			AP::MODEL::addAnnotation(pickEvent.objId, pt);

			//selPoints.push_back(pt.getPoint());// CPoint3d(pt.X(), pt.Y(), pt.Z()));
			selPoints[ptCnt] = CPoint3d(pickEvent.x, pickEvent.y, pickEvent.z);
			resNorm[ptCnt] = CVector3d(pickEvent.direction.x, pickEvent.direction.y, pickEvent.direction.z);

			//std::wstring lab( L"Picked: " + pt.getLabel() + L" [" + std::to_wstring(pt.X()) + L"," + std::to_wstring(pt.Y()) + L"," + std::to_wstring(pt.Z()) + L"]" );
			std::wstring lab(pt->getLabelW() + L" [" + std::to_wstring(pt->X()) + L"," + std::to_wstring(pt->Y()) + L"," + std::to_wstring(pt->Z()) + L"]");
			UI::PLUGINPANEL::setLabel(m_ID, L"label0" + std::to_wstring(ptCnt + 1), lab);

			if (ptCnt < 4) ptCnt++;
			else ptCnt = 0;

			bRegrCalculated = false;
		}
		else
		{
			CModel3D* obj = AP::WORKSPACE::getCurrentModel();
			CPointCloud* mesh = (CPointCloud*)(obj->getChild());

			CPoint3d origin(pickEvent.origin.x, pickEvent.origin.y, pickEvent.origin.z);
			CVector3d direction(pickEvent.direction.x, pickEvent.direction.y, pickEvent.direction.z);

			//std::vector<size_t> idxs = mesh->getKDtree(CMesh::KDtree::PRESERVE).find_all_in_distance_to_ray(0.5f, origin, direction);
			size_t idx = mesh->getKDtree(CPointCloud::KDtree::PRESERVE).closest_to_ray(origin, direction);

			CAnnotationPoint* pt = new CAnnotationPoint(mesh->vertices()[idx]);
			//pt->setLabel(L"Punkt" + std::to_wstring(ptCnt + 1));
			//AP::addAnnotation( objId, new CAnnotationPoint(pt) );
			AP::MODEL::addAnnotation(pickEvent.objId, pt);

			selPoints[ptCnt] = mesh->vertices()[idx];
			resNorm[ptCnt] = CVector3d(pickEvent.direction.x, pickEvent.direction.y, pickEvent.direction.z);

			//std::wstring lab( L"Picked: " + pt.getLabel() + L" [" + std::to_wstring(pt.X()) + L"," + std::to_wstring(pt.Y()) + L"," + std::to_wstring(pt.Z()) + L"]" );
			std::wstring lab(pt->getLabelW() + L" [" + std::to_wstring(pt->X()) + L"," + std::to_wstring(pt->Y()) + L"," + std::to_wstring(pt->Z()) + L"]");
			UI::PLUGINPANEL::setLabel(m_ID, L"label0" + std::to_wstring(ptCnt + 1), lab);

			if (ptCnt < 4) ptCnt++;
			else ptCnt = 0;

			bRegrCalculated = false;
		}
	}
	//else
	//	ptCnt = 0;
	//	m_picking = false;

	return true; // 'true' oznacza, że obsłużono komunikat i program ma już nie obsługiwać tego kliknięcia
}

double ConcretePlugin::fit(IcpPointToPoint& icp, CPointCloud* meshA, Matrix& R, Matrix& t)
{
	int32_t numT = meshA->vertices().size();

	std::vector<CPoint3d> vT(numT);

	std::copy(meshA->vertices().begin(), meshA->vertices().end(), vT.begin());

	double* T = &vT[0].v[0];

	double residual = icp.fit(T, numT, R, t, -1);

	UI::PLUGINPANEL::setLabel(m_ID, L"label04", std::to_wstring(icp.getLastDelta()));
	UI::PLUGINPANEL::setLabel(m_ID, L"label06", std::to_wstring(icp.getLastIter()));

	return residual;
}


void applyRt(CModel3D* model, Matrix& R, Matrix& t)
{
	model->getTransform().rotation().fromRotationMatrix(R.val);
	model->getTransform().translation() = CVector3d(t.val[0][0], t.val[1][0], t.val[2][0]);
}

void addRt(CModel3D* model, Matrix& R, Matrix& t)
{
	CQuaternion qRot;
	qRot.fromRotationMatrix(R.val);

	model->getTransform().rotation() *= qRot;
	model->getTransform().translation() -= CVector3d(t.val[0][0], t.val[1][0], t.val[2][0]);
}

CPointCloud* decimateCluod(CPointCloud* meshAorg, float initialRadius = 3)
{
	CPointCloud* meshA = new CPointCloud;

	size_t fullSize = meshAorg->vertices().size();
	float radius = initialRadius;

	//if (fullSize > 10000)
	{
		std::set<INDEX_TYPE> odrzucone;

		for (INDEX_TYPE i = 0; i < fullSize; i++)
		{
			if (odrzucone.find(i) == odrzucone.end())
			{
				std::vector<INDEX_TYPE> idxs = meshAorg->getKDtree(CPointCloud::KDtree::PRESERVE).find_all_in_distance_to_pt(radius, meshAorg->vertices()[i]);
				for (auto& idx : idxs)
				{
					odrzucone.insert(idx);
				}

				meshA->vertices().push_back(meshAorg->vertices()[i]);
			}
		}

		if (meshA->vertices().size() > 4) return meshA;
	}
	return NULL;
}

CPointCloud* ConcretePlugin::randDecimate(CPointCloud* d, int newsize)
{
	if (newsize > d->size())
	{
		return d->getCopy();
	}

	std::set<INDEX_TYPE> idxs;

	UI::STATUSBAR::setText("randDecimate");
	UI::PROGRESSBAR::init(0, 2 * newsize, 0);

	while (idxs.size() < newsize)
	{
		int r = ((double)rand() / RAND_MAX) * d->size();

		idxs.insert(r);
		UI::PROGRESSBAR::setValue(idxs.size());
	}
	int progress = newsize;

	CPointCloud* dest = new CPointCloud();

	for (auto i : idxs)
	{
		dest->addVertex(d->vertices()[i]);
		UI::PROGRESSBAR::setValue(progress++);
	}

	UI::PROGRESSBAR::hide();
	UI::STATUSBAR::setText("cloud decimated");
	return dest;
}


void ConcretePlugin::testICP_v1()
{
	UI::PLUGINPANEL::setLabel(m_ID, L"label04", L"???");
	UI::PLUGINPANEL::setLabel(m_ID, L"label06", L"???");

	std::list<int> sel = AP::WORKSPACE::SELECTION::getList();

	if (sel.size() > 1)
	{
		std::list<int>::iterator isel = sel.begin();

		CModel3D* objT = AP::WORKSPACE::getModel(*isel);
		isel++;
		CModel3D* objM = AP::WORKSPACE::getModel(*isel);

		double minDelta = std::stod(UI::PLUGINPANEL::getEditBoxValue(m_ID, L"editBoxDelta"));
		double maxIter = std::stod(UI::PLUGINPANEL::getEditBoxValue(m_ID, L"editBoxIter"));

		CPointCloud* cloudM = (CPointCloud*)(objM->getChild());

		int32_t dim = 3;
		int32_t numM = cloudM->vertices().size();
		std::vector<CPoint3d> vM(numM);

		std::copy(cloudM->vertices().begin(), cloudM->vertices().end(), vM.begin());
		double* M = &vM[0].v[0];

		UI::STATUSBAR::setText("Creating icp");
		//IcpPointToPlane icp(M, numM, dim);
		IcpPointToPoint icp(M, numM, dim);

		icp.setMinDeltaParam(minDelta);
		icp.setMaxIterations(maxIter);

		//==== FITTING ==========================================================

		UI::STATUSBAR::setText("Fitting");

		Matrix RR = Matrix::eye(3);
		Matrix tt = Matrix(3, 1);

		CPointCloud* cloudT = (CPointCloud*)(objT->getChild());
		objT->applyTransform();

		int aStep = std::stoi(UI::PLUGINPANEL::getEditBoxValue(m_ID, L"editBoxKat"));  //90; // krok 90 stopni;
		int nTests = (360 / aStep) * (360 / aStep); //16;

		std::vector<CModel3D*> objTdec(nTests);
		std::vector<CPointCloud*> cloudTdec(nTests);
		std::vector<Matrix> R(nTests);
		std::vector<Matrix> t(nTests);
		std::vector<double> delta(nTests);

		std::vector<int> aX(nTests);
		std::vector<int> aY(nTests);
		std::vector<int> aZ(nTests);

		cloudTdec[0] = decimateCluod(cloudT, 5);

		if (cloudTdec[0] != NULL)
		{
			objTdec[0] = new CModel3D;
			objTdec[0]->addChild(cloudTdec[0]);
			objTdec[0]->setLabel(L"_0");
			AP::WORKSPACE::addModel(objTdec[0]);

			R[0] = Matrix::eye(3);
			t[0] = Matrix(3, 1);

			delta[0] = fit(icp, cloudTdec[0], R[0], t[0]);

			applyRt(objTdec[0], R[0], t[0]);

			double bestDelta = delta[0];
			int bestFit = 0;

			int aaX = aX[0] = 0;
			int aaY = aY[0] = 0;
			int aaZ = aZ[0] = 0;

			CQuaternion rotX;
			CQuaternion rotY;

			for (int i = 1; i < nTests; i++)
			{
				aY[i] = aY[i - 1];
				aX[i] = aX[i - 1] + aStep;
				if (aX[i] >= 360)
				{
					aX[i] = 0;
					aY[i] += aStep;
					if (aY[i] >= 360)
					{
						aY[i] = 0;
					}
				}

				objTdec[i] = new CModel3D;
				cloudTdec[i] = cloudTdec[0]->getCopy();
				objTdec[i]->addChild(cloudTdec[i]);
				objTdec[i]->setLabel(L"_" + std::to_wstring(i));
				AP::WORKSPACE::addModel(objTdec[i]);

				rotX = CQuaternion(deg2rad(aX[i]), CVector3d::XAxis());
				objTdec[i]->getTransform().rotation() = rotX;

				if (aY[i] > 0)
				{
					rotY = CQuaternion(deg2rad(aY[i]), CVector3d::YAxis());

					objTdec[i]->getTransform().setOrigin(CPoint3f(0, 0, 0));
					objTdec[i]->getTransform().rotation().multAndSet(rotY);
				}

				objTdec[i]->applyTransform();

				R[i] = Matrix::eye(3);
				t[i] = Matrix(3, 1);

				delta[i] = fit(icp, cloudTdec[i], R[i], t[i]);

				//delta[i] = icp.getLastDelta();

				applyRt(objTdec[i], R[i], t[i]);

				if (abs(delta[i]) < abs(bestDelta))
				{
					bestDelta = delta[i];
					bestFit = i;
				}
				else if (delta[i] > 1.0)
				{
					AP::WORKSPACE::removeModel(objTdec[i]->id());
				}

				UI::PLUGINPANEL::setLabel(m_ID, L"label08", std::to_wstring(bestFit) + L" (" + std::to_wstring(delta[bestFit]) + L")");
			}

			rotX = CQuaternion(deg2rad(aX[bestFit]), CVector3d::XAxis());
			objT->getTransform().rotation() = rotX;

			if (aY[bestFit] > 0)
			{
				rotY = CQuaternion(deg2rad(aY[bestFit]), CVector3d::YAxis());

				objT->getTransform().setOrigin(CPoint3f(0, 0, 0));
				objT->getTransform().rotation().multAndSet(rotY);
			}

			objT->applyTransform();
			applyRt(objT, R[bestFit], t[bestFit]);
			//addRt(objT, R[bestFit], t[bestFit]);

			RR = R[bestFit];
			tt = t[bestFit];
		}

		//fit( icp, cloudT, RR, tt);
		//applyRt(objT, RR, tt);

		UI::STATUSBAR::setText(L"Gotowe.");
	}
	else
		UI::MESSAGEBOX::information(L"Najpierw wybierz dwa modele: A, B.");
}



CPointCloud * simpleDecimate( CPointCloud *d, int newsize = 100000 )
{
	CPointCloud* dest = new CPointCloud();

	CMesh::Vertices::iterator it;
	int orgsize = d->vertices().size();

	float delta = (float)orgsize / newsize;
	
	for (float fj = 0; fj < (float)orgsize; fj+=delta)
	{
		int j = floor(fj);

		dest->addVertex(d->vertices()[j]);// , d->vcolors()[j] );
	}

	return dest;
}


double ConcretePlugin::fit(IcpPointToPlane& icp, CPointCloud* cloudT, Matrix& R, Matrix& t)
{
	int32_t numT = cloudT->vertices().size();

	std::vector<CPoint3d> vT(numT);

	std::copy(cloudT->vertices().begin(), cloudT->vertices().end(), vT.begin());

	double* T = &vT[0].v[0];

	double residual = icp.fit(T, numT, R, t, -1);

	UI::PLUGINPANEL::setLabel(m_ID, L"label04", std::to_wstring(icp.getLastDelta()));
	UI::PLUGINPANEL::setLabel(m_ID, L"label06", std::to_wstring(icp.getLastIter()));

	return residual;
}



CPointCloud* ConcretePlugin::findNClosest(CPointCloud* ruchoma, CPointCloud* nieruchoma, int limit)
{
	nieruchoma->createKDtree();

	std::multimap<double,int> dobre;

	for (int i=0; i<ruchoma->size(); i++)
	{
		const CVertex &p = ruchoma->vertices()[i];

		int idx = nieruchoma->getKDtree(CPointCloud::KDtree::PRESERVE).closest_to_pt(p);
		double dist = CVector3d(p, nieruchoma->vertices()[idx]).length();

		dobre.insert({ dist,i });
	}

	CPointCloud* result = new CPointCloud;
	int size = (limit > dobre.size()) ? dobre.size() : limit;
	for (int i = 0; i < size; i++)
	{
		int idx = dobre.begin()->second;
		result->addVertex(ruchoma->vertices()[idx]);
		dobre.erase(dobre.begin());
	}

	nieruchoma->removeKDtree();
	return result;
}


void ConcretePlugin::onButton(std::wstring name)
{
	if (0 == name.compare(L"pickShells"))
	{
		CWorkspace::Children modele = AP::WORKSPACE::instance()->children();

		for (auto& m : modele)
		{
			CWorkspace::ChildType* obj = m.second;

			for (auto c : obj->getChildren())
			{
				if (c->hasType(CObject::MESH) && c->isSelected())
				{
					shells.push_back((CMesh*)c);
				}
			}
		}

		showMainPanel();
	}
	else if (0 == name.compare(L"pickPoints"))
	{
		readPointsDefs();

		if (!shells.empty())
		{
			for (const QString &key : points.keys())
			{
				AP::OBJECT::addChild(shells[0], points[key]);
			}
		}
		showMainPanel();
	}
	else if (0 == name.compare(L"tryICP"))
	{
		showIcpPanel();
	}
	else if (0 == name.compare(L"testICP"))
	{
		testICP_v1();
	}
	else if (0 == name.compare(L"rot1"))
	{
		CModel3D* obj = AP::WORKSPACE::getCurrentModel();
		obj->getTransform().setOrigin(CPoint3d(0, 0, 0));
		//obj->getTransform().rotateAroundZ(deg2rad(90));
	}
	else if (0 == name.compare(L"rot2"))
	{
		CModel3D* obj = AP::WORKSPACE::getCurrentModel();
		obj->getTransform().setOrigin(obj->getBoundingBox().getMidpoint());
		//obj->getTransform().rotateAroundZ(deg2rad(90));
	}
	else if (0 == name.compare(L"clearPanelICP"))
	{
		objRuchomy = objNieruchomy = nullptr;
		showIcpPanel();
	}
	else if (0 == name.compare(L"fit"))
	{
		if ((objRuchomy == nullptr) && (objNieruchomy == nullptr))
		{
			QString c1 = QString::fromStdWString(UI::PLUGINPANEL::getComboBoxCurrentItemText(m_ID, L"combo1"));
			QString c2 = QString::fromStdWString(UI::PLUGINPANEL::getComboBoxCurrentItemText(m_ID, L"combo2"));

			if (c1 == c2)
			{
				UI::MESSAGEBOX::information(L"Najpierw wybierz dwa różne modele: A, B.");
				return;
			}

			int id1 = c1.split(":")[0].toInt();
			int id2 = c2.split(":")[0].toInt();

			objRuchomy = new CModel3D;
			objRuchomy->addChild(shells[id1]->getCopy());
			objRuchomy->importChildrenGeometry();
			objRuchomy->setTransform(((CModel3D*)shells[id1]->getParent())->transform());
			objRuchomy->setLabel("ruchomy");
			AP::WORKSPACE::addModel(objRuchomy);

			objNieruchomy = new CModel3D;
			objNieruchomy->addChild(shells[id2]->getCopy());
			objNieruchomy->importChildrenGeometry();
			objNieruchomy->setTransform(((CModel3D*)shells[id2]->getParent())->transform());
			objNieruchomy->setLabel("nieruchomy");
			AP::WORKSPACE::addModel(objNieruchomy);
		}

		UI::PLUGINPANEL::setLabel(m_ID, L"label04", L"???");
		UI::PLUGINPANEL::setLabel(m_ID, L"label06", L"???");


			double minDelta = std::stod(UI::PLUGINPANEL::getEditBoxValue(m_ID, L"editBoxDelta"));
			double maxIter = std::stod(UI::PLUGINPANEL::getEditBoxValue(m_ID, L"editBoxIter"));

			CPointCloud* chmuraNieruchoma = (CPointCloud*)(objNieruchomy->getChild());

			int32_t dim = 3;
			int32_t sizeNieruch = chmuraNieruchoma->vertices().size();
			std::vector<CPoint3d> daneNieruch(sizeNieruch);

			std::copy(chmuraNieruchoma->vertices().begin(), chmuraNieruchoma->vertices().end(), daneNieruch.begin());
			double* ptrNieruch = &daneNieruch[0].v[0];

			UI::STATUSBAR::setText("Creating icp");
			IcpPointToPoint icp(ptrNieruch, sizeNieruch, dim);
			//IcpPointToPlane icp(ptrNieruch, sizeNieruch, dim);

			icp.setMinDeltaParam(minDelta);
			icp.setMaxIterations(maxIter);

			//==== FITTING ==========================================================

			UI::STATUSBAR::setText("Fitting");

			Matrix RR = Matrix::eye(3);
			Matrix tt = Matrix(3, 1);

			CPointCloud* chmuraRuchomaOrg = (CPointCloud*)(objRuchomy->getChild());
			objRuchomy->applyTransform();

			//CPointCloud* chmuraRuchoma = CPointCloud::findNClosest(chmuraRuchomaOrg, chmuraNieruchoma, 1000);
			CPointCloud* chmuraRuchoma = chmuraRuchomaOrg->getCopy();

			//AP::MODEL::addChild(objRuchomy, chmuraRuchoma);

			// double delta = fit(icp, cloudT, RR, tt);


			int32_t sizeRuch = chmuraRuchoma->vertices().size();

			std::vector<CPoint3d> daneRuch(sizeRuch);

			std::copy(chmuraRuchoma->vertices().begin(), chmuraRuchoma->vertices().end(), daneRuch.begin());

			double* ptrRuch = &daneRuch[0].v[0];

			double delta = icp.fit(ptrRuch, sizeRuch, RR, tt, -1);

			UI::PLUGINPANEL::setLabel(m_ID, L"label04", std::to_wstring(icp.getLastDelta()));
			UI::PLUGINPANEL::setLabel(m_ID, L"label06", std::to_wstring(icp.getLastIter()));


			applyRt(objRuchomy, RR, tt);

			//UI::PLUGINPANEL::setLabel(m_ID, L"label08", std::to_wstring(bestFit) + L" (" + std::to_wstring(delta[bestFit]) + L")");

			showIcpPanel();

			UI::STATUSBAR::setText(L"Gotowe.");
	}
	

	UI::updateAllViews();
}