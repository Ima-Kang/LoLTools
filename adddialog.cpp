#include "adddialog.h"
#include "ui_adddialog.h"

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);
    connect(ui->ign, SIGNAL(editingFinished()), this, SLOT(nameEntry()));
    connect(ui->usr, SIGNAL(editingFinished()), this, SLOT(userEntry()));
    connect(ui->pwd, SIGNAL(editingFinished()), this, SLOT(passwordEntry()));
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::nameEntry(){
    setInGameName(ui->ign->text());
}
void AddDialog::userEntry(){
    setUser(ui->usr->text());
}
void AddDialog::passwordEntry(){
    setPassword(ui->pwd->text());
}

