#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "keyboardmonitor.h"
#include <QList>
#include <QThread>

class InputManager : public QThread
{
public:
    InputManager();
    void startMonitoring();
    void stopMonitoring();

protected:
    void run();

private:
    void scanForDevices();

    QList<KeyboardMonitor*> m_keyboardMonitors;
    bool m_continue;
};

#endif // INPUTMANAGER_H
