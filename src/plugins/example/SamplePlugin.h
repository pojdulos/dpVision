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

	virtual void onLoad() override;
	virtual void onButton(const QString &name) override;

//public slots:
	void loadObject(const QString &path="");
	void createBox();
};
