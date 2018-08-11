#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QListWidget>
#include <QSettings>

class SettingsManager
{
private:
    static SettingsManager *singleton;
    QSettings *qsettings;

public:
    SettingsManager();
    void getSessionList(QListWidget *list);
    void addSession(QString name, QString host, QString port, QString user, QString dir);
    void getSession(QString name, QString *host, QString *port, QString *user, QString *dir);
    void delSession(QString name);
    void setLastSession(QString name);
    QString getLastSession();
    static SettingsManager *getSettingsManager();
};

#endif // SETTINGSMANAGER_H
