#include "inputmanager.h"
#include <QFile>
#include <QDir>
#include <QDebug>

InputManager::InputManager() : QThread()
{
}

void InputManager::startMonitoring()
{
    m_continue = true;
    start(QThread::IdlePriority);

    qInfo() << "Started monitoring for keyboard input devices";
}

void InputManager::stopMonitoring()
{
    qInfo() << "Stopping monitoring...";

    m_continue = false;
    wait(10000);

    foreach (KeyboardMonitor* pRunningKeyboardMonitor, m_keyboardMonitors)
    {
        pRunningKeyboardMonitor->stopMonitoring();
        delete pRunningKeyboardMonitor;
    }

    qInfo() << "Stopped";
}

void InputManager::run()
{
    while (m_continue)
    {
        scanForDevices();
        QThread::usleep(HALF_SECOND_IN_NANOS);
    }
}

void InputManager::scanForDevices()
{
    // Get the list of all current input devices

    QFile devicesInfoFile("/proc/bus/input/devices");

    QStringList fileLines;
    if (devicesInfoFile.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&devicesInfoFile);
        while (true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
            {
                break;
            }
            else
            {
                fileLines.append(line);
            }
        }

        devicesInfoFile.close();
    }
    else
    {
        qFatal("Fatal error: could not open /proc/bus/input/devices");
    }

    // Find the event files for those that correspond to keyboards

    QStringList keyboardEventFiles;

    foreach (QString line, fileLines)
    {
        // Is the device a keyboard?
        if (line.contains("H: Handlers") && line.contains("sysrq"))
        {
            // If so, get the event id
            QStringList parts = line.split(" ");
            foreach (QString part, parts)
            {
                if (part.startsWith("event"))
                {
                    keyboardEventFiles.append("/dev/input/" + part);
                }
            }
        }
    }

    // Stop any event syncs that are no longer connected

    for (int i = m_keyboardMonitors.length() - 1; i >= 0; i--)
    {
        KeyboardMonitor* pRunningKeyboardMonitor = m_keyboardMonitors.at(i);
        if (keyboardEventFiles.contains(pRunningKeyboardMonitor->devicePath()) == false)
        {
            m_keyboardMonitors.removeAt(i);
            pRunningKeyboardMonitor->stopMonitoring();
            delete pRunningKeyboardMonitor;
        }
    }

    // Ignore any devices that are already monitored

    foreach (KeyboardMonitor* pRunningKeyboardMonitor, m_keyboardMonitors)
    {
        for (int i = keyboardEventFiles.length() - 1; i >= 0; i--)
        {
            if (pRunningKeyboardMonitor->devicePath() == keyboardEventFiles[i])
            {
                keyboardEventFiles.removeAt(i);
            }
        }
    }

    // Start any remaining (i.e. new) keyboard event syncs

    foreach (QString symLinkTarget, keyboardEventFiles)
    {
        KeyboardMonitor* pNewKeyboardMonitor = new KeyboardMonitor(symLinkTarget);
        pNewKeyboardMonitor->startMonitoring();

        m_keyboardMonitors.append(pNewKeyboardMonitor);
    }
}
