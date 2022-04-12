#include "accountmanager.h"
#include "ui_accountmanager.h"
#include "adddialog.h"
#include "removedialog.h"
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
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(populateLayout()));
    accLayouts.insert(QString{"All"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Available"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Temp"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Perma"}, QList<QHBoxLayout*>{});
}

AccountManager::~AccountManager(){  delete ui;}

void AccountManager::keyPressEvent(QKeyEvent* event){
    keysPressed[event->key()] = true;
}

void AccountManager::keyReleaseEvent(QKeyEvent* event){
    if(keysPressed[Qt::Key_Control] && keysPressed[Qt::Key_A])
        on_actionAdd_account_triggered();
    else if(keysPressed[Qt::Key_Control] && keysPressed[Qt::Key_R])
        on_actionRemove_account_triggered();
    keysPressed[event->key()] = false;
}

void AccountManager::addToLayout(QVBoxLayout* layout, QHBoxLayout* newLayout){
    QVBoxLayout* rootLayout = qobject_cast<QVBoxLayout*>(layout->layout());
    rootLayout->insertLayout(0, newLayout);
}

QVBoxLayout* AccountManager::getCurrentLayout(){
    auto index = ui->tabWidget->currentIndex();
    QVBoxLayout* tab = ui->all_scroll_layout;
    if(index == 0){
        tab = ui->all_scroll_layout;
    } else if(index == 1){
        tab = ui->available_scroll_layout;
    } else if(index == 2){
        tab = ui->temp_scroll_layout;
    } else if(index == 3){
        tab = ui->perma_scroll_layout;
    }
    return tab;
}

void AccountManager::populateLayout(){
    auto index = ui->tabWidget->currentIndex();
    QString tabName;
    QVBoxLayout* tab = getCurrentLayout();
    if(index == 0){
        tabName = "All";
    } else if(index == 1){
        tabName = "Available";
    } else if(index == 2){
        tabName = "Temp";
    } else if(index == 3){
        tabName = "Perma";
    }
    for(auto hbl : accLayouts[tabName]){
        hbl->setParent(nullptr);
        addToLayout(tab, hbl);
    }
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
    auto acc = (AccountInfo)addDialog;
    accounts.push_back(acc);

    QHBoxLayout* newLayout = new QHBoxLayout();
    QLabel* number = new QLabel{tr("#%1").arg(accounts.count())};
    number->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    number->setMinimumWidth(25);
    newLayout->addWidget(number);
    QPushButton* nameButton = new QPushButton(acc.getInGameName());
    newLayout->addWidget(nameButton);
    QPushButton* userButton = new QPushButton(acc.getUser());
    newLayout->addWidget(userButton);
    QPushButton* passwordButton = new QPushButton(acc.getPassword());
    newLayout->addWidget(passwordButton);
    QString statusMessage = acc.getStatus() == "Temp" ?
        acc.getStatus() + ": " + acc.getDate().toString() :
    acc.getStatus();
    QPushButton* statusButton = new QPushButton(statusMessage);
    newLayout->addWidget(statusButton);
    newLayout->setSpacing(5);

    mUserToLayoutMap.insert(acc.getUser(), newLayout);
    accLayouts[QString{"All"}].push_back(newLayout);
    accLayouts[acc.getStatus()].push_back(newLayout);

    QVBoxLayout* tab = getCurrentLayout();
    addToLayout(tab, newLayout);
}

void AccountManager::remove_from_layouts(QString usr){
    QHBoxLayout* selectedLayout = mUserToLayoutMap.take(usr);

    for(auto list = accLayouts.begin(); list != accLayouts.end(); ++list){
        auto idx = (*list).indexOf(selectedLayout);
        if(idx != -1)
            (*list).remove(idx);
    }

    if(selectedLayout != nullptr){
        while(selectedLayout->count() != 0){
            QLayoutItem* item = selectedLayout->takeAt(0);
            delete item->widget();
            delete item;
        }
    }
    delete selectedLayout;
};

void AccountManager::on_actionRemove_account_triggered(){
    RemoveDialog removeDialog{nullptr, &accounts};
    removeDialog.setModal(true);
    if(removeDialog.exec() == QDialog::DialogCode::Rejected)
        return;
    remove_from_layouts(removeDialog.getUser());
}

