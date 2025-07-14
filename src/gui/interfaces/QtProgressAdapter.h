#include <QObject>
#include <QDebug>
#include "interfaces/IProgressListener.h"
#include "../MainWindow.h"
#include "../ProgressIndicator.h"

class QtProgressAdapter : public QObject, public IProgressListener {
    Q_OBJECT
    ProgressIndicator* progress_;
public:
    QtProgressAdapter(ProgressIndicator* prg = nullptr, QObject* parent = nullptr) : QObject(parent), progress_(prg)
    {
        if (progress_) {
            connect(this, &QtProgressAdapter::sig_init, progress_, &ProgressIndicator::onInit, Qt::QueuedConnection);
            connect(this, &QtProgressAdapter::sig_setValue, progress_, &ProgressIndicator::onSetValue, Qt::QueuedConnection);
            connect(this, &QtProgressAdapter::sig_setText, progress_, &ProgressIndicator::onSetText, Qt::QueuedConnection);
            connect(this, &QtProgressAdapter::sig_hide, progress_, &ProgressIndicator::onHide);// , Qt::QueuedConnection);
        }

    }

    virtual void init(int min, int max, int val, std::string text = "") override {
        //qInfo() << "INIT";
        if (progress_) emit sig_init(min, max, val, QString::fromStdString(text));
    }

    virtual void setValue(int value) override {
        //qInfo() << "SETVALUE " << value;
        if (progress_) emit sig_setValue(value);
    }

    virtual void setText(std::string text) override {
        if (progress_) emit sig_setText(QString::fromStdString(text));
    }

    virtual void hide() override {
        //qInfo() << "HIDE";
        if (progress_) emit sig_hide();
    }

signals:
    void sig_init(int min, int max, int val, QString text = "");
    void sig_setText(QString text);
    void sig_setValue(int value);
    void sig_hide();
};
