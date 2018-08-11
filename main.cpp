#include "loginwindow.h"
#include "mainwindow.h"
#include "connectionmanager.h"

#include <QApplication>
#include <QResource>
#include <QInputDialog>

//#define ESCAPE_TO_LOGIN

int main(int argc, char *argv[])
{

    /* local vars */
    LoginWindow *winLogin = nullptr;
    MainWindow  *winMain  = nullptr;
    QResource *resrc;
    FILE *f;
    bool ok;

    /* connection parameters for development */
    QString name = "test";
    QString host = "speech-a-74.pnp.melodis.com";
    QString port = "22";
    QString user = "mostafa";
    QString pass;
    QString dir  = "/disk1/mostafa";
    ConnectionManager *connection;

    /* create terminal login shell script */
    resrc = new QResource(":/scripts/login");
    f = fopen("/tmp/remotectl_login.sh", "w");
    fprintf(f, "%s", resrc->data());
    fclose(f);
    system("chmod +x /tmp/remotectl_login.sh");

    /* create terminal editor shell script */
    resrc = new QResource(":/scripts/editor");
    f = fopen("/tmp/remotectl_editor.sh", "w");
    fprintf(f, "%s", resrc->data());
    fclose(f);
    system("chmod +x /tmp/remotectl_editor.sh");

    /* create terminal top shell script */
    resrc = new QResource(":/scripts/top");
    f = fopen("/tmp/remotectl_top.sh", "w");
    fprintf(f, "%s", resrc->data());
    fclose(f);
    system("chmod +x /tmp/remotectl_top.sh");

    /* define Qt Application */
    QApplication a(argc, argv);

    /* show window */
#ifdef ESCAPE_TO_LOGIN
    /* show password prompt */
    pass = QInputDialog::getText(nullptr,
                                 "Login",
                                 "Enter password:",
                                 QLineEdit::Password,
                                 "",
                                 &ok);
    if (!ok) {
        fprintf(stderr, "Operation canceled!\n");
        return -1;
    }
    /* create new connection */
    connection = new ConnectionManager(name, host, port, user, pass);
    if (!connection->start()) {
        fprintf(stderr, "Connection failed!\n");
        return -1;
    }
    /* create winMain */
    winMain = new MainWindow();
    winMain->setConnection(connection);
    winMain->openDirectory(dir);
    winMain->openTerm();
    winMain->show();
#else
    winLogin = new LoginWindow();
    winLogin->show();
#endif /* ESCAPE_TO_LOGIN */

    /* qt loop */
    return a.exec();

}
