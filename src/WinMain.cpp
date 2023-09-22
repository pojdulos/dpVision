#include <Windows.h>
#include "MainApplication.h"
#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char* argv[])
{
//#ifdef _WIN32
//#ifdef _DEBUG
    if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
//#endif
//#endif

    QT_REQUIRE_VERSION(argc, argv, "5.15.2")

    QApplication::setStyle("fusion");

    CMainApplication::theApp = new CMainApplication(argc, argv);


    //QPalette palette = CMainApplication::palette();
    //palette.setColor(QPalette::Window, QColor(53, 53, 53));
    //palette.setColor(QPalette::WindowText, Qt::white);
    //palette.setColor(QPalette::Base, QColor(25, 25, 25));
    //palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    //palette.setColor(QPalette::ToolTipBase, Qt::black);
    //palette.setColor(QPalette::ToolTipText, Qt::white);
    //palette.setColor(QPalette::Text, Qt::white);
    //palette.setColor(QPalette::Button, QColor(53, 53, 53));
    //palette.setColor(QPalette::ButtonText, Qt::white);
    //palette.setColor(QPalette::BrightText, Qt::red);
    //palette.setColor(QPalette::Link, QColor(42, 130, 218));
    //palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    //palette.setColor(QPalette::HighlightedText, Qt::black);
    //CMainApplication::setPalette(palette);

    
    CMainApplication::theApp->preExec();

    std::cout << "\n" << QString(qgetenv("PATH")).toStdString() << "\n";

    int result = CMainApplication::theApp->exec();

    CMainApplication::theApp->postExec();

    CMainApplication::theApp->deleteLater();

//#ifdef _WIN32
//#ifdef _DEBUG
    FreeConsole();
//#endif
//#endif
    return result;
}
