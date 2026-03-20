#include "WorkspaceFile.h"
//#include "MainWindow.h"
//#include "ChildWindow.h"
#include "../api/adapters/AppAPIAdapter.h"
#include "StatusBarManager.h"
#include "utils/StringUtils.h"

namespace {
AppAPIAdapter& appApi()
{
	static AppAPIAdapter api;
	return api;
}
}


void CWorkspaceFile::loadFaro(std::wstring p, CWorkspace & wksp)
{
	char section[255];
	char value[255];

	float x, y, z;
	
	const char * path = dpVisionCore::utils::wstr2str(p).c_str();

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
			std::shared_ptr<CModel3D> obj = appApi().workspace().loadModel(fwsname.absoluteFilePath());
			if (NULL != obj)
			{
				id = obj->id();

				//obj->attachTexture();
				//printf( Last()->GetMeshInfoText().c_str() );

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

				StatusBarManager::setText(obj->getLabel());
			}
		}
	}

}


