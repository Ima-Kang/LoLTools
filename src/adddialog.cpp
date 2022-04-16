#include "../headers/adddialog.h"
#include "ui_adddialog.h"

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(entries()));
    connect(ui->sts, SIGNAL(activated(int)), this, SLOT(onTemp()));
    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate().addDays(14));
    ui->date->addWidget(dateEdit);
    dateEdit->hide();
    setRank("UNRANKED");
}

AddDialog::~AddDialog(){
    delete ui;
}
void AddDialog::onTemp(){
    if(ui->sts->currentText() == "Temp")
        dateEdit->show();
    else
        dateEdit->hide();

}
void AddDialog::entries(){
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
