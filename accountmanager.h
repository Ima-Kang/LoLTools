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
    void on_actionRemove_account_triggered();
    void populateLayout();
    void on_actionSort_All_Ctrl_S_triggered();

    void on_actionEdit_account_triggered();

private:
    Ui::AccountManager *ui;
    QVector<AccountInfo> accounts;

    QHash<int, bool> keysPressed;
    QHash<QString, QHBoxLayout*> mUserToLayoutMap;
    QHash<QString, QList<QHBoxLayout*>> accLayouts;

    QVBoxLayout* getCurrentLayout();
    void addToLayout(QVBoxLayout* layout, QHBoxLayout* accLayout);
    void keyReleaseEvent(QKeyEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void remove_from_layouts(QString usr);
    void clearLayout();

};
#endif // ACCOUNTMANAGER_H
