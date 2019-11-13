#ifndef KEYBOARDMONITOR_H
#define KEYBOARDMONITOR_H

#include "keypresshandler.h"

#include <QThread>

#define HALF_SECOND_IN_NANOS 500000

class KeyboardMonitor : public QThread
{
   public:

    KeyboardMonitor(const QString& devicePath);
    bool startMonitoring();
    void stopMonitoring();
    QString devicePath() const;

   protected:

    virtual void run();

private:

    KeyPressHandler* m_keyPressHandler;
    QString m_devicePath;
    int m_nDeviceHandle;
    bool m_continue;
};

#endif // KEYBOARDMONITOR_H
