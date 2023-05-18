#include "cviewcontextmenu.h"
#include "qapplication.h"
#include <QClipboard>
#include <QProcess>
#include <QToolTip>
CViewContextMenu::CViewContextMenu(QTreeView *view, CNetworkAdapter *adapter)
{
    mView = view;

    myNetworkAdapter = adapter;
    mView->setContextMenuPolicy(Qt::CustomContextMenu);

    mView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mView->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(mView, &QAbstractItemView::customContextMenuRequested, this, &CViewContextMenu::showContextMenu);

    connect(mView, &QAbstractItemView::doubleClicked, this,
            [](const QModelIndex &index) { QProcess::startDetached("control.exe", QStringList() << "ncpa.cpl"); });
}

CViewContextMenu::CViewContextMenu(QTableView *view, CNetworkAdapter *adapter)
{
    mView = view;
    myNetworkAdapter = adapter;
    mView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(view, &QTableView::customContextMenuRequested, this, [=](const QPoint &pos) {
        QMenu contextMenu(tr("Context Menu"));
        contextMenu.setStyleSheet(style);
        QFont font("Comic Sans MS", 12);
        contextMenu.setFont(font);

        QAction actionCopySelectedValue(tr("Copy selected"), this);
        QAction actionCopyAll(tr("Copy all"), this);
        QAction refresh(tr("Refresh"), this);
        connect(&refresh, &QAction::triggered, this, &CViewContextMenu::refresh);
        connect(&actionCopyAll, &QAction::triggered, this, &CViewContextMenu::copyAllTableValue);
        connect(&actionCopySelectedValue, &QAction::triggered, this, &CViewContextMenu::copyAllSelectedRow);

        contextMenu.addAction(&actionCopyAll);
        contextMenu.addAction(&actionCopySelectedValue);
        contextMenu.addSeparator(); // add a separator line
        contextMenu.addAction(&refresh);

        contextMenu.exec(mView->mapToGlobal(pos));
    });
}

void CViewContextMenu::showContextMenu(const QPoint &pos)
{
    qDebug() << "Hello from context menu\n";
    QMenu contextMenu(tr("Context Menu"));

    contextMenu.setStyleSheet(style);

    QFont font("Comic Sans MS", 12); // create a QFont object with the desired font properties
    contextMenu.setFont(font);       // set the font for the menu

    QAction actionCopySelectedValue(tr("Copy selected"), this);

    connect(&actionCopySelectedValue, &QAction::triggered, this, &CViewContextMenu::copySelectedValue);
    contextMenu.addAction(&actionCopySelectedValue);

    QAction actionCopyAll(tr("Copy all"), this);
    connect(&actionCopyAll, &QAction::triggered, this, &CViewContextMenu::copyAll);

    QAction refresh(tr("Refresh"), this);
    connect(&refresh, &QAction::triggered, this, &CViewContextMenu::refresh);

    QAction onlyValue(tr("Copy only value"), this);
    connect(&onlyValue, &QAction::triggered, this, &CViewContextMenu::copyValueItem);

    contextMenu.addAction(&actionCopyAll);
    contextMenu.addAction(&onlyValue);
    contextMenu.addSeparator(); // add a separator line
    contextMenu.addAction(&refresh);

    contextMenu.exec(mView->mapToGlobal(pos));
}

void CViewContextMenu::copySelectedValue()
{
    QModelIndexList selectedIndexes = mView->selectionModel()->selectedIndexes();
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

void CViewContextMenu::copyAllSelectedRow()
{
    QModelIndexList selectedIndexes = mView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
        return;

    QModelIndex index = selectedIndexes.first();
    int row = index.row();
    int columnCount = mView->model()->columnCount(index.parent());
    QStringList rowData;
    for (int column = 0; column < columnCount; ++column)
    {
        QModelIndex cellIndex = mView->model()->index(row, column, index.parent());
        QString data = mView->model()->data(cellIndex).toString();
        rowData << data;
    }
    QString clipboardText = rowData.join("\t");
    QApplication::clipboard()->setText(clipboardText);
}

void CViewContextMenu::copyAll()
{
    QString text;

    // Get the model from the tree view
    QAbstractItemModel *model = mView->model();

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

void CViewContextMenu::refresh()
{
    emit refreshClicked();
}

void CViewContextMenu::copyValueItem()
{
    QString text;
    QItemSelectionModel *selectionModel = mView->selectionModel();

    // Get the selected index
    QModelIndex index = selectionModel->currentIndex();

    // Get the value from the model data
    QString value = mView->model()->data(index).toString();

    // Append the value to the text string
    text += value;

    // Copy the text to the clipboard
    QApplication::clipboard()->setText(text);
}

void CViewContextMenu::copyAllTableValue()
{
    QAbstractItemModel *model = mView->model();
    if (!model)
    {
        return;
    }
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();
    QStringList headers;
    for (int column = 0; column < columnCount; ++column)
    {
        headers << model->headerData(column, Qt::Horizontal).toString();
    }
    QStringList rows;
    for (int row = 0; row < rowCount; ++row)
    {
        QStringList rowData;
        for (int column = 0; column < columnCount; ++column)
        {
            QModelIndex index = model->index(row, column);
            QString data = model->data(index).toString();
            rowData << data;
        }
        rows << rowData.join("\t");
    }
    QString clipboardText = headers.join("\t") + "\n" + rows.join("\n");
    QApplication::clipboard()->setText(clipboardText);
}
