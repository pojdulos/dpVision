#pragma once

#include <QObject>
#include <QString>
#include <QStatusBar>
#include "interfaces/IStatusListener.h"

class QtStatusBarAdapter : public QObject, public IStatusListener {
    Q_OBJECT
public:
    QtStatusBarAdapter(QStatusBar* bar) : bar_(bar) {}
    void setText(const std::string& text) override {
        if (bar_) bar_->showMessage(QString::fromStdString(text));
    }
    void clear() override {
        if (bar_) bar_->clearMessage();
    }
private:
    QStatusBar* bar_;
};
