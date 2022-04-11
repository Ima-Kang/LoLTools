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

void AccountManager::keyPressEvent(QKeyEvent* event){
    if(event->key() == Qt::Key_T){
        on_actionAdd_account_triggered();
    }
}

void AccountManager::addToLayout(QVBoxLayout* layout, AccountInfo ai){
    QVBoxLayout* rootLayout = qobject_cast<QVBoxLayout*>(
        layout->layout()
    );
    QHBoxLayout* newLayout = new QHBoxLayout(
        qobject_cast<QWidget*>(ui->all_scroll_layout)
    );
    newLayout->setSpacing(5);

    QLabel* number = new QLabel{
        tr("#%1").arg(layout->count()),
        qobject_cast<QWidget*>(ui->all_scroll_layout)
    };
    number->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    number->setMinimumWidth(25);
    newLayout->addWidget(number);
    QPushButton* nameButton = new QPushButton(
        ai.getInGameName(),
        qobject_cast<QWidget*>(ui->all_scroll_layout)
    );
    newLayout->addWidget(nameButton);
    QPushButton* userButton = new QPushButton(
        ai.getUser(),
        qobject_cast<QWidget*>(ui->all_scroll_layout)
    );
    newLayout->addWidget(userButton);
    QPushButton* passwordButton = new QPushButton(
        ai.getPassword(),
        qobject_cast<QWidget*>(ui->all_scroll_layout)
    );
    newLayout->addWidget(passwordButton);
    QPushButton* statusButton = new QPushButton(
        QString{"-"},
        qobject_cast<QWidget*>(ui->all_scroll_layout)
    );
    newLayout->addWidget(statusButton);

    rootLayout->insertLayout(0, newLayout);

    //mButtonToLayoutMap.insert(button, newLayout);
}

void AccountManager::on_actionAdd_account_triggered()
{
    AddDialog addDialog;
    addDialog.setModal(true);
    addDialog.exec();
    accounts.push_back((AccountInfo)addDialog);

    addToLayout(ui->all_scroll_layout, (AccountInfo)addDialog);
    addToLayout(ui->available_scroll_layout, (AccountInfo)addDialog);
    addToLayout(ui->temp_scroll_layout, (AccountInfo)addDialog);
    addToLayout(ui->perma_scroll_layout, (AccountInfo)addDialog);
}

