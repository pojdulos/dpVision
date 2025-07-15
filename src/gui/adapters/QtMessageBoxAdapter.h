#pragma once

#include <QObject>
#include <QString>
#include <QMessageBox>
#include "interfaces/IMessageListener.h"

class QtMessageBoxAdapter : public QObject, public IMessageListener {
    Q_OBJECT
public:
    QtMessageBoxAdapter() {}
    virtual void information(const std::string &msg, const std::string &tittle) override {
        QMessageBox::information(0, QString::fromUtf8(tittle.c_str()), QString::fromUtf8(msg.c_str()));
    }
    virtual void warning(const std::string &msg, const std::string &tittle) override {
        QMessageBox::warning(0, QString::fromUtf8(tittle.c_str()), QString::fromUtf8(msg.c_str())); 
    }
    virtual void error(const std::string &msg, const std::string &tittle) override {
        QMessageBox::critical(0, QString::fromUtf8(tittle.c_str()), QString::fromUtf8(msg.c_str())); 
    }
    //virtual int question(const std::string &msg, const std::string &tittle, const std::string &b0, const std::string &b1, const std::string &b2) override {
    //    return QMessageBox::question(0, QString::fromUtf8(tittle.c_str()), QString::fromUtf8(msg.c_str()), QString::fromUtf8(b0.c_str()), QString::fromUtf8(b1.c_str()), QString::fromUtf8(b2.c_str()));
    //}
};
