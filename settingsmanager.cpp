#include "settingsmanager.h"
#include <QSettings>
#include <QListWidget>

SettingsManager *SettingsManager::singleton = nullptr;

SettingsManager::SettingsManager()
{
     qsettings = new QSettings("MyCompany", "RemoteCtl");
}

void SettingsManager::getSessionList(QListWidget *list)
{
    /* remove elements in list */
    list->clear();
    /* add stored sessions */
    for (int i = 0; i < qsettings->childGroups().size(); ++i) {
        list->addItem(qsettings->childGroups().at(i));
    }
}

void SettingsManager::addSession(QString name, QString host, QString port, QString user, QString dir)
{
    qsettings->beginGroup(name);
    qsettings->setValue("host", host);
    qsettings->setValue("port", port);
    qsettings->setValue("user", user);
    qsettings->setValue("dir", dir);
    qsettings->endGroup();
}

void SettingsManager::getSession(QString name, QString *host, QString *port, QString *user, QString *dir)
{
    qsettings->beginGroup(name);
    *host = qsettings->value("host").toString();
    *port = qsettings->value("port").toString();
    *user = qsettings->value("user").toString();
    *dir  = qsettings->value("dir" ).toString();
    qsettings->endGroup();
}

void SettingsManager::delSession(QString name) {
    qsettings->remove(name);
}

void SettingsManager::setLastSession(QString name) {
    qsettings->setValue("lastSession", name);
}

QString SettingsManager::getLastSession() {
    return qsettings->value("lastSession").toString();
}

SettingsManager *SettingsManager::getSettingsManager()
{
    if (singleton == nullptr)
    {
        singleton = new SettingsManager();
    }
    return singleton;
}
