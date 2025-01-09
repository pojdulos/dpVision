#pragma once
#include "PluginInterface.h"

class SamplePlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "dpVision.PluginInterface" FILE "metadata.json")
    Q_INTERFACES(PluginInterface)

public:
	SamplePlugin(void) {}
	~SamplePlugin(void) {}

    /**
     * @brief Called on double-click on plugin name
     */
    // virtual void onRun() {}

    /**
     * @brief Called when plugin is loaded
     */
    virtual void onLoad() override;

    /**
     * @brief Called before plugin unload
     */
    // virtual void onUnload() {}

    /**
     * @brief Called when plugin is activated
     */
    // virtual void onActivate() {}

    /**
     * @brief Called when plugin is deactivated
     */
    // virtual void onDeactivate() {}

    /**
     * @brief Called when button on PluginPanel is pressed (deprecated)
     * @param name Button name
     */
    // virtual void onButton(std::wstring name) {}

    /**
     * @brief Called when button on PluginPanel is pressed
     * @param name Button name
     */
    virtual void onButton(const QString& name) override;

    /**
     * @brief Called when plugin is active and Ctrl+MouseClick is performed on any model
     * @param pickEvent Pick event information
     * @return true if event was handled, false otherwise
     */
    // virtual bool onMousePick(PickEvent pickEvent) { return false; }

    /**
     * @brief Called when plugin is active and Shift+MouseClick is performed on any model
     * @param objId ID of indicated object
     * @return true if event was handled, false otherwise
     */
    // virtual bool onModelIndication(int objId) { return false; }


private:
	void loadObject(const QString &path="");
	void createBox();
	void cutMesh();
};
