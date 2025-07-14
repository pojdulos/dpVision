#include <QProgressBar>
#include "IProgressListener.h"

class QtProgressAdapter : public IProgressListener {
    QProgressBar* bar_;
public:
    QtProgressAdapter(QProgressBar* bar) : bar_(bar) {}
    void onProgress(float value) override {
        bar_->setValue(int(value * bar_->maximum()));
    }
};
