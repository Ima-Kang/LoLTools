#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H
#include <QString>
#include <QDate>
#include <QTextStream>
#include <QDate>

class AccountInfo
{
private:
    QString ign, usr, pwd, sts;
    QDate date;
public:
    AccountInfo();
    AccountInfo(QString __ign, QString __usr, QString __pwd, QDate __date);
    QString getInGameName() const;
    QString getUser() const;
    QString getPassword() const;
    QString getStatus() const;
    QDate getDate() const;
    void setInGameName(QString ign);
    void setUser(QString usr);
    void setPassword(QString pwd);
    void setStatus(QString sts);
    void setDate(QDate date);
    friend bool operator==(const AccountInfo& lhs, const QString& rhs);
    friend QTextStream& operator<<(QTextStream& out, const AccountInfo account);
    friend QTextStream& operator>>(QTextStream& in, AccountInfo& account);
};

#endif // ACCOUNTINFO_H
