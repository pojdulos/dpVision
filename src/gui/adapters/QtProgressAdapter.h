#pragma once

#include <QObject>

#include "dll_global.h"
#include "interfaces/IProgressListener.h"

class ProgressIndicator;

class DPVISION_EXPORT QtProgressAdapter : public QObject, public IProgressListener {
    Q_OBJECT

    ProgressIndicator* indicator_;

public:
    explicit QtProgressAdapter(ProgressIndicator* prg = nullptr, QObject* parent = nullptr);

    void init(int min, int max, int val, std::string text = "") override;
    void setValue(int value) override;
    void setText(std::string text) override;
    void hide() override;
    void useCancelButton(std::function<void()> onCancel) override;

signals:
    void sig_init(int min, int max, int val, QString text = "");
    void sig_setText(QString text);
    void sig_setValue(int value);
    void sig_hide();
};
