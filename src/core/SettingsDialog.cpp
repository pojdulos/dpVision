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



void applyFontToAllWidgets(const QFont& font)
{
    qApp->setFont(font);

    // Przelicz wszystkie wid¿ety w aplikacji:
    for (QWidget* widget : qApp->allWidgets())
    {
        widget->setPalette(qApp->palette());
        widget->setAutoFillBackground(true);         // potrzebne, ¿eby t³o siê zmieni³o
        widget->setAttribute(Qt::WA_StyledBackground, true);

        // pomijamy g³ówne okno i dialogi
        if (qobject_cast<QMainWindow*>(widget) || qobject_cast<QDialog*>(widget))
            continue;

        widget->setFont(font);           // dla pewnoœci: przypisz font jeszcze raz
        widget->updateGeometry();        // przelicz minimalne wymiary
        widget->adjustSize();            // dopasuj do treœci
        widget->update();                // odœwie¿
    }
}

void SettingsDialog::applyDarkPalette()
{
    QPalette dark;
    dark.setColor(QPalette::Window, QColor(53, 53, 53));
    dark.setColor(QPalette::WindowText, Qt::white);
    dark.setColor(QPalette::Base, QColor(42, 42, 42));
    dark.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    dark.setColor(QPalette::ToolTipBase, Qt::white);
    dark.setColor(QPalette::ToolTipText, Qt::white);
    dark.setColor(QPalette::Text, Qt::white);
    dark.setColor(QPalette::Button, QColor(53, 53, 53));
    dark.setColor(QPalette::ButtonText, Qt::white);
    dark.setColor(QPalette::BrightText, Qt::red);
    dark.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
    dark.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(dark);
}

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
    //connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        QFont chosenFont = fontComboBox->currentFont();
        chosenFont.setPointSize(fontSizeSpinBox->value());

        // Styl ciemny/jasny:
        if (darkModeCheckBox->isChecked())
            applyDarkPalette();
        else
            qApp->setPalette(style()->standardPalette());
        
        applyFontToAllWidgets(chosenFont);
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
    formLayout->addRow(tr("Domyœlna czcionka:"), fontComboBox);
    formLayout->addRow(tr("Rozmiar czcionki:"), fontSizeSpinBox);
    layout->addLayout(formLayout);

    darkModeCheckBox = new QCheckBox(tr("Tryb ciemny"), this);
    darkModeCheckBox->setChecked(false); // domyœlnie jasny
    layout->addWidget(darkModeCheckBox);


    // Zapisz wskaŸnik, jeœli chcesz go u¿yæ póŸniej np. w metodzie accept()

    //layout->addWidget(new QLabel("Tutaj pojawi¹ siê ogólne ustawienia."));
    tabWidget->addTab(generalTab, tr("General"));

    // Tu mo¿na ³atwo dodaæ kolejne zak³adki
    // QWidget *advancedTab = new QWidget;
    // tabWidget->addTab(advancedTab, tr("Zaawansowane"));
}
