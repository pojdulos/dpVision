#include "DockWidgetHistogram.h"

#include "UI.h"
#include "AP.h"

#include "DockWidgetModel.h"
#include "Annotation.h"

#include "Workspace.h"

#include "WorkspaceTreeModel.h"
#include "WorkspaceTreeItem.h"

DockWidgetHistogram::DockWidgetHistogram(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);

	hw = new HistogramWidget(ui.dockWidgetContents);
	ui.gridLayout->addWidget(hw, 0, 0, 1, 1);
}

DockWidgetHistogram::~DockWidgetHistogram()
{
}

CHistogram* DockWidgetHistogram::getHistogram() {
	return hw->getHistogram();
}
