#include "ctreeviewcontextmenu.h"
#include "qapplication.h"
#include <QClipboard>
CTreeViewContextMenu::CTreeViewContextMenu(QTreeView *view, CNetworkAdapter *adapter)
{
    myTreeView = view;
    myNetworkAdapter = adapter;
    myTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    myTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    myTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(myTreeView, &QTreeView::customContextMenuRequested, this, &CTreeViewContextMenu::showContextMenu);
}

void CTreeViewContextMenu::showContextMenu(const QPoint &pos)
{
    qDebug() << "Hello from context menu\n";
    QMenu contextMenu(tr("Context Menu"));
    contextMenu.setStyleSheet("QMenu { background-color: rgb(33, 37, 43); color : white; }"
                              "QMenu::item:selected { background-color: rgb(26, 58, 85);} "
                              "QMenu::separator { background-color: white; height: 1px; margin: 5px 0px 5px 0px; }");

    QFont font("Comic Sans MS", 12); // create a QFont object with the desired font properties
    contextMenu.setFont(font);       // set the font for the menu

    QAction actionCopySelectedValue("Copy selected", this);

    connect(&actionCopySelectedValue, &QAction::triggered, this, &CTreeViewContextMenu::copySelectedValue);
    contextMenu.addAction(&actionCopySelectedValue);

    QAction actionCopyAll("Copy all", this);
    connect(&actionCopyAll, &QAction::triggered, this, &CTreeViewContextMenu::copyAll);

    QAction refresh("Refresh", this);
    connect(&refresh, &QAction::triggered, this, &CTreeViewContextMenu::refresh);

    QAction onlyValue("Copy only value", this);
    connect(&onlyValue, &QAction::triggered, this, &CTreeViewContextMenu::copyValueItem);

    contextMenu.addAction(&actionCopyAll);
    contextMenu.addAction(&onlyValue);
    contextMenu.addSeparator(); // add a separator line
    contextMenu.addAction(&refresh);

    contextMenu.exec(myTreeView->mapToGlobal(pos));
}

void CTreeViewContextMenu::copySelectedValue()
{
    QModelIndexList selectedIndexes = myTreeView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
        return;

    QString textToCopy;
    QModelIndex prevIndex; // to keep track of the previous index
    foreach (QModelIndex index, selectedIndexes)
    {
        if (index.column() == 1) // only copy values from the second column
        {
            if (prevIndex.isValid())
            {
                QString fieldName = prevIndex.data(Qt::DisplayRole).toString();
                QString fieldValue = index.data(Qt::DisplayRole).toString();
                textToCopy += fieldName + " -> " + fieldValue + "\n";
            }
        }
        prevIndex = index;
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(textToCopy);
}
void CTreeViewContextMenu::copyAll()
{
    QString text;

    // Get the model from the tree view
    QAbstractItemModel *model = myTreeView->model();

    // Iterate over each root index in the model
    for (int rootRow = 0; rootRow < model->rowCount(); ++rootRow)
    {
        QModelIndex rootIndex = model->index(rootRow, 0);
        text += rootIndex.data().toString() + "\n";
        // Iterate through each row in the root index
        for (int row = 0; row < model->rowCount(rootIndex); ++row)
        {
            // Iterate through each column in the root index
            for (int column = 0; column < model->columnCount(rootIndex); ++column)
            {
                // Get the index for this row and column
                QModelIndex index = model->index(row, column, rootIndex);

                // Get the data for this index
                QString value = model->data(index).toString();

                // Append the value to the text string, separated by a tab character
                text += value + "\t";
            }

            // Add a newline character to separate rows
            text += "\n";
        }
    }

    // Copy the text to the clipboard
    QApplication::clipboard()->setText(text);
}

void CTreeViewContextMenu::refresh()
{
    emit refreshClicked();
}

void CTreeViewContextMenu::copyValueItem()
{
    QString text;
    QItemSelectionModel *selectionModel = myTreeView->selectionModel();

    // Get the selected index
    QModelIndex index = selectionModel->currentIndex();

    // Get the value from the model data
    QString value = myTreeView->model()->data(index).toString();

    // Append the value to the text string
    text += value;

    // Copy the text to the clipboard
    QApplication::clipboard()->setText(text);
}
