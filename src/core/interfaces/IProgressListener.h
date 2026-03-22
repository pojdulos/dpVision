#include <string>
#include <functional>
#include <memory>
#include "dll_global.h"

class IProgressListener {
public:
    virtual ~IProgressListener() = default;
    virtual void init(int min, int max, int val, std::string text = "") = 0;
    virtual void setValue(int value) = 0;
    virtual void setText(std::string text) = 0;
    virtual void hide() = 0;

    virtual void useCancelButton(std::function<void()> onCancel) {}

    static void setDefault(std::shared_ptr<IProgressListener> l) { defaultListenerRef() = std::move(l); }
    static std::shared_ptr<IProgressListener> getDefault() { return defaultListenerRef(); }

private:
    static DPVISION_EXPORT std::shared_ptr<IProgressListener>& defaultListenerRef();
};
