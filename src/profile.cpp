#include "../headers/profile.h"

Profile::Profile
(QList<QString>* __champs, QList<QString>* __banChamps, QString* __profileName):
    profileName{__profileName}, champs{__champs}, banChamps{__banChamps}{}
Profile::Profile(): profileName{new QString},
    champs{new QList<QString>{}}, banChamps{new QList<QString>}{}

void Profile::read(QJsonObject &json){
    QJsonArray champArray = json["champs"].toArray();
    QJsonArray banChampArray = json["banChamps"].toArray();
    *profileName = json["profileName"].toString();

    for(int i = 0; i < champArray.size(); i++){
        QString champ = champArray[i].toString();
        champs->append(champ);
    }
    for(int i = 0; i < banChampArray.size(); i++){
        QString champ = banChampArray[i].toString();
        banChamps->append(champ);
    }
}
void Profile::write(QJsonObject &json){
    QJsonArray champArray, banChampArray;
    QJsonObject obj;
    foreach(const QString champ, *champs){
        champArray.append(champ);
    }

    foreach(const QString champ, *banChamps){
        banChampArray.append(champ);
    }
    json["champs"] = champArray;
    json["banChamps"] = banChampArray;
    json["profileName"] = *profileName;
    qDebug() << json.size();
}
