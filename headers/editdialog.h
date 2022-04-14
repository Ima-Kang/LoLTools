#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include <QDateEdit>
#include "accountinfo.h"
#include <QAbstractItemView>
#include <QScrollBar>

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog, public AccountInfo
{
    Q_OBJECT

public:
    explicit EditDialog(QWidget *parent = nullptr, QVector<AccountInfo>* __accounts = nullptr);
    ~EditDialog();
    QString getSelectedUser();
private:
    Ui::EditDialog *ui;
    QDateEdit* dateEdit;
    QVector<AccountInfo>& accounts;
    QString selectedUsr;

private slots:
    void entries();
    void onTemp();
    void selected();
};

#endif // EDITDIALOG_H
