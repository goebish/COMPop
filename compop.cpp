#include "stdafx.h"
#include "compop.h"
#include <QSerialPortInfo>
#include <QIcon>
#include <QPixmap>
#include <QSettings>
#include <QMessageBox>

compop::compop(QObject *parent)
    : QObject(parent)
{
    firstRun = true;
    
    trayIconMenu = new QMenu();
    
    actionNotifyConnect = new QAction("Notify on connect", this);
    actionNotifyConnect->setCheckable(true);
    connect(actionNotifyConnect, SIGNAL(triggered()), this, SLOT(saveSettings()));
    trayIconMenu->addAction(actionNotifyConnect);

    actionNotifyDisconnect = new QAction("Notify on disconnect", this);
    actionNotifyDisconnect->setCheckable(true);
    connect(actionNotifyDisconnect, SIGNAL(triggered()), this, SLOT(saveSettings()));
    trayIconMenu->addAction(actionNotifyDisconnect);

    actionStartup = new QAction("Launch on startup", this);
    actionStartup->setCheckable(true);
    connect(actionStartup, SIGNAL(triggered()), this, SLOT(saveSettings()));
    trayIconMenu->addAction(actionStartup);

    trayIconMenu->addSeparator();

    actionAbout = new QAction("About", this);
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(menuAbout()));
    trayIconMenu->addAction(actionAbout);

    trayIconMenu->addSeparator();

    actionQuit = new QAction("Quit", this);
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(menuQuit()));
    trayIconMenu->addAction(actionQuit);

    trayIcon = new QSystemTrayIcon(QIcon(QPixmap(":/COMPop/Resources/systray.png")));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("Found no COM ports");
    trayIcon->setVisible(true);

    loadSettings();

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    timer->start(POLLING_PERIOD);
}

compop::~compop()
{
    delete timer;
    trayIcon->setVisible(false);
    delete trayIconMenu;
    delete trayIcon;
}

void compop::timerTimeout()
{
    static QStringList list;
    QStringList newList;
    QString tooltip;
    int i;
    // enumerate available ports
    QList<QSerialPortInfo> serList = QSerialPortInfo::availablePorts();
    for(i=0; i<serList.count(); i++) {
        newList.append(serList[i].description() + QString(" (") + serList[i].portName() + QString(")"));
    }
    if(firstRun) {
        list = newList;
        firstRun = false;
    }
    if(list != newList) {
        // search for new ports
        if(actionNotifyConnect->isChecked()) {
            for(i=0; i<newList.count(); i++) {
                if(list.indexOf(newList[i]) < 0) {
                    trayIcon->showMessage("Port plugged in", newList[i], QSystemTrayIcon::Information, 20000);
                }
            }
        }
        // search for removed ports
        if(actionNotifyDisconnect->isChecked()) {
            for(i=0; i<list.count(); i++) {
                if(newList.indexOf(list[i]) < 0) {
                    trayIcon->showMessage("Port plugged out", list[i], QSystemTrayIcon::Information, 20000);
                }
            }
        }
        list = newList;
        if(list.isEmpty()) {
            trayIcon->setToolTip("Found no COM ports");
        }
        else {
            for(i=0; i<list.count(); i++) {
                tooltip += list[i] + QString("\n");
            }
            trayIcon->setToolTip(tooltip);
        }
    }
}

void compop::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Goebish Apps", "compop");
    actionNotifyConnect->setChecked(settings.value("NotifyConnect", "1") == "1");
    actionNotifyDisconnect->setChecked(settings.value("NotifyDisconnect", "1") == "1");
    actionStartup->setChecked(settings.value("Startup", "0") == "1");
}

// Slots

void compop::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Goebish Apps", "compop");
    settings.setValue("NotifyConnect", actionNotifyConnect->isChecked() ? "1" : "0");
    settings.setValue("NotifyDisconnect", actionNotifyDisconnect->isChecked() ? "1" : "0");
    settings.setValue("Startup", actionStartup->isChecked() ? "1" : "0");
    QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if(actionStartup->isChecked()) {
        registry.setValue("compop", QCoreApplication::applicationFilePath().replace("/","\\"));
    }
    else {
        registry.remove("compop");
    }
}

void compop::menuAbout()
{
    QMessageBox::about(nullptr, "About COMPop", "COMPop Virtual COM port notifier\n(c) 2019 Goebish\nVersion 1.0.2\n\nUpdates:\nhttps://github.com/goebish/COMPop/releases");
}

void compop::menuQuit()
{
    QApplication::quit();
}
