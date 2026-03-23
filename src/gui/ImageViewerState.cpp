#include "ImageViewerState.h"

#include <unordered_map>

namespace {
std::unordered_map<int, bool>& fitMap()
{
    static std::unordered_map<int, bool> values;
    return values;
}
}

bool ImageViewerState::fitToWindow(int imageId)
{
    auto it = fitMap().find(imageId);
    if (it == fitMap().end()) {
        return false;
    }
    return it->second;
}

void ImageViewerState::setFitToWindow(int imageId, bool fit)
{
    fitMap()[imageId] = fit;
}

void ImageViewerState::clear(int imageId)
{
    fitMap().erase(imageId);
}
