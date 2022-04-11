#include "accountinfo.h"

AccountInfo::AccountInfo(){}
AccountInfo::AccountInfo(
        QString __ign, QString __usr, QString __pwd, QDate __date):
    ign{__ign}, usr{__usr}, pwd{__pwd}, date{__date} {
}
QString AccountInfo::getInGameName(){return ign;}
QString AccountInfo::getUser(){return usr;}
QString AccountInfo::getPassword(){return pwd;}
QString AccountInfo::getStatus(){return sts;}
QDate AccountInfo::getDate(){return date;}

void AccountInfo::setInGameName(QString __ign){ign = __ign;}
void AccountInfo::setUser(QString __usr){usr = __usr;}
void AccountInfo::setPassword(QString __pwd){pwd = __pwd;}
void AccountInfo::setStatus(QString __sts){sts = __sts;}
void AccountInfo::setDate(QDate __date){date = __date;}
