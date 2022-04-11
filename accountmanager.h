#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QMainWindow>
#include "accountinfo.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AccountManager; }
QT_END_NAMESPACE

class AccountManager : public QMainWindow
{
    Q_OBJECT

public:
    AccountManager(QWidget *parent = nullptr);
    ~AccountManager();
    std::vector<AccountInfo> accounts;

private slots:
    void on_actionAdd_account_triggered();

private:
    Ui::AccountManager *ui;
};
#endif // ACCOUNTMANAGER_H
