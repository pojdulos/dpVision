#include "SettingsDialog.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QApplication>
#include <QMainWindow>
#include <QFontComboBox>
#include <QFormLayout>
#include <QCheckBox>

#include "AppSettings.h"


SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    resize(400, 300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    tabWidget = new QTabWidget(this);
    setupTabs();
    mainLayout->addWidget(tabWidget);



    // Dodaj przyciski OK / Anuluj na dole
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        // Styl ciemny/jasny:
        AppSettings::setDarkMode(darkModeCheckBox->isChecked());
        
        QFont chosenFont = fontComboBox->currentFont();
        chosenFont.setPointSize(fontSizeSpinBox->value());
        AppSettings::setFont(chosenFont);
        });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

void SettingsDialog::setupTabs()
{
    QWidget* generalTab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(generalTab);

    fontComboBox = new QFontComboBox(this);
    fontComboBox->setCurrentFont(font());

    fontSizeSpinBox = new QSpinBox(this);
    fontSizeSpinBox->setRange(6, 48);
    fontSizeSpinBox->setValue(font().pointSize());  // aktualny rozmiar czcionki

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Default font style:"), fontComboBox);
    formLayout->addRow(tr("Default font size: "), fontSizeSpinBox);
    layout->addLayout(formLayout);

    darkModeCheckBox = new QCheckBox(tr("Dark mode"), this);
    darkModeCheckBox->setChecked(AppSettings::isDarkMode()); // domyœlnie jasny
    layout->addWidget(darkModeCheckBox);


    // Zapisz wskaŸnik, jeœli chcesz go u¿yæ póŸniej np. w metodzie accept()

    //layout->addWidget(new QLabel("Tutaj pojawi¹ siê ogólne ustawienia."));
    tabWidget->addTab(generalTab, tr("General settings"));

    // Tu mo¿na ³atwo dodaæ kolejne zak³adki
    // QWidget *advancedTab = new QWidget;
    // tabWidget->addTab(advancedTab, tr("Zaawansowane"));
}
