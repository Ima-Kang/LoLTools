#ifndef REMOVEDIALOG_H
#define REMOVEDIALOG_H

#include <QDialog>
#include <QVector>
#include "accountinfo.h"
#include <QAbstractItemView>
#include <QScrollBar>
namespace Ui {
class RemoveDialog;
}

class RemoveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveDialog(QWidget *parent = nullptr,
        QVector<AccountInfo>* __accounts = nullptr);
    ~RemoveDialog();
    QString getUser();
private:
    Ui::RemoveDialog *ui;
    QVector<AccountInfo>& accounts;
    QString usr;
private slots:
    void selected();
};

#endif // REMOVEDIALOG_H
