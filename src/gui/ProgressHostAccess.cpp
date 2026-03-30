#include "ProgressHostAccess.h"

#include "MainWindow.h"
#include "ProgressIndicator.h"

ProgressIndicator* ProgressHostAccess::instance()
{
    if (auto win = CMainWindow::instance()) {
        return win->progressIndicator;
    }
    return nullptr;
}
