#include "TreeSelectDialog.h"
#include <QVariant>

#include "BaseObject.h"

TreeSelectDialog::TreeSelectDialog(std::shared_ptr<CBaseObject> obj, QWidget* parent)
    : QDialog(parent), m_obj(obj), selectedObjectHandle(nullptr)
{
    setWindowTitle(QString::fromUtf8("Object repositioning"));

    treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabel("Select target object");

    moveButton = new QPushButton(QString::fromUtf8("Move to selected"), this);
    moveButton->setObjectName("move");
    copyButton = new QPushButton(QString::fromUtf8("Copy to selected"), this);
    copyButton->setObjectName("copy");
    rearrangeButton = new QPushButton(QString::fromUtf8("Set as subtree of selected"), this);
    rearrangeButton->setObjectName("rearrange");
    
    if (! obj->hasCategory(CBaseObject::Category::OBJECT) )
        rearrangeButton->setEnabled(false);
    
    keepPositionCheckBox = new QCheckBox("Keep current position (will add an alignment transformation if needed)");
    keepPositionCheckBox->setChecked(false);

    cancelButton = new QPushButton("Cancel", this);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(moveButton);
    btnLayout->addWidget(copyButton);
    btnLayout->addWidget(rearrangeButton);
    btnLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(treeWidget);
    mainLayout->addWidget(keepPositionCheckBox);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);

    connect(moveButton, &QPushButton::clicked, this, &TreeSelectDialog::onActionClicked);
    connect(copyButton, &QPushButton::clicked, this, &TreeSelectDialog::onActionClicked);
    connect(rearrangeButton, &QPushButton::clicked, this, &TreeSelectDialog::onActionClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(treeWidget, &QTreeWidget::itemSelectionChanged, this, &TreeSelectDialog::onItemSelectionChanged);

}

void TreeSelectDialog::addItem(const QString& label, std::shared_ptr<CBaseObject> objectHandle, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, label);
    // Przechowuj uchwyt jako QVariant (konwersja do/quintptr)
    item->setData(0, Qt::UserRole, QVariant::fromValue<std::shared_ptr<CBaseObject>>(objectHandle));

    if (parent)
        parent->addChild(item);
    else
        treeWidget->addTopLevelItem(item);
}

void TreeSelectDialog::onActionClicked()
{
    QTreeWidgetItem* current = treeWidget->currentItem();
    if (!current) return; // brak wyboru

    selectedObjectHandle = current->data(0, Qt::UserRole).value<std::shared_ptr<CBaseObject>>();
    action = sender()->objectName();
    keep_pos = keepPositionCheckBox->isChecked();

    accept();
}

#include "QStandardItem"

void TreeSelectDialog::copyItemToWidget(QStandardItem* srcItem, QTreeWidgetItem* parentWidgetItem)
{
    // Skopiuj etykietę (kolumnę 0)
    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setText(0, srcItem->text());

    // Skopiuj UserRole (i inne role, jeśli trzeba)
    QVariant userData = srcItem->data(Qt::UserRole);
    newItem->setData(0, Qt::UserRole, userData);

    std::shared_ptr<CBaseObject> obj = userData.value<std::shared_ptr<CBaseObject>>();

    if ((obj==nullptr) || (obj == m_obj)||(obj->hasCategory(CBaseObject::Category::ANNOTATION) && m_obj->hasCategory(CBaseObject::Category::OBJECT))) {
        //newItem->setFlags(newItem->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEditable));
        newItem->setFlags(newItem->flags() & ~Qt::ItemIsEnabled);
        newItem->setToolTip(0, "No action can be performed for this element");
    }

    // Dodaj nowy item do rodzica lub jako top-level
    if (parentWidgetItem)
        parentWidgetItem->addChild(newItem);
    else
        this->treeWidget->addTopLevelItem(newItem);

    // Rekurencyjnie kopiuj dzieci
    for (int i = 0; i < srcItem->rowCount(); ++i)
    {
        QStandardItem* child = srcItem->child(i, 0); // zakładamy jedną kolumnę
        if (child)
            copyItemToWidget(child, newItem);
    }
}

void TreeSelectDialog::cloneModelToWidget(QStandardItemModel* srcModel)
{
    QTreeWidgetItem* rootItem = new QTreeWidgetItem();
    rootItem->setText(0, "Workspace root");
    rootItem->setData(0, Qt::UserRole, QVariant::fromValue(nullptr));

    this->treeWidget->clear();
    this->treeWidget->addTopLevelItem(rootItem);

    for (int i = 0; i < srcModel->rowCount(); ++i)
    {
        QStandardItem* item = srcModel->item(i, 0); // kolumna 0
        if (item)
            copyItemToWidget(item, rootItem);
    }

    this->treeWidget->expandAll();
}

void TreeSelectDialog::onItemSelectionChanged()
{
    QList<QTreeWidgetItem*> selected = treeWidget->selectedItems();
    if (!selected.isEmpty()) {
        QTreeWidgetItem* item = selected.first();

        selectedObjectHandle = item->data(0, Qt::UserRole).value<std::shared_ptr<CBaseObject>>();

        bool disable = selectedObjectHandle && (CBaseObject::getRoot(selectedObjectHandle) == CBaseObject::getRoot(m_obj));

        rearrangeButton->setDisabled(disable);
    }
}
