#include "accountinfo.h"
#include <iostream>
AccountInfo::AccountInfo(){}
AccountInfo::AccountInfo(
        QString __ign, QString __usr, QString __pwd, QDate __date):
    ign{__ign}, usr{__usr}, pwd{__pwd}, date{__date} {
}
QString AccountInfo::getInGameName()const {return ign;}
QString AccountInfo::getUser() const{return usr;}
QString AccountInfo::getPassword() const{return pwd;}
QString AccountInfo::getStatus() const{return sts;}
QDate AccountInfo::getDate() const{return date;}

void AccountInfo::setInGameName(QString __ign){ign = __ign;}
void AccountInfo::setUser(QString __usr){usr = __usr;}
void AccountInfo::setPassword(QString __pwd){pwd = __pwd;}
void AccountInfo::setStatus(QString __sts){sts = __sts;}
void AccountInfo::setDate(QDate __date){date = __date;}
bool operator==(const AccountInfo& lhs, const QString& rhs){
    return lhs.usr == rhs;
}
QTextStream& operator<<(QTextStream& out, const AccountInfo acc){
    out << acc.getInGameName() << "\n";
    out << acc.getUser() << "\n";
    out << acc.getPassword() << "\n";
    out << acc.getStatus() << "\n";
    if(acc.getStatus() == "Temp"){
        out << acc.getDate().year() << "\n";
        out << acc.getDate().month() << "\n";
        out << acc.getDate().day() << "\n";
    }
    return out;
}
QTextStream& operator>>(QTextStream& in, AccountInfo& acc){
    QString mo, day, yr;
    if(in.atEnd())
        return in;
    acc.ign = in.readLine(AccountInfo::IN_BUFFER);
    acc.usr = in.readLine(AccountInfo::IN_BUFFER);
    acc.pwd = in.readLine(AccountInfo::IN_BUFFER);
    acc.sts = in.readLine(AccountInfo::IN_BUFFER);
    if(acc.getStatus() == "Temp"){
        yr = in.readLine(AccountInfo::IN_BUFFER);
        mo = in.readLine(AccountInfo::IN_BUFFER);
        day = in.readLine(AccountInfo::IN_BUFFER);
    }
    acc.setDate(QDate{yr.toInt(), mo.toInt(), day.toInt()});
    if(acc.date <= QDate::currentDate() && acc.date != QDate{})
        acc.sts = "Available";
    return in;
}
