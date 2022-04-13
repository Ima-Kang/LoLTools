#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "accountinfo.h"
#include "editdialog.h"
#include "ui_accountmanager.h"
#include "adddialog.h"
#include "removedialog.h"

#include <QMainWindow>
#include <QPushButton>
#include <QLayout>
#include <QKeyEvent>
#include <QSet>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QClipboard>

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
    void on_actionRemove_account_triggered();
    void populateLayout();
    void on_actionEdit_account_triggered();

private:
    Ui::AccountManager *ui;
    QVector<AccountInfo> accounts;

    QHash<int, bool> keysPressed;
    QHash<QString, QHBoxLayout*> mUserToLayoutMap;
    QHash<QString, QList<QHBoxLayout*>> accLayouts;

    QVBoxLayout* getCurrentLayout();
    void updateDetails();
    void generateAccountLayout(AccountInfo& acc);
    void addToLayout(QVBoxLayout* layout, QHBoxLayout* accLayout);
    void keyReleaseEvent(QKeyEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void remove_from_layouts(QString usr);
    void clearLayout();
    void closeEvent(QCloseEvent *bar) override;
    void loadAccounts();
    void onButtonCopy();

};
#endif // ACCOUNTMANAGER_H
