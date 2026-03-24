#pragma once

class QString;
class ProgressIndicator;

// Legacy compatibility contract. New plugin code should prefer
// IProgressControlAPI for progress reporting and use IGuiInternalsAPI for raw
// progress widget access.
class IProgressAPI {
public:
    virtual ~IProgressAPI() = default;
    [[deprecated("Legacy raw GUI escape hatch. Prefer IProgressControlAPI and only use IGuiInternalsAPI for privileged widget access.")]]
    virtual ProgressIndicator* instance() = 0;
    virtual void init(int min, int max, int val) = 0;
    virtual void setValue(int val) = 0;
    virtual void hide() = 0;
    virtual void setText(const QString& text) = 0;
};
