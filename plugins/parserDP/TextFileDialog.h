#ifndef TEXTFILEDIALOG_H
#define TEXTFILEDIALOG_H
#include "dll_global.h"

#include "ui_TextFileDialog.h"

#include "QtWidgets/QWidget"
#include "QtWidgets/QComboBox"

#include "TextFileDialogFieldType.h"

class DPVISION_DLL_API MyValueWidget : public QWidget
{
	Q_OBJECT

		const static char* typeNames[];

public:


	explicit MyValueWidget(QString value, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	~MyValueWidget() {};

	QComboBox* typeCombo;
};

class DPVISION_DLL_API TextFileDialog : public QDialog
{
	Q_OBJECT

public:
	QVector<QString> tenLines;
	int skipLines;
	QChar separator;
	QVector<MyValueWidget*> valueWidgets;
	int currentPreset;

	explicit TextFileDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~TextFileDialog() {};

	inline Ui::Dialog& getUI() { return ui; };

	bool init(const QString fileName, int lines);

	void addValueWidgets();

	void removeValueWidgets();
	
	void storePreset(const QString preset);
	void storeCurrentPreset();
	void readPresets();

public slots:
	void onSepRadio(bool);
	void onLineNb(int);
	void onPreset(QString);

private:
	Ui::Dialog ui;
};

#endif