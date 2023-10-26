#pragma once
#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H

#include "Global.h"
#include "DPFileInfo.h"

#include "BackgroundPlane.h"
#include "Workspace.h"

#include "PluginInterface.h"
#include "PluginManager.h"

#include <QtWidgets/QApplication>
#include <QtCore/QSettings>

typedef QMap<unsigned int, PluginInterface*> CPlugins;

class __declspec(dllexport) CMainApplication : public QApplication
{
	//Q_OBJECT
public:
	using QApplication::QApplication;
	CMainApplication(int& argc, char** argv);
	static CMainApplication* theApp;

protected:
	QStringList vListaPlikow;

	QString sExeDir;
	QString sAppDir;
	QString sTmpDir;

	int m_lastObjectId;

public:
	bool isInitialised;
	bool bGlobalPicking;
	bool bPickSnap;

	CPlugins plugins;
	PluginInterface *activePlugin;

	QSettings *settings;

	const QString& appExecDir() { return sExeDir; };
	const QString& appDataDir() { return sAppDir; };
	const QString& appTempDir() { return sTmpDir; };

	PluginInterface *getPlugin( unsigned int id );
	PluginInterface* getPlugin(const char* strUUID);


	void preExec();
	void postExec();

	bool loadQtPlugin(const QString &pluginPath);
	bool loadWinPlugin(const QString &pluginPath);
	bool loadPlugin( const QString &pluginPath );

	void unloadPlugin( const QString pluginPath );
	void unloadPlugin( const unsigned int id );
	void UnloadAllPlugins();
	void LoadAllPlugins();
	void RunPlugin( const unsigned int id );
	bool runPlugin(const char* strUUID);

	int getUniqueId();
};

#endif