#include <string>
#include <functional>
#include <memory>

class IProgressListener {
public:
    virtual ~IProgressListener() = default;
    virtual void init(int min, int max, int val, std::string text = "") = 0;
    virtual void setValue(int value) = 0;
    virtual void setText(std::string text) = 0;
    virtual void hide() = 0;

    virtual void useCancelButton(std::function<void()> onCancel) {}

    static void setDefault(std::shared_ptr<IProgressListener> l) { defaultListener_ = l; }
    static IProgressListener* getDefault() { return defaultListener_.get(); }

private:
    static std::shared_ptr<IProgressListener> defaultListener_;
};
