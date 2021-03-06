#include "../headers/removedialog.h"
#include "ui_removedialog.h"

RemoveDialog::RemoveDialog(QWidget *parent, QVector<AccountInfo>* __accounts) :
    QDialog(parent),
    ui(new Ui::RemoveDialog),
    accounts{*__accounts}
{
    ui->setupUi(this);
    for(auto& a : accounts)
        ui->accountBox->insertItem(0, a.getUser());
    ui->accountBox->setFont(QFont{"Arial", 12});
    connect(ui->accountBox, SIGNAL(activated(int)), this, SLOT(selected()));
    connect(this, SIGNAL(accepted()), this, SLOT(selected()));
    ui->accountBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}
RemoveDialog::~RemoveDialog()
{
    delete ui;
}
QString RemoveDialog::getUser(){
    return usr;
}
void RemoveDialog::selected(){
    usr = ui->accountBox->currentText();
}
