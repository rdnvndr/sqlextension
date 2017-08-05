#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeView>
#include <QMenu>
#include <QMainWindow>

#include <sqlextension/uuidtreemodel.h>
#include <sqlextension/sqltreedata.h>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    //! Конструктор класса
	MainWindow(QWidget *parent = 0);

    //! Деструктор класса
    ~MainWindow();

private slots:
    //! Показывает контекстное меню в дереве
    void showTreeViewContextMenu(const QPoint& pos);

    //! Вставляет строку
    void insertRow(const QModelIndex &index);

    //! Удаляет строку
    void removeRow(const QModelIndex &index);

private:
    //! Узел дерева
    SqlTreeData   *m_data;

    //! Модель дерева
    UuidTreeModel *m_model;

    //! Дерево
    QTreeView     *m_treeView;

    //! Контекстное меню дерева
    QMenu         *m_treeMenu;

    //! Команда вставки строки
    QAction       *m_insertAction;

    //! Команда удаления строки
    QAction       *m_removeAction;

};

#endif // MAINWINDOW_H
