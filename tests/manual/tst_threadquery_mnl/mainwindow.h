#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sqlextension/threadquery.h>

using namespace RTPTechGroup::SqlExtension;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_runPushButton_clicked();

    void on_stopPushButton_clicked();

private:
    Ui::MainWindow *ui;
    ThreadQuery *m_threadTest;
};

#endif // MAINWINDOW_H
