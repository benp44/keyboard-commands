#include "keypresshandler.h"
#include "keyboardcodes.h"

#include <QDebug>
#include <QProcess>

KeyPressHandler::KeyPressHandler()
{
    m_config.loadConfig();
}

void KeyPressHandler::handleKeypress(int keycode)
{
    QStringList commands = m_config.getCommandsForKey(keycode);

    foreach(QString command, commands)
    {
        runCommandDetached(command);
    }
}

void KeyPressHandler::runCommandDetached(const QString& command)
{
    qInfo() << "Running command:" << command;

    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    env.insert("DISPLAY", ":0.0");
    process.setProcessEnvironment(env);

    bool success = process.startDetached(command);
    process.close();

    if (success == false)
    {
        qWarning() << "Error trying to run" << command << ":" << process.error();
    }
}
