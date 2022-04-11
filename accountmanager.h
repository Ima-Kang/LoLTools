#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QMainWindow>
#include <QPushButton>
#include "accountinfo.h"
#include <QLayout>
#include <QKeyEvent>
#include <QSet>

#include <QMessageBox>

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
    Ui::AccountManager *ui;
    std::vector<AccountInfo> accounts;
    QHash<int, bool> keysPressed;
    QHash<QPushButton*, QHBoxLayout*> mButtonToLayoutMap;

    void addToLayout(QVBoxLayout* layout, AccountInfo ai);
    void keyReleaseEvent(QKeyEvent* event);
    void keyPressEvent(QKeyEvent* event);
};
#endif // ACCOUNTMANAGER_H
