#include "QtProgressAdapter.h"

#include <QMetaObject>
#include <QPushButton>
#include <QThread>

#include "../ProgressIndicator.h"

namespace {
bool runsOnGuiThread(ProgressIndicator* indicator)
{
    return indicator && QThread::currentThread() == indicator->thread();
}
}

QtProgressAdapter::QtProgressAdapter(ProgressIndicator* prg, QObject* parent)
    : QObject(parent)
    , indicator_(prg)
{
    if (indicator_) {
        connect(this, &QtProgressAdapter::sig_init, indicator_, &ProgressIndicator::onInit, Qt::QueuedConnection);
        connect(this, &QtProgressAdapter::sig_setValue, indicator_, &ProgressIndicator::onSetValue, Qt::QueuedConnection);
        connect(this, &QtProgressAdapter::sig_setText, indicator_, &ProgressIndicator::onSetText, Qt::QueuedConnection);
        connect(this, &QtProgressAdapter::sig_hide, indicator_, &ProgressIndicator::onHide, Qt::QueuedConnection);
    }
}

void QtProgressAdapter::init(int min, int max, int val, std::string text)
{
    if (indicator_) {
        const QString qText = QString::fromStdString(text);
        if (runsOnGuiThread(indicator_)) {
            indicator_->onInit(min, max, val, qText);
        } else {
            emit sig_init(min, max, val, qText);
        }
    }
}

void QtProgressAdapter::setValue(int value)
{
    if (indicator_) {
        if (runsOnGuiThread(indicator_)) {
            indicator_->onSetValue(value);
        } else {
            emit sig_setValue(value);
        }
    }
}

void QtProgressAdapter::setText(std::string text)
{
    if (indicator_) {
        const QString qText = QString::fromStdString(text);
        if (runsOnGuiThread(indicator_)) {
            indicator_->onSetText(qText);
        } else {
            emit sig_setText(qText);
        }
    }
}

void QtProgressAdapter::hide()
{
    if (indicator_) {
        auto hideCancelButton = [indicator = indicator_]() {
            if (!indicator || !indicator->cancelButton()) {
                return;
            }

            disconnect(indicator->cancelButton(), nullptr, nullptr, nullptr);
            indicator->cancelButton()->hide();
        };

        if (runsOnGuiThread(indicator_)) {
            indicator_->onHide();
            hideCancelButton();
        } else {
            emit sig_hide();
            QMetaObject::invokeMethod(indicator_, hideCancelButton, Qt::QueuedConnection);
        }
    }
}

void QtProgressAdapter::useCancelButton(std::function<void()> onCancel)
{
    if (!indicator_) {
        return;
    }

    QMetaObject::invokeMethod(indicator_, [indicator = indicator_, onCancel = std::move(onCancel)]() mutable {
        if (!indicator || !indicator->cancelButton()) {
            return;
        }

        indicator->cancelButton()->show();
        indicator->cancelButton()->update();
        indicator->cancelButton()->repaint();

        disconnect(indicator->cancelButton(), nullptr, nullptr, nullptr);
        QObject::connect(indicator->cancelButton(), &QPushButton::clicked, indicator, [onCancel]() {
            if (onCancel) {
                onCancel();
            }
        });
    }, Qt::QueuedConnection);
}
