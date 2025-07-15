#include <QObject>
#include <QDebug>
#include "interfaces/IProgressListener.h"
#include "../MainWindow.h"
#include "../ProgressIndicator.h"

class QtProgressAdapter : public QObject, public IProgressListener {
    Q_OBJECT
    ProgressIndicator* indicator_;
public:
    QtProgressAdapter(ProgressIndicator* prg = nullptr, QObject* parent = nullptr) : QObject(parent), indicator_(prg)
    {
        if (indicator_) {
            connect(this, &QtProgressAdapter::sig_init, indicator_, &ProgressIndicator::onInit, Qt::QueuedConnection);
            connect(this, &QtProgressAdapter::sig_setValue, indicator_, &ProgressIndicator::onSetValue, Qt::QueuedConnection);
            connect(this, &QtProgressAdapter::sig_setText, indicator_, &ProgressIndicator::onSetText, Qt::QueuedConnection);
            connect(this, &QtProgressAdapter::sig_hide, indicator_, &ProgressIndicator::onHide, Qt::QueuedConnection);
        }
    }

    virtual void init(int min, int max, int val, std::string text = "") override {
        if (indicator_) emit sig_init(min, max, val, QString::fromStdString(text));
    }

    virtual void setValue(int value) override {
        if (indicator_) emit sig_setValue(value);
    }

    virtual void setText(std::string text) override {
        if (indicator_) emit sig_setText(QString::fromStdString(text));
    }

    virtual void hide() override {
        if (indicator_) {
            disconnect(indicator_->cancelButton(), nullptr, nullptr, nullptr);
            indicator_->cancelButton()->hide();

            emit sig_hide();
        }
    }

    void useCancelButton(std::function<void()> onCancel) override {
        if (!indicator_ || !indicator_->cancelButton()) return;
        indicator_->cancelButton()->show();
        indicator_->cancelButton()->update();
        indicator_->cancelButton()->repaint(); 

        // Od³¹cz stare, podepnij nowe:
        disconnect(indicator_->cancelButton(), nullptr, nullptr, nullptr);
        QObject::connect(indicator_->cancelButton(), &QPushButton::clicked, indicator_, [onCancel]() { if (onCancel) onCancel(); });
    }

signals:
    void sig_init(int min, int max, int val, QString text = "");
    void sig_setText(QString text);
    void sig_setValue(int value);
    void sig_hide();
};
