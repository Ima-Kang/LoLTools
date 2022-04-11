#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QMainWindow>
#include <QPushButton>
#include "accountinfo.h"
#include <QLayout>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class AccountManager; }
QT_END_NAMESPACE

class AccountManager : public QMainWindow
{
    Q_OBJECT

public:
    AccountManager(QWidget *parent = nullptr);
    ~AccountManager();
private slots:
    void on_actionAdd_account_triggered();

private:
    void keyPressEvent(QKeyEvent* event);
    Ui::AccountManager *ui;
    std::vector<AccountInfo> accounts;
    QHash<QPushButton*, QHBoxLayout*> mButtonToLayoutMap;

    void addToLayout(QVBoxLayout* layout, AccountInfo ai);
};
#endif // ACCOUNTMANAGER_H
