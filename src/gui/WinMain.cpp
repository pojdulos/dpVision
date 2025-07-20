//#include <Windows.h>
#include "MainApplication.h"
#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>


#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "AppSettings.h"

#include "../api/AP.h"

#include "StatusBarManager.h"

int main(int argc, char* argv[])
{
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode(hConsole, &consoleMode);
    consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, consoleMode);
#endif

    QT_REQUIRE_VERSION(argc, argv, "5.15.2")

    CMainApplication::theApp = new CMainApplication(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("dpVision - eksperymentalne narzędzie do wizualizacji, edycji i integracji wielomodalnych i wielowymiarowych danych.");
    parser.addHelpOption();
    parser.addVersionOption();

    // Dodanie opcji
    QCommandLineOption optionVerbose(QStringList() << "V" << "verbose", QString::fromUtf8("Włącz tryb szczegółowy."));
    parser.addOption(optionVerbose);

    QCommandLineOption optionDark(QStringList() << "d" << "dark", QString::fromUtf8("Włącz tryb ciemny."));
    parser.addOption(optionDark);

    QCommandLineOption optionFile(QStringList() << "f" << "file", QString::fromUtf8("Ścieżka do pliku."), "file");
    parser.addOption(optionFile);

    QCommandLineOption optionMode(QStringList() << "m" << "mode", "Tryb pracy aplikacji.", "mode", "default");
    parser.addOption(optionMode);

    // Analiza opcji
    parser.process(CMainApplication::theApp->arguments());

    CMainApplication::theApp->verbose_mode = parser.isSet(optionVerbose);
    
    QString filePath = parser.value(optionFile);
    QString mode = parser.value(optionMode);

    qDebug() << QString::fromUtf8("Tryb szczegółowy:") << QString::fromUtf8(CMainApplication::theApp->verbose_mode ? "Włączony" : "Wyłączony");
    qDebug() << QString::fromUtf8("Ścieżka do pliku: %1").arg(filePath.isEmpty() ? "Nie podano" : filePath);
    qDebug() << QString::fromUtf8("Tryb pracy: %1").arg(mode);

    //QLocale locale(QLocale::C);
    //QLocale::setDefault(locale);

    Q_INIT_RESOURCE(dpVision);

    AppSettings::apply();

    //if (AppSettings::mainSettings()->value("mainwindow/maximized", false).toBool())
    //{
    //    CMainWindow::instance()->showMaximized();
    //}
    //else
    //{
    CMainWindow::instance()->show();
    //}

    //CMainWindow::instance()->restoreGeometry(AppSettings::mainSettings()->value("gui/mainwindow/geometry").toByteArray());
    //CMainWindow::instance()->restoreState(AppSettings::mainSettings()->value("gui/mainwindow/dockState").toByteArray());


    StatusBarManager::setText("loading plugins...");

    CMainApplication::theApp->LoadAllPlugins();

    //wczytuję pliki z linii poleceń jeśli podano
    for (const QString& plik : parser.positionalArguments())
    {
        if (QFile(plik).exists())
        {
            StatusBarManager::setText("loading model: " + plik);
            AP::WORKSPACE::loadModel(plik);
        }
    }
    
    CWorkspace::instance()->_objectActivate(NO_CURRENT_MODEL);

    CMainApplication::theApp->isInitialised = true;

    StatusBarManager::setText("starting tcp server...");

    CMainWindow::instance()->startServer();

    //FreeConsole();
    //AllocConsole();
    //AttachConsole(GetCurrentProcessId());
    //freopen("CON", "w", stdout); 
    //freopen("CON", "w", stderr);
    //freopen("CON", "r", stdin);

    StatusBarManager::setText("ready...");











    //std::cout << "\n" << QString(qgetenv("PATH")).toStdString() << "\n";

    int result = CMainApplication::theApp->exec();

    CMainApplication::theApp->UnloadAllPlugins();
    //CWorkspace::instance()->clear();
    //CWorkspace::instance()->deleteLater();

    CMainApplication::theApp->deleteLater();

#ifdef _WIN32
    FreeConsole();
#endif

    return result;
}
