#ifndef CONFIG_H
#define CONFIG_H

#include <QJsonObject>
#include <QJsonArray>
#include <QHash>

class Config
{
   public:
    Config();

    void loadConfig();
    QStringList getCommandsForKey(int keycode) const;

   private:
    QJsonArray getJson();
    void loadConfigFilePath();
    QByteArray loadByteArray();
    void parseJson(const QJsonArray& jsonArr);

    QHash<int, QStringList> m_keyCommands;
    QString m_configFilePath;
};

#endif  // CONFIG_H
