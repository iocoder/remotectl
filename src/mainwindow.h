#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qtermwidget5/qtermwidget.h>

#include "connectionmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    ConnectionManager *connection;
    QLineEdit *txtAddress;

    void refreshTree(QTreeWidgetItem *item);
    void attachConsoleActions(QTermWidget *console, int tabNo);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setConnection(ConnectionManager *_connection);
    void openDirectory(QString dir);
    void openTerm();
    void openEditor(QString path, QString fname);
    void openTop();

private slots:
    void on_lstDir_itemExpanded(QTreeWidgetItem *item);
    void on_actionGo_triggered();
    void on_actionTerminal_triggered();
    void on_tabView_tabCloseRequested(int index);
    void on_actionOpenFile_triggered();
    void on_lstDir_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_actionTop_triggered();
    void on_actionShowHidden_triggered(bool checked);
    void on_actionRefresh_triggered();
    void on_actionReconnect_triggered();
    void on_actionNewSession_triggered();
    void on_actionQuitSession_triggered();
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
