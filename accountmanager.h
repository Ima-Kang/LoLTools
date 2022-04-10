#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class AccountManager; }
QT_END_NAMESPACE

class AccountManager : public QMainWindow
{
    Q_OBJECT

public:
    AccountManager(QWidget *parent = nullptr);
    ~AccountManager();

private:
    Ui::AccountManager *ui;
};
#endif // ACCOUNTMANAGER_H
