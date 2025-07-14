#include "MainWindow.h"

#include "AP.h"
#include "MainApplication.h"

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QTcpServer>

#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QTcpSocket>
#include <QCloseEvent>

void CMainWindow::startServer()
{
	server = new QTcpServer(this);
	//server->listen("dpVisionServer");
	server->listen(QHostAddress::LocalHost, 49199);
	connect(server, SIGNAL(newConnection()), this, SLOT(processNewConnection()));
}

void CMainWindow::processNewConnection()
{
	clientConnection = new QTcpSocket(this);
	clientConnection = server->nextPendingConnection();
	
	connect(clientConnection, SIGNAL(readyRead()), this, SLOT(proceessData()));

//	clientConnection->write(QString("Hello. Can I help you?\n\r").toLocal8Bit());
}

#include "Utilities.h"
#include "Workspace.h"

void CMainWindow::proceessData()
{
	QTcpSocket* clientSocket = (QTcpSocket*)sender();
	QByteArray recievedData;
	recievedData = clientSocket->readAll();

	QByteArrayList cmdline = recievedData.simplified().split(' ');
	const QString cmd(cmdline.first());
	QString response;

	if (cmd.startsWith("loadDICOM"))
	{
		const QString filename(cmdline.at(1));

		std::shared_ptr<CModel3D> obj = AP::WORKSPACE::loadModel("*.dcm", filename);
		if (obj != nullptr)
		{
			response = QString::number(obj->id()) + "\n\r";
		}
		else
		{
			response = "Przykro mi. Cos poszlo nie tak...\n\r";
		}
		clientSocket->write(response.toLocal8Bit());
	}
	else if (cmd.startsWith("load")) {
		const QString filename(cmdline.at(1));

		std::shared_ptr<CModel3D> obj = AP::WORKSPACE::loadModel(filename);
		if (obj != nullptr) {
			response = QString::number(obj->id()) + "\n\r";
		}
		else
		{
			response = "Przykro mi. Cos poszlo nie tak...\n\r";
		}
		clientSocket->write(response.toLocal8Bit());
	}
	else if (cmd.startsWith("list")) {
		response = "";
		for (auto x : *AP::getWorkspace())
		{
			if (x.second != nullptr) {
				response = QString::number(x.second->id()) + " " + x.second->getLabel() + " " + x.second->path() + "\n\r";
			}
		}
		clientSocket->write(response.toLocal8Bit());
	}
	else if (cmd.startsWith("rotate")) {
		int id = QString(cmdline.at(1)).toInt();
		double rX = QString(cmdline.at(2)).toDouble();

		std::shared_ptr<CModel3D> obj = std::dynamic_pointer_cast<CModel3D>( AP::WORKSPACE::findId(id) );
		if (obj != nullptr) {
			obj->getTransform().rotateAroundAxisDeg(CVector3d::XAxis(), rX);
			updateAllViews();
			UI::DOCK::PROPERTIES::updateProperties();
			clientSocket->write(QString("Gotowe.\n\r").toLocal8Bit());
		}
		else
			clientSocket->write(QString("Przykro mi. Cos poszlo nie tak...\n\r").toLocal8Bit());
	}
	else if (cmd.startsWith("remove")) {
		int id = QString(cmdline.at(1)).toInt();

		std::shared_ptr<CModel3D> obj = std::dynamic_pointer_cast<CModel3D>(AP::WORKSPACE::findId(id));
		if (obj != nullptr) {
			AP::WORKSPACE::removeModel(id);
			updateAllViews();
			UI::DOCK::PROPERTIES::updateProperties();
			clientSocket->write(QString("Gotowe.\n\r").toLocal8Bit());
		}
		else
			clientSocket->write(QString("Przykro mi. Cos poszlo nie tak...\n\r").toLocal8Bit());
	}
	else if (cmd.startsWith("plugin") && (cmdline.size() > 2))
	{
		response = "Niepoprawna sk�adnia polecenia, napisales: %1 \n\r" + recievedData + "\n\r";
		if (cmdline.at(1).startsWith("run"))
		{
			if (AP::PLUGIN::runPlugin(cmdline.at(2)))
			{
				response = "Wtyczka zosta�a uruchomiona.\n\r";
			}
			else
			{
				response = "Nie znaleziono wtyczki o podanym UUID.\n\r";
			}
		}
		clientSocket->write(response.toLocal8Bit());
	}
	else if (cmd.startsWith("quit")) {
		response = "Bye!!!\n\r";
		clientSocket->write(response.toLocal8Bit());

		QCloseEvent* event = new QCloseEvent();
		closeEvent(event);
	}
	else {
		response = "Nie rozumiem polecenia, napisales: %1 \n\r" + recievedData + "\n\r";
		clientSocket->write(response.toLocal8Bit());
	}
}
