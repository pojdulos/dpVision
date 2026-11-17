#pragma once

#include <QObject>

#include "interfaces/IStatusListener.h"

class QStatusBar;

class QtStatusBarAdapter : public QObject, public IStatusListener {
    Q_OBJECT

public:
    explicit QtStatusBarAdapter(QStatusBar* bar);

    void setText(const std::string& text) override;
    void clear() override;

private:
    QStatusBar* bar_;
};
