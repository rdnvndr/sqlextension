#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeView>
#include <QMainWindow>

#include "threadmodel.h"

//! Главное окно приложения
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    //! Конструктор класса
    MainWindow(QWidget *parent = nullptr);

    //! Деструктор класса
    ~MainWindow();

private:
    //! Дерево
    QTreeView   *m_treeView;

    //! Модель
    ThreadModel *m_model;
};

#endif // MAINWINDOW_H
