#include "ParserTXT.h"

#include "ParserLoaderTXT.h"
#include "AP.h"

#include <QtCore/QThread>

CParserTXT::CParserTXT() : ThreadParser()
{
	setDescr("Text 3D scanner files");
	m_exts.insert("txt");
	m_exts.insert("asc");
	m_exts.insert("xyz");
	m_exts.insert("xyzi"); // file with 16-bit intensity (grayscale) for each point
}

CParserTXT::~CParserTXT(void)
{
}


CModel3D* CParserTXT::load(const QString path, bool wait)
{
	synchronous = wait;

	fileinfo.setFile( path );

	if (fileinfo.exists())
	{
		if (dialog.init(fileinfo.absoluteFilePath(), 100) && dialog.exec())
		{
			loader = new ParserLoaderTXT(fileinfo.absoluteFilePath());

			for (int i = 0; i < dialog.valueWidgets.size(); i++)
			{
				((ParserLoaderTXT*)loader)->addField((FieldType)dialog.valueWidgets[i]->typeCombo->currentIndex());
			}

			((ParserLoaderTXT*)loader)->setSeparator(dialog.separator);
			((ParserLoaderTXT*)loader)->skipLines(dialog.skipLines);

			done = false;
			cancelled = false;

			runLoader();

			if (synchronous)
			{
				while (!done && !cancelled)
				{
					AP::processEvents(true);
				}

				if (done)
				{
					ThreadParser::onLoadDone();

					return m_model;
				}
			}
		}
		else
		{
			UI::STATUSBAR::setText("File load cancelled!");
		}
	}

	return nullptr;
}


void CParserTXT::onLoadDone()
{
	dialog.storeCurrentPreset();
	
	m_model = new CModel3D();
	m_model->addChild(loader->getChild());
	
	m_model->setPath(fileinfo.absoluteFilePath());
	m_model->setLabel(fileinfo.fileName());

	done = true;

	if (!synchronous)
	{
		AP::WORKSPACE::addModel(m_model);

		ThreadParser::onLoadDone();
	}
}

