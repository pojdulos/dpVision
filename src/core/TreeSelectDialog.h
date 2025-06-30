#pragma once

#include <QDialog>
#include <QTreeWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>

class CBaseObject;

class QStandardItem;
class QStandardItemModel;



class TreeSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TreeSelectDialog(CBaseObject* obj, QWidget* parent = nullptr);

    // Dodawanie elementu: tekst i uchwyt do obiektu (np. wskaŸnik)
    void addItem(const QString& label, CBaseObject* objectHandle, QTreeWidgetItem* parent = nullptr);

    // Pobierz uchwyt do wybranego obiektu (po zamkniêciu dialogu)
    
    inline CBaseObject* selectedHandle() const { return selectedObjectHandle; }
    inline QString getAction() const { return action; };
    inline bool keepPosition() const { return keep_pos; };
    
    void copyItemToWidget(QStandardItem* srcItem, QTreeWidgetItem* parentWidgetItem);

    void cloneModelToWidget(QStandardItemModel* srcModel);

    void onItemSelectionChanged();


private slots:
    void onActionClicked();

private:
    CBaseObject* m_obj;
    QTreeWidget* treeWidget;
    QPushButton* moveButton;
    QPushButton* copyButton;
    QPushButton* rearrangeButton;
    QPushButton* cancelButton;
    QCheckBox* keepPositionCheckBox;
    CBaseObject* selectedObjectHandle;
    QString action;
    bool keep_pos;
};
