#include "config.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

Config::Config()
{
}

void Config::loadConfig()
{
    loadConfigFilePath();

    QFileInfo fileInfo(m_configFilePath);

    if (fileInfo.exists() == false)
    {
        qFatal("Fatal: Config file missing");
    }

    QJsonArray json = getJson();
    parseJson(json);
}

QStringList Config::getCommandsForKey(int keycode) const
{
    if (m_keyCommands.contains(keycode))
    {
        return m_keyCommands.value(keycode);
    }

    return QStringList();
}

QJsonArray Config::getJson()
{
    QByteArray contentArr = loadByteArray();
    QJsonArray content;

    if (contentArr.length() > 0)
    {
        QJsonParseError* parseError = new QJsonParseError();

        QJsonDocument doc = QJsonDocument::fromJson(contentArr, parseError);

        bool valid = true;
        if (parseError->error != QJsonParseError::NoError)
        {
            valid = false;
            qDebug() << "Json parsing error";
        }

        if (doc.isArray() == false)
        {
            valid = false;
            qDebug() << "Json parsing error: expected array not found";
        }

        if (valid)
        {
            content = doc.array();
        }
    }

    return content;
}

QByteArray Config::loadByteArray()
{
    QFile file(m_configFilePath);
    QByteArray byteArray;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (in.atEnd() == false)
        {
            byteArray.append(in.readLine());
        }

        file.close();
    }
    return byteArray;
}

void Config::loadConfigFilePath()
{
    QString cmdFilePath = "";
    int noOfArguments = QCoreApplication::arguments().length();
    if (noOfArguments > 0)
    {
        bool prevFlagF = false;
        foreach(QString arg, QCoreApplication::arguments())
        {
            if (prevFlagF)
            {
                cmdFilePath = arg;
            }
            if (arg.simplified() == "-f")
            {
                prevFlagF = true;
            }
        }
    }

    // Fall back to default
    if (cmdFilePath == "")
    {
        cmdFilePath = "keyboard-commands.json";
    }

    m_configFilePath = cmdFilePath;
}

void Config::parseJson(const QJsonArray& jsonArr)
{
    m_keyCommands.clear();

    for (int i = 0; i < jsonArr.count(); i++)
    {
        QJsonValue rootVal = jsonArr.at(i);

        if (rootVal.isObject())
        {
            QJsonObject rootObj = jsonArr.at(i).toObject();

            QJsonValue idVal = rootObj["id"];

            if (idVal.isObject())
            {
                QJsonObject idObj = idVal.toObject();

                int keycode = idObj["keycode"].toInt(-1);

                if (keycode >= 0)
                {
                    QString name = idObj["name"].toString();
                    QJsonArray commands = rootObj["commands"].toArray();

                    if (commands.empty() == false)
                    {
                        qDebug() << "Command(s) found for " << name;

                        m_keyCommands[keycode] = QStringList();
                        foreach (QJsonValue command, commands)
                        {
                            m_keyCommands[keycode].append(command.toString());
                        }
                    }
                }
            }
            else
            {
                qDebug() << "Warning: value detected in root JSON array that is not JSON object";
            }
        }
        else
        {
            qDebug() << "Warning: value detected in root JSON array that is not JSON object";
        }
    }
}
