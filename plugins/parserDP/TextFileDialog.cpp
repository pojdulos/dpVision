#include "TextFileDialog.h"

const char* MyValueWidget::typeNames[] = {
        "Ignore",
        "X coordinate",
        "Y coordinate",
        "Z coordinate",
        "Red (8-bit)",
        "Green (8-bit)",
        "Blue (8-bit)",
        "Alpha (8-bit)",
        "Red (float)",
        "Green (float)",
        "Blue (float)",
        "Alpha (float)",
        "Grayscale (8-bit)",
        "Grayscale (16-bit)",
        "Reflection (8-bit)",
        "Horizontal angle (float)",
        "Vertical angle (float)",
        "Distance (float)"
};

MyValueWidget::MyValueWidget(QString value, QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
    QHBoxLayout* horizontalLayout_3 = new QHBoxLayout(this);
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    horizontalLayout_3->setContentsMargins(-1, 1, -1, -1);


    QLabel* textLabel = new QLabel(this);
    textLabel->setObjectName(QString::fromUtf8("textLabel"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(textLabel->sizePolicy().hasHeightForWidth());
    textLabel->setSizePolicy(sizePolicy);
    textLabel->setMinimumSize(QSize(80, 0));

    horizontalLayout_3->addWidget(textLabel);

    textLabel->setText(value);

    typeCombo = new QComboBox(this);
    horizontalLayout_3->addWidget(typeCombo);

    int i = 0;
    for (const char* t : typeNames)
    {
        typeCombo->insertItem(i++, t);
    }

    //typeCombo->insertItem(Ignore, typeNames[Ignore]);
    //typeCombo->insertItem(floatX, typeNames[floatX]);
    //typeCombo->insertItem(floatY, typeNames[floatY]);
    //typeCombo->insertItem(floatZ, typeNames[floatZ]);
    //typeCombo->insertItem(byteRed, typeNames[byteRed]);
    //typeCombo->insertItem(byteGreen, typeNames[byteGreen]);
    //typeCombo->insertItem(byteBlue, typeNames[byteBlue]);
    //typeCombo->insertItem(byteAlpha, typeNames[byteAlpha]);
    //typeCombo->insertItem(floatRed, typeNames[floatRed]);
    //typeCombo->insertItem(floatGreen, typeNames[floatGreen]);
    //typeCombo->insertItem(floatBlue, typeNames[floatBlue]);
    //typeCombo->insertItem(floatAlpha, typeNames[floatAlpha]);
    //typeCombo->insertItem(byteGrayscale, typeNames[byteGrayscale]);
    //typeCombo->insertItem(wordGrayscale, typeNames[wordGrayscale]);

    //typeCombo->insertItem(byteReflection, typeNames[byteReflection]);
    //typeCombo->insertItem(floatHorizontalAngle, typeNames[floatHorizontalAngle]);
    //typeCombo->insertItem(floatVerticalAngle, typeNames[floatVerticalAngle]);
    //typeCombo->insertItem(floatDistance, typeNames[floatDistance]);

    //typeCombo->insertItem(17, QCoreApplication::translate("Dialog", "Array column (int)", nullptr));
    //typeCombo->insertItem(18, QCoreApplication::translate("Dialog", "Array row (int)", nullptr));
}

TextFileDialog::TextFileDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);

    skipLines = 0;
    separator = ' ';

    readPresets();
}

#include <QtCore/QFile>
#include <QtCore/QTextStream>

bool TextFileDialog::init(const QString fileName, int lines)
{
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly | QFile::Text))
    {
        QTextStream in(&inputFile);
        int i = 0;
        while (!in.atEnd() && (i < lines))
        {
            tenLines.push_back( in.readLine() );
            i++;
        }
        inputFile.close();
    
        addValueWidgets();

        return true;
    }

    return false;
}

void TextFileDialog::addValueWidgets()
{
    for (QString v : tenLines[skipLines].split(separator))
    {
        valueWidgets.push_back(new MyValueWidget(v, ui.scrollAreaWidgetContents));
        ui.verticalLayout_2->addWidget(valueWidgets.back());
    }

    this->repaint();
}

void TextFileDialog::removeValueWidgets()
{
    for (QWidget* w : valueWidgets)
    {
        delete w;
    }

    valueWidgets.clear();
}

void TextFileDialog::onSepRadio(bool b)
{
    if (ui.radioSpace->isChecked()) separator = ' ';
    else if (ui.radioTab->isChecked()) separator = '\t';
    else if (ui.radioSemi->isChecked()) separator = ';';
    else if (ui.radioComma->isChecked()) separator = ',';

    removeValueWidgets();
    addValueWidgets();
    
    onPreset(ui.presetsCombo->itemText(currentPreset));

    this->repaint();
}

void TextFileDialog::onLineNb(int i)
{
    skipLines = i<10?i:9;

    removeValueWidgets();
    addValueWidgets();

    onPreset(ui.presetsCombo->itemText(currentPreset));

    this->repaint();
}

void TextFileDialog::onPreset(QString preset)
{
    QStringList psts = preset.split(' ');

    for (int i = 0; i < valueWidgets.size(); i++)
    {
        if ( i < psts.size() )
            valueWidgets[i]->typeCombo->setCurrentIndex(psts[i].toInt());
        else
            valueWidgets[i]->typeCombo->setCurrentIndex(0);
    }

    currentPreset = ui.presetsCombo->currentIndex();
}

#include "AP.h"
#include "MainApplication.h"

void TextFileDialog::storePreset(const QString preset)
{
    QStringList presets = AP::mainApp().settings->value("textFileFormatPresets").toStringList();
    presets.removeAll(preset);
    presets.prepend(preset);
    while (presets.size() > 10)
        presets.removeLast();
    AP::mainApp().settings->setValue("textFileFormatPresets", presets);
}

void TextFileDialog::storeCurrentPreset()
{
    QString preset("");

    for (int i = 0; i < valueWidgets.size(); i++)
    {
        preset.append(QString::number(valueWidgets[i]->typeCombo->currentIndex()) + " ");
    }
    storePreset(preset.trimmed());
}

void TextFileDialog::readPresets() {
    QStringList presets = AP::mainApp().settings->value("textFileFormatPresets").toStringList();

    auto itEnd = 0;
    if (presets.size() <= 10)
        itEnd = presets.size();
    else
        itEnd = 10;

    ui.presetsCombo->clear();

    for (auto i = 0; i < itEnd; ++i)
    {
        ui.presetsCombo->addItem(presets[i]);
    }

    currentPreset = 0;
    if (!presets.empty()) onPreset(presets[0]);
}
