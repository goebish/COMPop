/*
COMPop Virtual COM port notifier
Copyright (C) 2019 Goebish

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include <QtWidgets/QApplication>
#include <QtGui/QIcon>
#include <QtWidgets/QSystemTrayIcon>
#include <QSerialPortInfo>
#include <qthread.h>
#include <qsharedmemory.h>

#define POLLING_PERIOD 2500

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSharedMemory shared("COMPop");
    // app already running ?
    if(!shared.create( 1, QSharedMemory::ReadWrite))
		return 1;
    uint8_t i;
    bool firstRun = true;
    QStringList list;
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(QPixmap(":/COMPop/Resources/systray.png")));
    trayIcon->setToolTip("COMPop");
    trayIcon->setVisible(true);
    for(;;) {
        QStringList newList;
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
            for(i=0; i<newList.count(); i++) {
                if(list.indexOf(newList[i]) < 0) {
                    trayIcon->showMessage("Port Plugged in", newList[i], QSystemTrayIcon::Information, 20000);
                }
            }
            // search for removed ports
            for(i=0; i<list.count(); i++) {
                if(newList.indexOf(list[i]) < 0) {
                    trayIcon->showMessage("Port Plugged out", list[i], QSystemTrayIcon::Information, 20000);
                }
            }
            list = newList;
            continue;
        }
        QThread::msleep(POLLING_PERIOD);
    }
}
