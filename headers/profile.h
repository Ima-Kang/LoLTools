#ifndef PROFILE_H
#define PROFILE_H

#include <QList>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
class Profile
{
public:
    QString* profileName;
    QList<QString>* champs;
    QList<QString>* banChamps;
    Profile();
    Profile(QList<QString>* __champs, QList<QString>* __banChamps,
        QString* __profileName);
    void read(QJsonObject &json);
    void write(QJsonObject &json);
};

#endif // PROFILE_H
