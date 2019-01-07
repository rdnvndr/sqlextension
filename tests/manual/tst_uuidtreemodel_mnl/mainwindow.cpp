#include "mainwindow.h"

#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + "/tree.db");
    db.open();

    m_data = new SqlTreeData(this);
    m_data->setDatabase(db);
    m_data->setTable("utags");
	QStringList columns;
    columns  << "name";
    m_data->setFields( "id", "parent", columns );

    m_model = new UuidTreeModel(m_data, this);
    m_treeView = new QTreeView( this );
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setModel(m_model);

    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeView, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showTreeViewContextMenu(const QPoint&)) );

	setWindowTitle(tr("SQL Tree Model"));
    setCentralWidget(m_treeView);

    m_insertAction = new QAction(tr("Insert"), this);
    m_removeAction = new QAction(tr("Remove"), this);
    m_treeMenu = new QMenu(m_treeView);
    m_treeMenu->addAction(m_insertAction);
    m_treeMenu->addAction(m_removeAction);
}

MainWindow::~MainWindow()
{

}

void MainWindow::showTreeViewContextMenu( const QPoint& pos )
{
    QModelIndex index = m_treeView->indexAt(pos);
    m_removeAction->setEnabled( index.isValid() );
	
    QAction* action = m_treeMenu->exec( m_treeView->viewport()->mapToGlobal(pos) );
    if ( action == m_insertAction )
		insertRow(index);
    else if ( action == m_removeAction )
		removeRow(index);
}

void MainWindow::insertRow( const QModelIndex& index )
{
    m_model->insertRow( 0, index );
}

void MainWindow::removeRow( const QModelIndex& index )
{
    m_model->removeRow( index.row(), index.parent() );
}
