#pragma once

#include "../core/dll_global.h"

class ProgressIndicator;
class QString;

class DPVISION_EXPORT ProgressHostAccess {
public:
    static ProgressIndicator* instance();
    static void init(int min, int max, int val);
    static void setValue(int val);
    static void hide();
    static void setText(const QString& text);
};
