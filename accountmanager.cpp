#include "accountmanager.h"
#include "ui_accountmanager.h"
#include "adddialog.h"
#include <iostream>

AccountManager::AccountManager(QWidget *parent):
        QMainWindow(parent), ui(new Ui::AccountManager){
    ui->setupUi(this);
    setFixedSize(width(), height());
//    QObject::connect(
//        ui->addWidget_button,
//        &QPushButton::clicked,
//        this, &MainWindow::onAddWidget
//    );
}

AccountManager::~AccountManager(){  delete ui;}

void AccountManager::keyPressEvent(QKeyEvent* event){
    keysPressed[event->key()] = true;
}

void AccountManager::keyReleaseEvent(QKeyEvent* event){
    if(keysPressed[Qt::Key_Control] && keysPressed[Qt::Key_A])
        on_actionAdd_account_triggered();
    keysPressed[event->key()] = false;
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
    QString statusMessage = ai.getStatus() == "Temp" ?
        ai.getStatus() + ": " + ai.getDate().toString() :
    ai.getStatus();
    QPushButton* statusButton = new QPushButton(
        statusMessage,
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
    if(addDialog.exec() == QDialog::DialogCode::Rejected)
        return;
    if(addDialog.getUser() == "-" || addDialog.getPassword() == "-"){
        auto msgErr = new QMessageBox{
            QMessageBox::Warning,
            QString{""},
            QString{"Username and password required"},
            QMessageBox::NoButton
        };
        msgErr->exec();
        return;
    }

    accounts.push_back((AccountInfo)addDialog);

    addToLayout(ui->all_scroll_layout, (AccountInfo)addDialog);
    addToLayout(ui->available_scroll_layout, (AccountInfo)addDialog);
    addToLayout(ui->temp_scroll_layout, (AccountInfo)addDialog);
    addToLayout(ui->perma_scroll_layout, (AccountInfo)addDialog);
}

