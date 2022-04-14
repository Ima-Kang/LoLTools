#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QDate>
#include "accountinfo.h"
#include <QString>
#include <QDateEdit>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog, public AccountInfo
{
    Q_OBJECT

public:
    explicit AddDialog(QWidget *parent = nullptr);
    ~AddDialog();

private:
    Ui::AddDialog *ui;
    QDateEdit* dateEdit;
private slots:
    void entries();
    void onTemp();
};

#endif // ADDDIALOG_H
