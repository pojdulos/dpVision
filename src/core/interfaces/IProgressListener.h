#include <string>

class IProgressListener {
public:
    virtual ~IProgressListener() = default;
    virtual void onProgress(float value) = 0; // value np. od 0.0 do 1.0
    virtual void onMessage(const std::string& msg) {} // opcjonalnie
};
