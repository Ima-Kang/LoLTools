#include "accountmanager.h"
#include "ui_accountmanager.h"
#include "adddialog.h"
#include <iostream>
AccountManager::AccountManager(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AccountManager)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
}

AccountManager::~AccountManager()
{
    delete ui;
}


void AccountManager::on_actionAdd_account_triggered()
{
    AddDialog addDialog;
    addDialog.setModal(true);
    addDialog.exec();
    std::cout << addDialog.getInGameName().toStdString() << std::endl;
}

