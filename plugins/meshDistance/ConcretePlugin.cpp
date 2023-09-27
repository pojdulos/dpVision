#include "StdAfx.h"
#include "CommonDLL.h"
#include "ConcretePlugin.h"

#include "Wektor3D.h"
#include "Workspace.h"
#include "MainApplication.h"

#include "MeshDistance.h"
#include "Stat.h"

#include "AP.h"

#include <string>
#include <fstream>


ConcretePlugin::ConcretePlugin(void)
{
	setName( PLUGIN_NAME );
    CommonDLL::Instance().AddName( PLUGIN_NAME );

	//UI::MENU::addUserAction( "Test menu", myAction );
	//UI::MESSAGEBOX::error( L"I'm LOADED" );

	m_picking = false;

}

ConcretePlugin::~ConcretePlugin(void)
{
	//UI::MESSAGEBOX::error( L"I'm UNLOADED" );
}


void ConcretePlugin::onLoad()
{
	UI::PLUGINPANEL::create( m_ID, L"MeshDistance" );

	//UI::PLUGINPANEL::addButton(m_ID, L"Szyna2_start", L"spline", 5, 0 );

	UI::PLUGINPANEL::addButton(m_ID, L"MeshDistanceStart", L"Mesh Distance", 7, 0);

	//UI::PLUGINPANEL::addButton(m_ID, L"Error_test", L"błedy - test", 7, 0);
	//UI::PLUGINPANEL::addButton(m_ID, L"Error_ART", L"błedy do ART", 80, 0);
	//UI::PLUGINPANEL::addButton(m_ID, L"Error_ART2", L"błedy do ART (2019)", 82, 0);
	//UI::PLUGINPANEL::addButton(m_ID, L"KD_test", L"test KD", 9, 0);
}

void ConcretePlugin::onUnload()
{
	delete symulator;
}


void ConcretePlugin::MarchingTetrahedron(void)
{
}

#include "AnnotationPoint.h"
//#include "AnnotationVPoint.h"

bool ConcretePlugin::onMousePick(Plugin::PickEvent pickEvent)
{
	return true;
}

void szybki_test()
{
	CTest1<float> object1;
	CTest2 object2;
	CTest3 object3;

	object1.metodaA();
	object1.metodaB();
	CTest2::metoda2();
	object3.metoda3();

	UI::STATUSBAR::printf("T1: %d, T2: %d, T3: %d, CTriple<float>: %d, Vertex: %d", sizeof( object1), sizeof( object2), sizeof( object3), sizeof( CTriple<float>), sizeof( CVertex));
}



//void writeStats(std::vector<double>& difV, double ff, double r, std::wstring prefix = L"")
//{
//	std::ofstream plik2(L"zest_" + prefix + L".csv", std::ios::out | std::ios::app);
//
//	std::ofstream plik(L"stat_" + prefix + L".txt", std::ios::out);
//
//	for (auto& f : difV) { f = f < 0 ? -f : f; }
//
//	double errmax = *std::max_element(difV.begin(), difV.end());
//	double errmin = *std::min_element(difV.begin(), difV.end());
//
//	double q1 = CStat::quantile(difV, 0.25);
//	double q2 = CStat::quantile(difV, 0.5);
//	double q3 = CStat::quantile(difV, 0.75);
//
//	plik << std::endl << "# Minimalny błąd" << std::endl << "errmin = " << errmin << std::endl;
//
//	plik << std::endl << "# 1 Kwartyl" << std::endl << "q1 = " << q1 << std::endl;
//	plik << std::endl << "# Mediana" << std::endl << "q2 = " << q2 << std::endl;
//	plik << std::endl << "# 3 Kwartyl" << std::endl << "q3 = " << q3 << std::endl;
//
//	plik << std::endl << "# Maksymalny błąd" << std::endl << "errmax = " << errmax << std::endl;
//
//	//plik2 << ff << '\t' << r << '\t' << errmin << '\t' << q1 << '\t' << q2 << '\t' << q3 << '\t' << errmax << std::endl;
//	plik2 << q1 << std::endl << errmin << std::endl << q2 << std::endl << q3 << std::endl << errmax << std::endl;
//
//	//std::vector<double> test = CStat::genData(CStat::DistType::Normal, difV.size(), 0.0, 0.003);
//	plik << std::endl << "# Test Kolmogorova-Smirnova" << std::endl << "ksTest = " << CStat::KolmogorovSmirnovTest(difV) << std::endl;
//
//	plik << std::endl << "# Percentyl 2.5%" << std::endl << "p0250 = " << CStat::quantile(difV, 0.025) << std::endl;
//	plik << std::endl << "# Percentyl 97.56%" << std::endl << "p9756 = " << CStat::quantile(difV, 0.9756) << std::endl;
//
//	plik << "set arrow from -q1, (GPVAL_DATA_Y_MIN) to -q1, (GPVAL_DATA_Y_MAX) front nohead" << std::endl;
//	plik << "set arrow from q1, (GPVAL_DATA_Y_MIN) to q1, (GPVAL_DATA_Y_MAX) front nohead" << std::endl;
//
//	plik << "set arrow from -q2, (GPVAL_DATA_Y_MIN) to -q2, (GPVAL_DATA_Y_MAX) lc 'blue' front nohead" << std::endl;
//	plik << "set arrow from q2, (GPVAL_DATA_Y_MIN) to q2, (GPVAL_DATA_Y_MAX) lc 'blue' front nohead" << std::endl;
//
//	plik << "set arrow from -q3, (GPVAL_DATA_Y_MIN) to -q3, (GPVAL_DATA_Y_MAX) front nohead" << std::endl;
//	plik << "set arrow from q3, (GPVAL_DATA_Y_MIN) to q3, (GPVAL_DATA_Y_MAX) front nohead" << std::endl;
//
//	plik << "replot" << std::endl;
//
//	plik2.close();
//	plik.close();
//}


std::string outDir = "g:/praca/output/";
std::wstring outDirW = L"g:/praca/output/";

void writeStats(std::vector<double>& errs, std::string prefix = "", double minLimit=-DBL_MAX, double maxLimit=DBL_MAX)
{
	

	std::ofstream output_file(outDir + "errs_" + prefix + ".txt");
	std::ostream_iterator<double> output_iterator(output_file, "\n");
	std::copy(errs.begin(), errs.end(), output_iterator);
	output_file.close();

	std::vector<double> difV;
	
	for (auto& f : errs)
	{
		if ((f>=minLimit) && (f<=maxLimit))
			difV.push_back(f);
	}


	std::wofstream plik2(outDir + "zest_" + prefix + ".csv", std::ios::out | std::ios::app);

	std::wofstream plik(outDir + "stat_" + prefix + ".txt", std::ios::out);

	double errmax = *std::max_element(difV.begin(), difV.end());
	double errmin = *std::min_element(difV.begin(), difV.end());
	
	double meanError = CStat::MeanError(difV);
	
	double MSE = CStat::MeanSquareError(difV);
	double RMSE = sqrt(MSE);

	double stdDev = CStat::standardDeviation(difV, meanError);

	plik << std::endl << "# Minimalna odleglosc" << std::endl << "minDistance = " << errmin << std::endl;
	plik << std::endl << "# Sredna odleglosc" << std::endl << "avgDistance = " << meanError << std::endl;
	plik << std::endl << "# Maksymalna odleglosc" << std::endl << "maxDistance = " << errmax << std::endl;

	plik << std::endl << "# MSE" << std::endl << "mse = " << MSE << std::endl;
	plik << std::endl << "# RMSE" << std::endl << "rmse = " << RMSE << std::endl;

	plik << std::endl << "# STD DEV" << std::endl << "stdev = " << stdDev << std::endl;


	double q1 = CStat::quantile(difV, 0.25);
	double q2 = CStat::quantile(difV, 0.5);
	double q3 = CStat::quantile(difV, 0.75);

	plik << std::endl << "# 1 Kwartyl" << std::endl << "q1 = " << q1 << std::endl;
	plik << std::endl << "# Mediana" << std::endl << "q2 = " << q2 << std::endl;
	plik << std::endl << "# 3 Kwartyl" << std::endl << "q3 = " << q3 << std::endl;


	//plik2 << ff << '\t' << r << '\t' << errmin << '\t' << q1 << '\t' << q2 << '\t' << q3 << '\t' << errmax << std::endl;
	plik2 << q1 << std::endl << errmin << std::endl << q2 << std::endl << q3 << std::endl << errmax << std::endl;

	//std::vector<double> test = CStat::genData(CStat::DistType::Normal, difV.size(), 0.0, 0.003);
	plik << std::endl << "# Test Kolmogorova-Smirnova" << std::endl << "ksTest = " << CStat::KolmogorovSmirnovTest(difV) << std::endl;

	plik << std::endl << "# Percentyl 2.5%" << std::endl << "p0250 = " << CStat::quantile(difV, 0.025) << std::endl;
	plik << std::endl << "# Percentyl 97.56%" << std::endl << "p9756 = " << CStat::quantile(difV, 0.9756) << std::endl;

	//plik << "set arrow from -q1, (GPVAL_DATA_Y_MIN) to -q1, (GPVAL_DATA_Y_MAX) front nohead" << std::endl;
	plik << "set arrow from q1, (GPVAL_DATA_Y_MIN) to q1, (GPVAL_DATA_Y_MAX) front nohead" << std::endl;

	plik << "set arrow from avgDist, (GPVAL_DATA_Y_MIN) to avgDist, (GPVAL_DATA_Y_MAX) lc 'red' front nohead" << std::endl;

	//plik << "set arrow from -q2, (GPVAL_DATA_Y_MIN) to -q2, (GPVAL_DATA_Y_MAX) lc 'blue' front nohead" << std::endl;
	plik << "set arrow from q2, (GPVAL_DATA_Y_MIN) to q2, (GPVAL_DATA_Y_MAX) lc 'blue' front nohead" << std::endl;

	//plik << "set arrow from -q3, (GPVAL_DATA_Y_MIN) to -q3, (GPVAL_DATA_Y_MAX) front nohead" << std::endl;
	plik << "set arrow from q3, (GPVAL_DATA_Y_MIN) to q3, (GPVAL_DATA_Y_MAX) front nohead" << std::endl;

	plik << "replot" << std::endl;

	plik2.close();
	plik.close();
}



#include "AnnotationBezier.h"
#include "Histogram.h"

#include "./KDTree.h"



void ConcretePlugin::onButton( std::wstring name )
{
	if (0 == name.compare(L"MeshDistanceStart"))
	{
		std::vector<std::wstring> dane = { L"v-f", L"vNf", L"v-v" };
		std::vector<std::wstring> dane2 = { L"wart. bezwzgl.", L"ze znakiem" };

		UI::PLUGINPANEL::removeWidget(m_ID, L"MeshDistanceStart");
		//	UI::PLUGINPANEL::removeWidget(m_ID, L"label01");
		UI::PLUGINPANEL::addLabel(m_ID, L"algorytmL", L"Algorytm: ", 10, 0);
		UI::PLUGINPANEL::addComboBox(m_ID, L"algorytmCombo", 10, 1);
		UI::PLUGINPANEL::setComboBoxItems(m_ID, L"algorytmCombo", dane);

		UI::PLUGINPANEL::addLabel(m_ID, L"rodzajL", L"błąd: ", 20, 0);
		UI::PLUGINPANEL::addComboBox(m_ID, L"rodzajCombo", 20, 1);
		UI::PLUGINPANEL::setComboBoxItems(m_ID, L"rodzajCombo", dane2);

		UI::PLUGINPANEL::addButton(m_ID, L"MeshDistance", L"przelicz", 40, 0, 1, 2);
		
	}
	else if (0 == name.compare(L"MeshDistance"))
	{
		std::list<int> sel = AP::WORKSPACE::SELECTION::getList();

		if (sel.size() < 2) return;

		std::list<int>::iterator is = sel.begin();

		CModel3D * obj1 = AP::WORKSPACE::getModel(*is);
		int id1 = obj1->id();

		is++;
		CModel3D * obj2 = AP::WORKSPACE::getModel(*is);
		int id2 = obj2->id();

		int levels = 128;

		//obj1->getTransform().translate(CVector3d(0.0, 0.0, 2.0));
		//obj1->applyTransform(CTransform());

		std::wstring alg = UI::PLUGINPANEL::getComboBoxCurrentItemText(m_ID, L"algorytmCombo");

		bool wartBezwzgl = (0 == UI::PLUGINPANEL::getComboBoxCurrentItemText(m_ID, L"rodzajCombo").compare(L"wart. bezwzgl."));

		CHistogram *histo1;

		if (0 == alg.compare(L"v-f"))
		{
			std::vector<double> errs = symulator->meshDistance_Vertex2Face(obj1, obj2, false, wartBezwzgl);
			histo1 = new CHistogram(obj1, errs, levels);
			histo1->setLabel(L"histogram (v-f)");

			double upLimit = 2.0;

			histo1->setLimits(0.0, upLimit);
			writeStats(errs, obj1->getLabelA() + "_v-f_" + std::to_string(upLimit), 0.0, upLimit);
			histo1->save(outDirW, L"histo_" + obj1->getLabelW() + L"_v-f_" + std::to_wstring(upLimit));
		}
		else if (0 == alg.compare(L"vNf"))
		{
			std::vector<double> errs = symulator->meshDistance_withNormal(obj1, obj2, DBL_MAX, true, wartBezwzgl);
			histo1 = new CHistogram(obj1, errs, levels);
			histo1->setLabel(L"histogram (vNf)");



		}
		else if (0 == alg.compare(L"v-v"))
		{
			std::vector<double> errs = symulator->meshDistance_Vertex2VertexNEW(obj1, obj2);

			double upLimit = 2.0;

			histo1 = new CHistogram(obj1, errs, levels);
			histo1->setLabel(L"histogram (v-v)");
			histo1->setLimits(0.0, upLimit);
			writeStats(errs, obj1->getLabelA() + "_" + std::to_string(upLimit), 0.0, upLimit);
			histo1->save(outDirW, L"histo_"+ obj1->getLabelW() + L"_" + std::to_wstring(upLimit));
		}
		else
		{
			return;
		}
		
		histo1->setDest((CMesh*)obj1->getChild());

		AP::MODEL::addAnnotation(obj1, histo1);

		UI::updateAllViews();
	}
	else if (0 == name.compare(L"Error_test"))
	{
		//CModel3D * obj1 = AP::WORKSPACE::getModel(1000000000);
		//CModel3D * obj2 = AP::WORKSPACE::getModel(1001000000);

		//std::wstring dir(L"d://darekp//prog//dpVisionProject//test_AG//");
		std::wstring dir(L"d://darekp//prog//dpVisionProject//test_Photogram//");
		int levels = 64;

		//CModel3D * obj1 = AP::MODEL::load(dir + L"powierzchnia_zgryzu.obj");
		//CModel3D * obj1 = AP::MODEL::load(dir + L"mesh_high_med_aligned.obj");
		//CModel3D * obj1 = AP::MODEL::load(dir + L"model_02_06g1.obj");
		//CModel3D * obj1 = AP::MODEL::load(dir + L"model_H-H_02_06g2.obj");
		CModel3D * obj1 = AP::MODEL::load(dir + L"inokulkif3_H-MmeshG.obj");

		AP::WORKSPACE::addModel(obj1); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id1 = obj1->id();

//		CModel3D * obj2 = AP::MODEL::load(dir + L"int_AG.obj");
		//CModel3D * obj2 = AP::MODEL::load(dir + L"minolta.obj");
		//CModel3D * obj2 = AP::MODEL::load(dir + L"zuchwa_orginalna_g1.obj");
		CModel3D * obj2 = AP::MODEL::load(dir + L"samekulki.obj");

		AP::WORKSPACE::addModel(obj2); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id2 = obj2->id();
		
		//obj1->getTransform().translate(CVector3d(0.0, 0.0, 2.0));
		//obj1->applyTransform(CTransform());


		CHistogram *histo1 = new CHistogram(symulator->meshDistance_withNormal(obj1, obj2, DBL_MAX, true, true), levels);
		
		histo1->setLabel(L"histogram (vNf)");
		AP::MODEL::addAnnotation(obj1, histo1);
		//histo1->setVisible(true);

		double wariancja;
		double globalError = symulator->calcGlobalError( (CModel3D*)histo1->getParent(), histo1->getData(), wariancja);

		//std::vector<CHistogram*> v({ histo1 });
		//std::pair<double, double> limits = CHistogram::unifyLimits(v, std::pair<double, double>(0.0, 0.85));

		FILE *plik = fopen("odleglosci.txt", "w");

		fprintf(plik, "\nMetoda: vNf\n");
		//fprintf(plik, "skipped: %d points.\n\n", liczInf);
		fprintf(plik, "min error: %lf\n", histo1->getDataMin());
		fprintf(plik, "max error: %lf\n", histo1->getDataMax());
		fprintf(plik, "global error: %lf\n", globalError);
		fprintf(plik, "wariancja: %lf\n", wariancja);
		fprintf(plik, "odchylenie std.: %lf\n", sqrt(wariancja));

		fprintf(plik, "histogram:\n");
		for (int i = 0; i < histo1->m_values.size(); i++)
		{
			std::pair<double, double> r = histo1->range(i);
			fprintf(plik, "%3d, %lf, %lf, %lu\n", i, r.first, r.second, histo1->m_values[i] );
		}

		fclose(plik);

		UI::updateAllViews();
	}
	else if (0 == name.compare(L"Error_ART"))
	{
		int levels = 256;
		bool useAbsValues = true;// false;
		bool useNormals = true;

		std::pair<double, double> limits(-0.85, 0.85);

		//std::wstring dir(L"d://darekp//prog//dpVisionProject//porownanie_do_art//");
		std::wstring dir(L".//roboczy//porownanie_do_art//");

		CModel3D * obj1 = AP::MODEL::load(dir + L"pachwork//pow_zgr_1+1_8.obj");
		AP::WORKSPACE::addModel(obj1); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id1 = obj1->id();

		CModel3D * obj2 = AP::MODEL::load(dir + L"pachwork//szyna_1.obj");
		AP::WORKSPACE::addModel(obj2); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id2 = obj2->id();

		UI::updateAllViews();
		//UI::DOCK::WORKSPACE::update();

		CHistogram *histo_pach = new CHistogram(symulator->meshDistance_withNormal(obj1, obj2, DBL_MAX, useNormals, useAbsValues), levels);
		histo_pach->setLabel(L"patchwork histogram");
		obj1->setLabel(L"patchwork");

		AP::MODEL::addAnnotation(obj1, histo_pach);

		CModel3D * obj3 = AP::MODEL::load(dir + L"dyl//zgryz.obj");
		AP::WORKSPACE::addModel(obj3); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id3 = obj3->id();

		CModel3D * obj4 = AP::MODEL::load(dir + L"dyl//szyna.obj");
		AP::WORKSPACE::addModel(obj4); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id4 = obj4->id();

		//obj4->getTransform().translate(CVector3d(0.0, 0.0, -0.1));
		//obj4->applyTransform(CTransform());

		UI::updateAllViews();
		//UI::DOCK::WORKSPACE::update();

		CHistogram *histo_dil = new CHistogram(symulator->meshDistance_withNormal(obj3, obj4, DBL_MAX, useNormals, useAbsValues), levels);
		histo_dil->setLabel(L"dilition histogram");
		obj3->setLabel(L"dilition");

		AP::MODEL::addAnnotation(obj3, histo_dil);

		CModel3D * obj5 = AP::MODEL::load(dir + L"vol//pow_zgr_1+05.obj");
		AP::WORKSPACE::addModel(obj5); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id5 = obj5->id();

		CModel3D * obj6 = AP::MODEL::load(dir + L"vol//szyna_5_KS.obj");
		AP::WORKSPACE::addModel(obj6); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id6 = obj6->id();

		//obj6->getTransform().translate(CVector3d(0.0, 0.0, -0.1));
		//obj6->applyTransform(CTransform());

		UI::updateAllViews();
		//UI::DOCK::WORKSPACE::update();

		CHistogram *histo_vol = new CHistogram(symulator->meshDistance_withNormal(obj5, obj6, DBL_MAX, useNormals, useAbsValues), levels);
		histo_vol->setLabel(L"volumetric histogram");
		obj5->setLabel(L"volumetric");

		AP::MODEL::addAnnotation(obj5, histo_vol);

		CModel3D * obj7 = AP::MODEL::load(dir + L"interp//pow_zgr_1.obj");
		AP::WORKSPACE::addModel(obj7); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id7 = obj7->id();

		CModel3D * obj8 = AP::MODEL::load(dir + L"interp//pow_interp_DP.obj");
		AP::WORKSPACE::addModel(obj8); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id8 = obj8->id();

		obj8->getTransform().translate(CVector3d(0.0, 0.0, 1.8));
		obj8->applyTransform(CTransform());

		UI::updateAllViews();
		UI::DOCK::PROPERTIES::updateProperties();

		CHistogram *histo_int = new CHistogram(symulator->meshDistance_withNormal(obj7, obj8, DBL_MAX, useNormals, useAbsValues), levels);
		histo_int->setLabel(L"interpolation histogram");
		obj7->setLabel(L"interpolation");

		AP::MODEL::addAnnotation(obj7, histo_int);


		std::vector<CHistogram*> v({ histo_pach, histo_dil, histo_vol, histo_int });

		//CHistogram::unifyLimits(v);
		CHistogram::unifyLimits( v, limits );

		CHistogram::save( v, dir, L"histogramy" );

		double wariancja;
		double globalError;

		FILE *plik = fopen("odleglosci.txt", "w");

		for (std::vector<CHistogram*>::iterator it = v.begin(); it != v.end(); it++)
		{
			globalError = symulator->calcGlobalError( (CModel3D*)(*it)->getParent(), (*it)->getData(), wariancja );

			fwprintf( plik, (*it)->getLabelW().c_str() );
			//fprintf(plik, "skipped: %d points.\n\n", liczInf);
			fprintf(plik, "\nmin error: %lf\n", (*it)->getDataMin());
			fprintf(plik, "max error: %lf\n", (*it)->getDataMax());
			fprintf(plik, "global error: %lf\n", globalError);
			fprintf(plik, "wariancja: %lf\n", wariancja);
			fprintf(plik, "odchylenie std.: %lf\n", sqrt(wariancja));

			(*it)->setSelfVisibility(true);
		}

		fprintf(plik, "\nWykresy\n");
		fprintf(plik, "lowerLimit: %lf\n", limits.first);
		fprintf(plik, "upperLimit: %lf\n", limits.second);

		fclose(plik);

		AP::WORKSPACE::removeModel(id2);
		AP::WORKSPACE::removeModel(id4);
		AP::WORKSPACE::removeModel(id6);
		AP::WORKSPACE::removeModel(id8);

		obj1->getTransform().translate(CWektor3D(-40.0, 25.0, 0.0));
		obj3->getTransform().translate(CWektor3D( 25.0, 25.0, 0.0));
		obj5->getTransform().translate(CWektor3D(-40.0,-30.0, 0.0));
		obj7->getTransform().translate(CWektor3D( 25.0,-30.0, 0.0));

		UI::updateAllViews();
	}
	else if (0 == name.compare(L"Error_ART2"))
	{
		int levels = 256;
		bool useAbsValues = true;// false;
		bool useNormals = true;

		std::pair<double, double> limits( 0.00, 0.85 );

		//std::wstring dir(L"d://darekp//prog//dpVisionProject//porownanie_do_art//");
		std::wstring dir(L".//roboczy//porownanie_do_art//");

		CModel3D * obj1 = AP::MODEL::load(dir + L"pachwork//pow_zgr_1+1_8.obj");
		AP::WORKSPACE::addModel(obj1); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id1 = obj1->id();

		CModel3D * obj2 = AP::MODEL::load(dir + L"pachwork//szyna_1.obj");
		AP::WORKSPACE::addModel(obj2); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id2 = obj2->id();

		UI::updateAllViews();
		//UI::DOCK::WORKSPACE::update();

		CHistogram *histo_pach = new CHistogram(symulator->meshDistance_withNormal(obj1, obj2, DBL_MAX, useNormals, useAbsValues), levels);
		histo_pach->setLabel(L"patchwork histogram");
		obj1->setLabel(L"patchwork");

		AP::MODEL::addAnnotation(obj1, histo_pach);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CModel3D * obj3 = AP::MODEL::load(dir + L"dyl//zgryz.obj");
		AP::WORKSPACE::addModel(obj3); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id3 = obj3->id();

		CModel3D * obj4 = AP::MODEL::load(dir + L"dyl//szyna.obj");
		AP::WORKSPACE::addModel(obj4); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id4 = obj4->id();

		//obj4->getTransform().translate(CVector3d(0.0, 0.0, -0.1));
		//obj4->applyTransform(CTransform());

		UI::updateAllViews();
		//UI::DOCK::WORKSPACE::update();

		CHistogram *histo_dil = new CHistogram(symulator->meshDistance_withNormal(obj3, obj4, DBL_MAX, useNormals, useAbsValues), levels);
		histo_dil->setLabel(L"dilition histogram");
		obj3->setLabel(L"dilition");

		AP::MODEL::addAnnotation(obj3, histo_dil);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CModel3D * obj5 = AP::MODEL::load(dir + L"vol//pow_zgr_1+05.obj");
		AP::WORKSPACE::addModel(obj5); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id5 = obj5->id();

		CModel3D * obj6 = AP::MODEL::load(dir + L"vol//szyna_5_KS.obj");
		AP::WORKSPACE::addModel(obj6); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id6 = obj6->id();

		//obj6->getTransform().translate(CVector3d(0.0, 0.0, -0.1));
		//obj6->applyTransform(CTransform());

		UI::updateAllViews();
		//UI::DOCK::WORKSPACE::update();

		CHistogram *histo_vol = new CHistogram(symulator->meshDistance_withNormal(obj5, obj6, DBL_MAX, useNormals, useAbsValues), levels);
		histo_vol->setLabel(L"volumetric histogram");
		obj5->setLabel(L"volumetric");

		AP::MODEL::addAnnotation(obj5, histo_vol);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//CModel3D * obj7 = AP::MODEL::load(dir + L"interp//pow_zgr_1.obj");
		//AP::WORKSPACE::addModel(obj7); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		//int id7 = obj7->id();

		//CModel3D * obj8 = AP::MODEL::load(dir + L"interp//pow_interp_DP.obj");
		//AP::WORKSPACE::addModel(obj8); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		//int id8 = obj8->id();

		//obj8->getTransform().translate(CVector3d(0.0, 0.0, 1.8));
		//obj8->applyTransform(CTransform());

		//UI::updateAllViews();
		//UI::DOCK::PROPERTIES::updateProperties();

		//CHistogram *histo_int = new CHistogram(symulator->meshDistance_withNormal(obj7, obj8, DBL_MAX, useNormals, useAbsValues), levels);
		//histo_int->setLabel(L"interpolation histogram");
		//obj7->setLabel(L"interpolation");

		//AP::MODEL::addAnnotation(obj7, histo_int);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CModel3D * obj7 = AP::MODEL::load(dir + L"rbf//pow_zgr_1.obj");
		AP::WORKSPACE::addModel(obj7); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id7 = obj7->id();

		CModel3D * obj8 = AP::MODEL::load(dir + L"rbf//p3nred01prog1step20_p1500_1.obj");
		AP::WORKSPACE::addModel(obj8); // powinno sie sprawdzić rezultat: 'true' jeśli OK
		int id8 = obj8->id();

		obj8->getTransform().translate(CVector3d(0.0, 0.0, 1.0));
		obj8->applyTransform(CTransform());

		UI::updateAllViews();
		UI::DOCK::PROPERTIES::updateProperties();

		CHistogram *histo_rbf = new CHistogram(symulator->meshDistance_withNormal(obj7, obj8, DBL_MAX, useNormals, useAbsValues), levels);
		histo_rbf->setLabel(L"rbf histogram");
		obj7->setLabel(L"rbf");

		AP::MODEL::addAnnotation(obj7, histo_rbf);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<CHistogram*> v({ histo_pach, histo_dil, histo_vol, histo_rbf });

		//CHistogram::unifyLimits(v);
		CHistogram::unifyLimits(v, limits);

		CHistogram::save(v, dir, L"histogramy");

		double wariancja;
		double globalError;

		FILE *plik = fopen("odleglosci.txt", "w");

		for (std::vector<CHistogram*>::iterator it = v.begin(); it != v.end(); it++)
		{
			globalError = symulator->calcGlobalError((CModel3D*)(*it)->getParent(), (*it)->getData(), wariancja);

			fwprintf(plik, (*it)->getLabelW().c_str());
			//fprintf(plik, "skipped: %d points.\n\n", liczInf);
			fprintf(plik, "\nmin error: %lf\n", (*it)->getDataMin());
			fprintf(plik, "max error: %lf\n", (*it)->getDataMax());
			fprintf(plik, "global error: %lf\n", globalError);
			fprintf(plik, "wariancja: %lf\n", wariancja);
			fprintf(plik, "odchylenie std.: %lf\n", sqrt(wariancja));

			(*it)->setSelfVisibility(true);
		}

		fprintf(plik, "\nWykresy\n");
		fprintf(plik, "lowerLimit: %lf\n", limits.first);
		fprintf(plik, "upperLimit: %lf\n", limits.second);

		fclose(plik);

		AP::WORKSPACE::removeModel(id2);
		AP::WORKSPACE::removeModel(id4);
		AP::WORKSPACE::removeModel(id6);
		AP::WORKSPACE::removeModel(id8);

		obj1->getTransform().translate(CWektor3D(-40.0, 25.0, 0.0));
		obj3->getTransform().translate(CWektor3D(25.0, 25.0, 0.0));
		obj5->getTransform().translate(CWektor3D(-40.0, -30.0, 0.0));
		obj7->getTransform().translate(CWektor3D(25.0, -30.0, 0.0));

		UI::updateAllViews();
	}
	else if (0 == name.compare(L"KD_test"))
	{
		KDTree kd;
		kd.testMain();
	}
	else
	{
	}

	UI::updateAllViews();
}

