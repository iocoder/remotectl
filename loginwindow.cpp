#include "version.h"
#include "settingsmanager.h"
#include "connectionmanager.h"
#include "mainwindow.h"
#include "loginwindow.h"
#include "ui_loginwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopWidget>

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    SettingsManager *settings = SettingsManager::getSettingsManager();
    ui->setupUi(this);
    /* icon */
    setWindowIcon(QIcon::fromTheme("preferences-system-network"));
    /* read session history */
    settings->getSessionList(ui->lstOpen);
    if (ui->lstOpen->count() == 0) {
        ui->radioOpen->setDisabled(true);
    } else {
        QList<QListWidgetItem *> list = ui->lstOpen->findItems(settings->getLastSession(), Qt::MatchExactly);
        if (list.count() > 0) {
            /* we found the session */
            ui->radioNew->setChecked(false);
            ui->radioOpen->setChecked(true);
            ui->lstOpen->setCurrentItem(list.first());
        }
    }
    /* center */
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->desktop()->availableGeometry()
        )
    );
    /* form header */
    ui->lblHeader->setText(QString(PROGNAME)+" "+PROGVERSION);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_radioNew_toggled(bool checked)
{
    /* enable/disable frmNew based on button's status */
    ui->frmNew->setEnabled(checked);
}

void LoginWindow::on_radioOpen_toggled(bool checked)
{
    /* enable/disable frmOpen based on button's status */
    ui->frmOpen->setEnabled(checked);
    ui->btnDelete->setEnabled(checked);
    if (checked) {
        ui->lstOpen->setCurrentRow(0);
    } else {
        ui->lstOpen->setCurrentRow(-1);
    }
}

void LoginWindow::on_btnQuit_pressed()
{
    close();
}

void LoginWindow::on_btnLogin_pressed()
{
    QMessageBox msgBox;
    SettingsManager *settings = SettingsManager::getSettingsManager();
    ConnectionManager *connection;
    QString name, host, port, user, dir, pass;
    MainWindow *winmain;
    bool ok;

    /* make sure text boxes are not empty */
    if ( !ui->txtId->text().compare("") )
    {
        msgBox.setWindowTitle("Error");
        msgBox.setText("Identifier text box is empty!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    if ( !ui->txtHost->text().compare("") )
    {
        msgBox.setWindowTitle("Error");
        msgBox.setText("Host text box is empty!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    if ( !ui->txtPort->text().compare("") )
    {
        msgBox.setWindowTitle("Error");
        msgBox.setText("Port text box is empty!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    if ( !ui->txtUser->text().compare("") )
    {
        msgBox.setWindowTitle("Error");
        msgBox.setText("User text box is empty!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    if ( !ui->txtDir->text().compare("") )
    {
        msgBox.setWindowTitle("Error");
        msgBox.setText("Directory text box is empty!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    /* read session information */
    name = ui->txtId->text();
    host = ui->txtHost->text();
    port = ui->txtPort->text();
    user = ui->txtUser->text();
    dir  = ui->txtDir->text();

    /* add information to settings manager */
    settings->addSession(name, host, port, user, dir);

    /* reload list */
    settings->getSessionList(ui->lstOpen);
    if (ui->lstOpen->count() != 0)
    {
        ui->radioOpen->setDisabled(false);
    }

    /* ask for password */
    pass = QInputDialog::getText(this,
                                 tr("Login"),
                                 tr("Enter password:"),
                                 QLineEdit::Password,
                                 "",
                                 &ok);

    /* create new connection object */
    connection = new ConnectionManager(name, host, port, user, pass);

    /* no connect? */
    if (!ok or !connection->start())
    {
        /* connection failed? */
        if (ok)
        {
            msgBox.setWindowTitle("Error");
            msgBox.setText("Connection failed!");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
        /* deallocate connection */
        delete connection;
        /* select new Session */
        ui->radioNew->setChecked(true);
        ui->radioOpen->setChecked(false);
    }
    else
    {
        /* set as last session */
        settings->setLastSession(name);
        /* open new MainWindow */
        winmain = new MainWindow();
        winmain->show();
        /* set winmain parameters */
        winmain->setConnection(connection);
        winmain->openDirectory(dir);
        winmain->openTerm();
        /* close this form */
        close();
    }

}

void LoginWindow::on_lstOpen_currentRowChanged(int currentRow)
{
    QString name, host, port, user, dir;
    SettingsManager *settings = SettingsManager::getSettingsManager();

    /* retrieve information */
    if (currentRow >= 0) {
        name = ui->lstOpen->item(currentRow)->text();
        settings->getSession(name, &host, &port, &user, &dir);
        ui->txtId->setText(name);
        ui->txtHost->setText(host);
        ui->txtPort->setText(port);
        ui->txtUser->setText(user);
        ui->txtDir->setText(dir);
    }
}

void LoginWindow::on_btnDelete_pressed()
{
    QString name;
    SettingsManager *settings = SettingsManager::getSettingsManager();
    name = ui->lstOpen->currentItem()->text();
    settings->delSession(name);
    settings->getSessionList(ui->lstOpen);
    if (ui->lstOpen->count() == 0) {
        ui->radioOpen->setDisabled(true);
        ui->radioOpen->setChecked(false);
        ui->radioNew->setChecked(true);
    }
}
