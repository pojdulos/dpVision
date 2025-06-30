#include "WorkspaceFile.h"
#include "MainWindow.h"
#include "GLViewer.h"
//#include "ChildWindow.h"


//void CWorkspaceFile::saveDPW(CWorkspace & wksp, std::wstring p)
//{
//	int lb = wksp.m_data.size();
//	const char * path = UI::wstr2str(p).c_str();
//	
//	char section[255];
//	char value[255];
//
//	sprintf(value, "%d", lb);
//	WritePrivateProfileStringA("workspace", "count", value, path);
//
//	for (std::map<int, CModel3D*>::iterator it = wksp.m_data.begin(); it != wksp.m_data.end(); it++)
//	{
//		sprintf(section, "model_%d", (*it).second->id());
//		WritePrivateProfileStringA(section, "filename", (*it).second->pathA().c_str(), path);
//
//		sprintf(value, "%f,%f,%f", (*it).second->getTransform().translation().X(), (*it).second->getTransform().translation().Y(), (*it).second->getTransform().translation().Z());
//		WritePrivateProfileStringA(section, "translation", value, path);
//
//		CVector3d e = (*it).second->getTransform().rotation().eulerAnglesDeg();
//		sprintf(value, "%f,%f,%f", e.x, e.y, e.z);
//		WritePrivateProfileStringA(section, "rotation", value, path);
//
//		sprintf(value, "%f", (*it).second->getTransform().scale().x);
//		WritePrivateProfileStringA(section, "scale", value, path);
//
//		sprintf(value, "%d", (*it).second->isLocked() ? 1 : 0);
//		WritePrivateProfileStringA(section, "lock", value, path);
//	}
//
//	_light *m_lights = wksp.m_lights;
//
//	for (int i = 0; i<8; i++)
//	{
//		sprintf(section, "light%d_active", i);
//		WritePrivateProfileStringA("lights", section, m_lights[i].active ? "1" : "0", path);
//		sprintf(section, "light%d_fixed", i);
//		WritePrivateProfileStringA("lights", section, m_lights[i].fixed ? "1" : "0", path);
//
//		sprintf(section, "light%d_ambient", i);
//		sprintf(value, "%f,%f,%f,%f", m_lights[i].ambient[0], m_lights[i].ambient[1], m_lights[i].ambient[2], m_lights[i].ambient[3]);
//		WritePrivateProfileStringA("lights", section, value, path);
//		sprintf(section, "light%d_diffuse", i);
//		sprintf(value, "%f,%f,%f,%f", m_lights[i].diffuse[0], m_lights[i].diffuse[1], m_lights[i].diffuse[2], m_lights[i].diffuse[3]);
//		WritePrivateProfileStringA("lights", section, value, path);
//		sprintf(section, "light%d_specular", i);
//		sprintf(value, "%f,%f,%f,%f", m_lights[i].specular[0], m_lights[i].specular[1], m_lights[i].specular[2], m_lights[i].specular[3]);
//		WritePrivateProfileStringA("lights", section, value, path);
//		sprintf(section, "light%d_position", i);
//		sprintf(value, "%f,%f,%f,%f", m_lights[i].position[0], m_lights[i].position[1], m_lights[i].position[2], m_lights[i].position[3]);
//		WritePrivateProfileStringA("lights", section, value, path);
//		sprintf(section, "light%d_spot_direction", i);
//		sprintf(value, "%f,%f,%f", m_lights[i].spot_direction[0], m_lights[i].spot_direction[1], m_lights[i].spot_direction[2]);
//		WritePrivateProfileStringA("lights", section, value, path);
//		sprintf(section, "light%d_spot_cut_off", i);
//		sprintf(value, "%f", m_lights[i].spot_cut_off);
//		WritePrivateProfileStringA("lights", section, value, path);
//	}
//}
//

void CWorkspaceFile::loadFaro(std::wstring p, CWorkspace & wksp)
{
	char section[255];
	char value[255];

	float x, y, z;
	
	const char * path = UI::wstr2str(p).c_str();

	wksp.clear();

	// GLuint lb = GetPrivateProfileIntA("faro", "NumScans", 0, path);
	QSettings settings(QString::fromUtf8(path), QSettings::IniFormat);
	GLuint lb = settings.value("faro/NumScans", 0).toUInt();

	for (GLuint i = 0; i<lb; i++)
	{
		sprintf(section, "scan_%d", i);
		// GetPrivateProfileStringA(section, "OccupancyGrid", "", value, 255, path);
		QString val = settings.value(QString("%1/OccupancyGrid").arg(section), "").toString();
		QByteArray ba = val.toUtf8();
		strncpy(value, ba.constData(), 255);
		value[254] = '\0'; // dla pewności zakończenia

		// Siatk� zaj�to�ci trzeba b�dzie wczyta� jako osobny model lub jako podelement modelu
		// Trzeba to przemy�le� i przedyskutowa�

		// GetPrivateProfileStringA(section, "Name", "", value, 255, path);
		val = settings.value(QString("%1/Name").arg(section), "").toString();
		ba = val.toUtf8();
		strncpy(value, ba.constData(), 255);
		value[254] = '\0'; // dla pewności zakończenia

		CFileInfo fwsname(value);

		int id;

		if (fwsname.hasExt("fws"))
		{
			// pliki fws potraktujemy specjalnie...
			// tu trzeba by przerobi� parser FWS, albo skopiowa� co trzeba



		}
		else
		{
			CModel3D *obj = AP::WORKSPACE::loadModel(fwsname.absoluteFilePath().toStdWString());
			if (NULL != obj)
			{
				id = obj->id();

				//obj->attachTexture();
				//UI::STATUSBAR::printf( Last()->GetMeshInfoText().c_str() );

				//GetPrivateProfileStringA(section, "translation", "0.0,0.0,0.0", value, 255, path);
				QString val = settings.value(QString("%1/translation").arg(section), "0.0,0.0,0.0").toString();
				QByteArray ba = val.toUtf8();
				strncpy(value, ba.constData(), 255);
				value[254] = '\0'; // dla pewności zakończenia

				sscanf(value, "%f,%f,%f", &x, &y, &z);
				obj->getTransform().translation() = CVector3d(x, y, z);

				//GetPrivateProfileStringA(section, "rotation", "0.0,0.0,0.0", value, 255, path);
				val = settings.value(QString("%1/rotation").arg(section), "0.0,0.0,0.0").toString();
				ba = val.toUtf8();
				strncpy(value, ba.constData(), 255);
				value[254] = '\0'; // dla pewności zakończenia

				sscanf(value, "%f,%f,%f", &x, &y, &z);
				obj->getTransform().rotation().fromEulerAnglesDeg( x, y, z);

				//GetPrivateProfileStringA(section, "scale", "0.0", value, 255, path);
				val = settings.value(QString("%1/scale").arg(section), "0.0").toString();
				ba = val.toUtf8();
				strncpy(value, ba.constData(), 255);
				value[254] = '\0'; // dla pewności zakończenia

				sscanf(value, "%f", &x);
				obj->getTransform().setScale(x);

				uint lb = settings.value(QString("%1/lock").arg(section), 0).toUInt();
				if (1 == lb ) //GetPrivateProfileIntA(section, "lock", 0, path))
					obj->setLocked(true);
				else
					obj->setLocked(false);

				UI::STATUSBAR::printf(obj->getLabelW().c_str());
			}
		}
	}

}


//void CWorkspaceFile::loadDPW(std::wstring p, CWorkspace & wksp)
//{
//	char section[255];
//	char value[255];
//
//	float x, y, z;
//
//	const char * path = UI::wstr2str(p).c_str();
//
//	wksp.clear();
//
//	GLuint lb = GetPrivateProfileIntA("workspace", "count", 0, path);
//
//	for (GLuint i = 0; i<lb; i++)
//	{
//		sprintf(section, "model_%d", i);
//		GetPrivateProfileStringA(section, "filename", "", value, 255, path);
//
//		
//		CModel3D *obj = AP::WORKSPACE::loadModel(UI::str2wstr(value));
//		if ( NULL != obj )
//		{
//			int id = obj->id();
//
//			//obj->attachTexture();
//			//UI::STATUSBAR::printf( Last()->GetMeshInfoText().c_str() );
//
//			GetPrivateProfileStringA(section, "translation", "0.0,0.0,0.0", value, 255, path);
//			sscanf(value, "%f,%f,%f", &x, &y, &z);
//			obj->getTransform().translation() = CVector3d(x, y, z);
//
//			GetPrivateProfileStringA(section, "rotation", "0.0,0.0,0.0", value, 255, path);
//			sscanf(value, "%f,%f,%f", &x, &y, &z);
//			obj->getTransform().rotation().fromEulerAnglesDeg(x, y, z);
//
//			GetPrivateProfileStringA(section, "scale", "0.0", value, 255, path);
//			sscanf(value, "%f", &x);
//			obj->getTransform().setScale(x);
//
//			if (1 == GetPrivateProfileIntA(section, "lock", 0, path))
//				obj->setLocked(true);
//			else
//				obj->setLocked(false);
//
//			UI::STATUSBAR::printf(obj->getLabelW().c_str());
//		}
//	}
//}

