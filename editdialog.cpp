#include "editdialog.h"
#include "ui_editdialog.h"

EditDialog::EditDialog(QWidget *parent, QVector<AccountInfo>* __accounts) :
    QDialog(parent),
    ui(new Ui::EditDialog),
    accounts{*__accounts}
{
    ui->setupUi(this);
    for(auto& a : accounts)
        ui->accountBox->insertItem(0, a.getUser());
    ui->accountBox->setFont(QFont{"Arial", 12});
    connect(this, SIGNAL(accepted()), this, SLOT(entries()));
    connect(ui->sts, SIGNAL(activated(int)), this, SLOT(onTemp()));
    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate().addDays(14));
    ui->date->addWidget(dateEdit);
    dateEdit->hide();
    connect(ui->accountBox, SIGNAL(activated(int)), this, SLOT(selected()));
    connect(this, SIGNAL(accepted()), this, SLOT(selected()));
    ui->accountBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

EditDialog::~EditDialog()
{
    delete ui;
}
void EditDialog::onTemp(){
    if(ui->sts->currentText() == "Temp")
        dateEdit->show();
    else
        dateEdit->hide();
}

QString EditDialog::getSelectedUser(){
    return selectedUsr;
}

void EditDialog::selected(){
    selectedUsr = ui->accountBox->currentText();
    if(selectedUsr.isEmpty())
        return;

    auto acc = accounts.at(accounts.indexOf(selectedUsr));
    ui->ign->setText(acc.getInGameName());
    ui->usr->setText(acc.getUser());
    ui->pwd->setText(acc.getPassword());
    if(acc.getStatus() == "Available")
        ui->sts->setCurrentIndex(0);
    else if(acc.getStatus() == "Temp"){
        ui->sts->setCurrentIndex(1);
        dateEdit->show();
    }
    else if(acc.getStatus() == "Perma")
        ui->sts->setCurrentIndex(2);
    dateEdit->setDate(acc.getDate());

}
void EditDialog::entries(){
    setInGameName(ui->ign->text() != "" ? ui->ign->text() : "-");
    setUser(ui->usr->text() != "" ? ui->usr->text() : "-");
    setPassword(ui->pwd->text() != "" ? ui->pwd->text() : "-");
    if(ui->sts->currentText() == "Temp"){
        setDate(dateEdit->date());
        if(getDate() <= QDate::currentDate() && getDate() != QDate{}){
            setDate(QDate{});
            setStatus("Available");
            return;
        }
    }
    setStatus(ui->sts->currentText());
}

