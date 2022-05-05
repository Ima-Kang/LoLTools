#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include "accountinfo.h"
#include "editdialog.h"
#include "ui_accountmanager.h"
#include "adddialog.h"
#include "removedialog.h"
#include "script.h"

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
#include <QStyle>


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

    void on_actionEnableAccept_triggered();

private:
    Ui::AccountManager *ui;
    QVector<AccountInfo> accounts;
    Script* scripts;

    QHash<int, bool> keysPressed;
    QHash<QString, QFrame*> mUserToLayoutMap;
    QHash<QString, QList<QFrame*>> accLayouts;

    QVBoxLayout* getCurrentLayout();
    void updateDetails();
    void generateAccountLayout(AccountInfo& acc);
    void addToLayout(QVBoxLayout* layout, QFrame* accLayout);
    void keyReleaseEvent(QKeyEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void remove_from_layouts(QString usr);
    void clearLayout();
    void closeEvent(QCloseEvent *bar) override;
    void loadAccounts();
    void onButtonCopy();
    void updateRowNumber();
    void onStatusChange();
};
#endif // ACCOUNTMANAGER_H
