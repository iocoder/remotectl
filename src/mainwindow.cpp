#include "version.h"
#include "connectionmanager.h"
#include "loginwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>
#include <QDesktopWidget>
#include <QResource>
#include <qtermwidget5/qtermwidget.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QLabel *lblAddress = new QLabel(QString("Address:   "));
    QAction *action;
    /* setup ui */
    ui->setupUi(this);
    /* icon */
    setWindowIcon(QIcon(":/icons/remotectl.png"));
    /* set splitter sizes */
    QList<int> Sizes;
    Sizes.append(0.2 * sizeHint().height());
    Sizes.append(0.8 * sizeHint().height());
    ui->splitter->setSizes(Sizes);
    /* center */
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->desktop()->availableGeometry()
        )
    );
    /* qtreeview header */
    ui->lstDir->headerItem()->setText(0, "File Hierarchy");
    /* tab view */
    ui->tabView->clear();
    /* add address */
    txtAddress = new QLineEdit();
    action = ui->barAddress->addWidget(lblAddress);
    action = ui->barAddress->addWidget(txtAddress);
    action->setVisible(true);
    ui->barAddress->addAction(ui->actionGo);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setConnection(ConnectionManager *_connection) {
    /* set connection */
    connection = _connection;
    /* set title */
    setWindowTitle(connection->getName() + " - " + PROGNAMELOW);
}

void MainWindow::openDirectory(QString dir) {
    QMessageBox msgBox;
    /* update text address */
    txtAddress->setText(dir);
    /* add directory name */
    QTreeWidgetItem *topDir = new QTreeWidgetItem();
    topDir->setText(0, dir);
    topDir->setIcon(0, QIcon::fromTheme("folder-remote"));
    topDir->setText(1, "D");
    topDir->setText(2, dir);
    /* add to view */
    ui->lstDir->clear();
    ui->lstDir->addTopLevelItem(topDir);
    /* get sub list */
    topDir->setExpanded(true);
}

void MainWindow::refreshTree(QTreeWidgetItem *item) {
    /* refresh tree view */
    QMessageBox msgBox;
    while (item->childCount() > 0)
    {
        item->removeChild(item->child(0));
    }
    if (!connection->getDirectoryListing(item->text(2), item))
    {
        msgBox.setWindowTitle("Error");
        msgBox.setText("Cannot open directory!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
    item->sortChildren(0, Qt::SortOrder::AscendingOrder);
    item->sortChildren(1, Qt::SortOrder::AscendingOrder);
}

void MainWindow::attachConsoleActions(QTermWidget *console, int tabNo) {
    /* console menu */
    QMenu *consoleMenu = new QMenu(console);
    consoleMenu->addAction(ui->actionCopyText->icon(), ui->actionCopyText->text());
    consoleMenu->addAction(ui->actionPasteText->icon(), ui->actionPasteText->text());
    consoleMenu->addAction(ui->actionKeyStroke->icon(), ui->actionKeyStroke->text());
    consoleMenu->addSeparator();
    consoleMenu->addAction(ui->actionScrollbar->icon(), ui->actionScrollbar->text());
    consoleMenu->addAction(ui->actionColorScheme->icon(), ui->actionColorScheme->text());
    consoleMenu->addAction(ui->actionFont->icon(), ui->actionFont->text());
    consoleMenu->addSeparator();
    consoleMenu->addAction(ui->actionCloseTab->icon(), ui->actionCloseTab->text());
    /* copy */
    connect(consoleMenu->actions().at(0), &QAction::triggered,
            this, [=]() -> void {
                console->copyClipboard();
            });
    /* paste */
    connect(consoleMenu->actions().at(1), &QAction::triggered,
            this, [=]() -> void {
                console->pasteClipboard();
            });
    /* add menu to console */
    console->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(console, &QTermWidget::customContextMenuRequested,
            this, [=](const QPoint &pt) -> void {
                /* show context menu */
                consoleMenu->exec(console->mapToGlobal(pt));
            });
    /* keyboard shortcuts */
    connect(console, &QTermWidget::termKeyPressed,
            this, [=](const QKeyEvent *key) -> void {
                /* copy? */
                if (key->modifiers() & Qt::AltModifier and
                    key->text().toLower() == "c")
                {
                    console->copyClipboard();
                }
                /* paste? */
                if (key->modifiers() & Qt::AltModifier and
                    key->text().toLower() == "v")
                {
                    console->pasteClipboard();
                }
            });
}

void MainWindow::openTerm() {
    QList<QString>::iterator qstr;
    QStringList qstrlist;
    QStringList env;
    QString dir;
    int tabNo;
    /* get login dir */
    dir = ui->lstDir->topLevelItem(0)->text(2);
    /* open console */
    QTermWidget *console = new QTermWidget(0);
    /* console colors */
    qstrlist = console->availableColorSchemes();
    for (qstr = qstrlist.begin(); qstr != qstrlist.end(); qstr++) {
        /*fprintf(stderr, "val: %s\n", qstr.i->t().toLatin1().data());*/
    }
    console->setColorScheme("GreenOnBlack");
    /* environment variables */
    env.append(QString("SSHHOST=")+connection->getHost());
    env.append(QString("SSHPORT=")+connection->getPort());
    env.append(QString("SSHUSER=")+connection->getUser());
    env.append(QString("SSHPASS=")+connection->getPass());
    env.append(QString("SSHDIR=")+dir);
    env.append(QString("TERM=xterm"));
    console->setEnvironment(env);
    /* add tab */
    tabNo = ui->tabView->addTab(console, ui->actionTerminal->icon(), "ssh:" + dir);
    ui->tabView->setCurrentIndex(tabNo);
    console->setFocus();
    attachConsoleActions(console, tabNo);
    /* login shell */
    console->setShellProgram("/tmp/remotectl_login.sh");
    console->startShellProgram();
}

void MainWindow::openTop() {
    QStringList env;
    int tabNo;
    /* open console */
    QTermWidget *console = new QTermWidget(0);
    /* console colors */
    console->setColorScheme("GreenOnBlack");
    /* environment variables */
    env.append(QString("SSHHOST=")+connection->getHost());
    env.append(QString("SSHPORT=")+connection->getPort());
    env.append(QString("SSHUSER=")+connection->getUser());
    env.append(QString("SSHPASS=")+connection->getPass());
    env.append(QString("TERM=xterm"));
    console->setEnvironment(env);
    /* add tab */
    tabNo = ui->tabView->addTab(console, ui->actionTop->icon(), "top");
    ui->tabView->setCurrentIndex(tabNo);
    console->setFocus();
    attachConsoleActions(console, tabNo);
    /* login shell */
    console->setShellProgram("/tmp/remotectl_top.sh");
    console->startShellProgram();
}

void MainWindow::openEditor(QString path, QString fname) {
    QTermWidget *console;
    QStringList env;
    int tabNo;
    /* open console */
    console = new QTermWidget(0);
    /* console colors */
    console->setColorScheme("GreenOnBlack");
    /* environment variables */
    env.append(QString("SSHHOST=")+connection->getHost());
    env.append(QString("SSHPORT=")+connection->getPort());
    env.append(QString("SSHUSER=")+connection->getUser());
    env.append(QString("SSHPASS=")+connection->getPass());
    env.append(QString("SSHFILE=")+path);
    env.append(QString("SSHNAME=")+fname);
    env.append(QString("TERM=xterm"));
    console->setEnvironment(env);
    /* add tab */
    tabNo = ui->tabView->addTab(console, connection->fileIcon(fname), fname);
    ui->tabView->setCurrentIndex(tabNo);
    console->setFocus();
    attachConsoleActions(console, tabNo);
    /* login shell */
    console->setShellProgram("/tmp/remotectl_editor.sh");
    console->startShellProgram();
}

void MainWindow::on_tabView_tabCloseRequested(int index)
{
    /* close tab */
    QTermWidget *console = (QTermWidget *) ui->tabView->widget(index);
    //QString killcmd=QString("pkill -P ")+QString(console->getShellPID());
    //system(killcmd.toLatin1().data());
    //ui->tabView->widget(index)->close();
    ui->tabView->removeTab(index);
    delete console;
}

void MainWindow::on_lstDir_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    /* tree view file opened */
    on_actionOpenFile_triggered();
}

void MainWindow::on_lstDir_itemExpanded(QTreeWidgetItem *item)
{
    /* tree view folder opened */
    refreshTree(item);
}

void MainWindow::on_actionGo_triggered()
{
    /* change root directory */
    openDirectory(txtAddress->text());
}

void MainWindow::on_actionTerminal_triggered()
{
    /* just open the terminal */
    openTerm();
}

void MainWindow::on_actionOpenFile_triggered()
{
    QString path;
    QTreeWidgetItem *curWidget;
    QString fname;
    int i;
    /* loop over selected files */
    for (i = 0; i < ui->lstDir->selectedItems().count(); i++) {
        curWidget = ui->lstDir->selectedItems().at(i);
        if (!curWidget->text(1).compare("F")) {
            /* is file */
            fname = curWidget->text(0);
            path  = curWidget->text(2);
            /* open file */
            openEditor(path, fname);
        }
    }
}

void MainWindow::on_actionTop_triggered()
{
    /* just open top */
    openTop();
}

void MainWindow::on_actionShowHidden_triggered(bool checked)
{
    /* toggle hidden files */
    connection->setHidden(checked);
    /* refresh tree view */
    on_actionRefresh_triggered();
}

void MainWindow::on_actionRefresh_triggered()
{
    /* refresh top-level tree widget item */
    refreshTree(ui->lstDir->topLevelItem(0));
}

void MainWindow::on_actionReconnect_triggered()
{
    /* restart connection */
    connection->start();
    /* refresh tree view */
    on_actionRefresh_triggered();
}

void MainWindow::on_actionNewSession_triggered()
{
    /* new session */
    (new LoginWindow())->show();
}

void MainWindow::on_actionQuitSession_triggered()
{
    /* close session */
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    /* about */
    QMessageBox::about(this,
                       QString(PROGNAME) + " " + PROGVERSION,
                       "Copyright (c) 2018 - Mostafa Abdelaziz.");
}
