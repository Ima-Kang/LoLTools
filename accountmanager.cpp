#include "accountmanager.h"
#include "ui_accountmanager.h"

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

