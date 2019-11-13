#include "keyboardmonitor.h"

#include <QDebug>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string>
#include <linux/input.h>

using namespace std;

#define KEY_PRESS_EVENT 1

KeyboardMonitor::KeyboardMonitor(const QString& devicePath) : QThread()
{
    m_devicePath = devicePath;
    m_keyPressHandler = new KeyPressHandler();
}

QString KeyboardMonitor::devicePath() const
{
    return m_devicePath;
}

bool KeyboardMonitor::startMonitoring()
{
    // Try to open the device

    if ((m_nDeviceHandle = open (m_devicePath.toStdString().c_str(), O_RDONLY)) == -1)
    {
        qWarning() <<  m_devicePath << "is not a valid device";
        return false;
    }

    qDebug().nospace() << "Starting monitoring of device:" << m_devicePath;

    m_continue = true;
    start(QThread::IdlePriority);

    return true;
}

void KeyboardMonitor::stopMonitoring()
{
    qDebug().nospace() << "Stopping monitoring of device:" << m_devicePath;

    m_continue = false;
    wait(5000);

    close (m_nDeviceHandle);
}

void KeyboardMonitor::run()
{
    struct input_event zEventData[64];

    int nEventSize = sizeof (struct input_event);
    int nReadLength;
    string deviceName = m_devicePath.toStdString();

    ioctl (m_nDeviceHandle, EVIOCGNAME (64), deviceName.c_str());

    qInfo() << "Monitoring" << deviceName.c_str();

    while (m_continue == true)
    {
        // Read i/o events from the device

        fd_set set;

        FD_ZERO(&set);
        FD_SET(m_nDeviceHandle, &set);

        struct timeval timeout;

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        // Select waits for either i/o event or timeout

        int returnValue = select(m_nDeviceHandle + 1, &set, NULL, NULL, &timeout);

        if (returnValue == 0)
        {
            // Timeout
        }
        else if (returnValue > 0)
        {
            // I/O event

            nReadLength = read(m_nDeviceHandle, zEventData, nEventSize * 64);

            if (nReadLength < nEventSize)
            {
                qWarning() << "Error reading from device (possibly disconnected)";
                QThread::sleep(2);
            }
            else
            {
                int nEventValue = zEventData[0].value;

                if (nEventValue != ' ' && zEventData[1].value == 1 && zEventData[1].type == KEY_PRESS_EVENT)
                {
                    m_keyPressHandler->handleKeypress(zEventData[1].code);
                }
            }
        }
    }
}
