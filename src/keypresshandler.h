#ifndef KEYPRESSHANDLER_H
#define KEYPRESSHANDLER_H

#include "config.h"

#include <QString>

class KeyPressHandler
{
public:
    KeyPressHandler();

    void loadKeypressCommands();
    void handleKeypress(int keycode);

private:

    void runCommandDetached(const QString& command);

    Config m_config;
};

#endif // KEYPRESSHANDLER_H
