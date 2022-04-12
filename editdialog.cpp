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
QString EditDialog::getUser(){
    return usr;
}
void EditDialog::selected(){
    usr = ui->accountBox->currentText();
}
void EditDialog::entries(){
    setInGameName(ui->ign->text() != "" ? ui->ign->text() : "-");
    setUser(ui->usr->text() != "" ? ui->usr->text() : "-");
    setPassword(ui->pwd->text() != "" ? ui->pwd->text() : "-");
    if(ui->sts->currentText() == "Temp")
        setDate(dateEdit->date());
    setStatus(ui->sts->currentText());
}

