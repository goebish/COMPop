#pragma once

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QAction>

#define POLLING_PERIOD 2500

class compop : public QObject
{
    Q_OBJECT

public:
    compop(QObject *parent);
    ~compop();

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu; 
    QTimer *timer;
    bool firstRun;
    QPointer<QAction> actionNotifyConnect;
    QPointer<QAction> actionNotifyDisconnect;
    QPointer<QAction> actionStartup;
    QPointer<QAction> actionAbout;
    QPointer<QAction> actionQuit;
    void loadSettings();

private slots:
    void timerTimeout();
    void menuAbout();
    void menuQuit();
    void saveSettings();
};
