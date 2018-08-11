#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QString>
#include <QTreeWidgetItem>
#include <libssh2.h>

class ConnectionManager
{
private:
    QString _name;
    QString _host;
    QString _port;
    QString _user;
    QString _pass;
    bool _hidden;
    LIBSSH2_SESSION *session;

public:
    ConnectionManager(QString name, QString host, QString port, QString user, QString pass);
    bool start();
    void disconnect();
    bool getDirectoryListing(QString dir, QTreeWidgetItem *qtree);
    QString getName();
    QString getHost();
    QString getPort();
    QString getUser();
    QString getPass();
    void setHidden(bool newVal);
    QIcon fileIcon(const QString &filename);
};

#endif // CONNECTIONMANAGER_H
