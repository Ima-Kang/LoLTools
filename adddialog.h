#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QDate>
#include "accountinfo.h"

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

private slots:
    void nameEntry();
    void userEntry();
    void passwordEntry();
};

#endif // ADDDIALOG_H
