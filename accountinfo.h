#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H
#include <QString>
#include <QDate>

class AccountInfo
{
private:
    QString ign, usr, pwd, sts;
    QDate date;
public:
    AccountInfo();
    AccountInfo(QString __ign, QString __usr, QString __pwd, QDate __date);
    QString getInGameName();
    QString getUser();
    QString getPassword();
    QString getStatus();
    QDate getDate();
    void setInGameName(QString ign);
    void setUser(QString usr);
    void setPassword(QString pwd);
    void setStatus(QString sts);
    void setDate(QDate date);
};

#endif // ACCOUNTINFO_H
