#include <Windows.h>
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

#include "AP.h"

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

    QApplication::setStyle("fusion");

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

    if (parser.isSet(optionDark)) {
        QPalette palette = CMainApplication::palette();
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(25, 25, 25));
        palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, Qt::black);
        CMainApplication::setPalette(palette);

        qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"
            "QWidget { background-color: #353535; color: #ffffff; }"
            "QWidget:disabled { color: #555555 }"
            "QMainWindow::separator { background-color: #353535; }"
            "QFrame { border: 1px solid #353535; }"
            "QPushButton { background-color: #353535; color: #ffffff; border: 1px solid #808080; padding: 4px }"
            "QPushButton:hover { background-color: #555555; }"
            "QPushButton:disabled { background-color: #353535; color: #555555; border: 1px solid #555555 }"
            "QMenuBar { background-color: #353535; color: #ffffff; }"
            "QMenuBar::item { background-color: #353535; color: #ffffff; }"
            "QMenuBar::item:selected { background-color: #555555; }"
            "QMenu { background-color: #353535; color: #ffffff; }"
            "QMenu::item:selected { background-color: #555555; }"
            "QStatusBar { background-color: #353535; color: #ffffff; }"
            "QScrollBar { background-color: #353535; }"
            "QScrollBar::handle { background-color: #555555; }"
            "QScrollBar::add-line { background-color: #353535; }"
            "QScrollBar::sub-line { background-color: #353535; }");
    }

    
    //CMainApplication::theApp->preExec();

    Q_INIT_RESOURCE(dpVision);

    if (CMainApplication::theApp->settings->value("mainwindow/maximized", false).toBool())
    {
        CMainWindow::instance()->showMaximized();
    }
    else
    {
        CMainWindow::instance()->show();
    }

    UI::STATUSBAR::printf("loading plugins...");

    CMainApplication::theApp->LoadAllPlugins();

    //wczytuję pliki z linii poleceń jeśli podano
    for (const QString& plik : parser.positionalArguments())
    {
        if (QFile(plik).exists())
        {
            UI::STATUSBAR::setText("loading model: " + plik);
            AP::WORKSPACE::loadModel(plik);
        }
    }
    
    CWorkspace::instance()->_setCurrentModel(NO_CURRENT_MODEL);

    CMainApplication::theApp->isInitialised = true;

    UI::STATUSBAR::printf("starting tcp server...");

    CMainWindow::instance()->startServer();

    //FreeConsole();
    //AllocConsole();
    //AttachConsole(GetCurrentProcessId());
    //freopen("CON", "w", stdout); 
    //freopen("CON", "w", stderr);
    //freopen("CON", "r", stdin);

    UI::STATUSBAR::printf("ready...");











    //std::cout << "\n" << QString(qgetenv("PATH")).toStdString() << "\n";

    int result = CMainApplication::theApp->exec();

    CMainApplication::theApp->UnloadAllPlugins();
    CWorkspace::instance()->clear();

    CMainApplication::theApp->deleteLater();

#ifdef _WIN32
    FreeConsole();
#endif

    return result;
}
