#include <string>

class IProgressListener {
public:
    virtual ~IProgressListener() = default;
    virtual void init(int min, int max, int val, std::string text = "") = 0;
    virtual void setValue(int value) = 0;
    virtual void setText(std::string text) = 0;
    virtual void hide() = 0;
};
