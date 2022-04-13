#include <iostream>
#include "accountmanager.h"

AccountManager::AccountManager(QWidget *parent):
        QMainWindow(parent), ui(new Ui::AccountManager){
    ui->setupUi(this);
    setFixedSize(width(), height());
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(populateLayout()));
    accLayouts.insert(QString{"All"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Available"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Temp"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Perma"}, QList<QHBoxLayout*>{});
    loadAccounts();
    updateDetails();
}

AccountManager::~AccountManager(){  delete ui;}

void AccountManager::loadAccounts(){
    QFile file(QDir::currentPath().append(".accounts"));
    if(!file.open(QFile::ReadOnly | QFile::Text))
        return;
    QTextStream in(&file);
    while(!in.atEnd()){
        AccountInfo acc;
        in >> acc;
        if(acc.getInGameName().isEmpty())
            break;

        accounts.push_back(acc);
        generateAccountLayout(acc);
    }
    file.close();
}

void AccountManager::closeEvent(QCloseEvent *bar){
    QFile file(QDir::currentPath().append(".accounts"));
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(
            this,
            "Warning", "Cannot save file: " + file.errorString()
        );
        return;
    }
    QTextStream out(&file);
    for(auto& acc : accounts){
        out << acc;
    }

    file.close();
    bar->accept();
}

void AccountManager::keyPressEvent(QKeyEvent* event){
    keysPressed[event->key()] = true;
}

void AccountManager::keyReleaseEvent(QKeyEvent* event){
    if(keysPressed[Qt::Key_Control] && keysPressed[Qt::Key_A]){
        keysPressed[Qt::Key_Control] = false;
        keysPressed[Qt::Key_A] = false;
        on_actionAdd_account_triggered();
    } else if(keysPressed[Qt::Key_Control] && keysPressed[Qt::Key_R]){
        keysPressed[Qt::Key_Control] = false;
        keysPressed[Qt::Key_R] = false;
        on_actionRemove_account_triggered();
    } else if(keysPressed[Qt::Key_Control] && keysPressed[Qt::Key_E]){
        keysPressed[Qt::Key_Control] = false;
        keysPressed[Qt::Key_E] = false;
        on_actionEdit_account_triggered();
    }
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

void AccountManager::onButtonCopy(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(button->text());
}

void AccountManager::generateAccountLayout(AccountInfo& acc){
    QHBoxLayout* newLayout = new QHBoxLayout();
    QLabel* number = new QLabel{tr("#%1").arg(accounts.count())};
    number->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    number->setMinimumWidth(25);
    newLayout->addWidget(number);
    QPushButton* nameButton = new QPushButton(acc.getInGameName());
    QObject::connect(
        nameButton, &QPushButton::clicked,
        this, &AccountManager::onButtonCopy
    );
    newLayout->addWidget(nameButton);
    QPushButton* userButton = new QPushButton(acc.getUser());
    QObject::connect(
        userButton, &QPushButton::clicked,
        this, &AccountManager::onButtonCopy
    );
    newLayout->addWidget(userButton);
    QPushButton* passwordButton = new QPushButton(acc.getPassword());
    QObject::connect(
        passwordButton, &QPushButton::clicked,
        this, &AccountManager::onButtonCopy
    );
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

void AccountManager::on_actionAdd_account_triggered(){
    AddDialog addDialog;

    addDialog.setModal(true);
    if(addDialog.exec() == QDialog::DialogCode::Rejected)
        return;
    auto msgErr = new QMessageBox{
        QMessageBox::Warning,
        QString{""},
        QString{""},
        QMessageBox::NoButton
    };
    if(addDialog.getUser() == "-" || addDialog.getPassword() == "-"){
        msgErr->setText("Username and password required");
        msgErr->exec();
        return;
    }
    if(accounts.contains(addDialog.getUser())){
        msgErr->setText("Account already exists in table");
        msgErr->exec();
        return;
    }

    auto acc = (AccountInfo)addDialog;
    accounts.push_back(acc);
    generateAccountLayout(acc);
    updateDetails();
}

void AccountManager::remove_from_layouts(QString usr){
    QHBoxLayout* selectedLayout = mUserToLayoutMap.take(usr);
    accounts.remove(accounts.indexOf(usr));

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
    RemoveDialog removeDialog{nullptr, &(accounts)};
    removeDialog.setModal(true);
    if(removeDialog.exec() == QDialog::DialogCode::Rejected)
        return;
    if(removeDialog.getUser().isEmpty())
        return;
    remove_from_layouts(removeDialog.getUser());
    updateDetails();
}

void AccountManager::updateDetails(){
    ui->numAccounts->setText(
        "Number of Accounts: " +
        QString{std::to_string(accounts.count()).c_str()}
    );
    ui->numAvailable->setText(
        "Available Accounts: " + QString{
            std::to_string(std::count_if(
                accounts.begin(), accounts.end(),
                [](AccountInfo a){return a.getStatus() == "Available";}
            )).c_str()
        }
    );
    ui->numTemp->setText(
        "Temp Accounts: " + QString{
            std::to_string(std::count_if(
                accounts.begin(), accounts.end(),
                [](AccountInfo a){return a.getStatus() == "Temp";}
            )).c_str()
        }
    );
    ui->numPerma->setText(
        "Perma Accounts: " + QString{
            std::to_string(std::count_if(
                accounts.begin(), accounts.end(),
                [](AccountInfo a){return a.getStatus() == "Perma";}
            )).c_str()
        }
    );
};

void AccountManager::on_actionEdit_account_triggered(){
    EditDialog editDialog{nullptr, &(accounts)};
    editDialog.setModal(true);
    if(editDialog.exec() == QDialog::DialogCode::Rejected)
        return;
    auto msgErr = new QMessageBox{
        QMessageBox::Warning,
        QString{""},
        QString{""},
        QMessageBox::NoButton
    };
    if(editDialog.getUser() == "-" || editDialog.getPassword() == "-"){
        msgErr->setText("Username and password required");
        msgErr->exec();
        return;
    }
    //  Removing from old container and placing it in the new container
    QHBoxLayout* selectedLayout = mUserToLayoutMap.take(editDialog.getSelectedUser());
    AccountInfo ai = (AccountInfo)editDialog;
    auto oldStatus = accounts.at(accounts.indexOf(editDialog.getSelectedUser())).getStatus();

    mUserToLayoutMap[editDialog.getUser()] = selectedLayout;
    accLayouts[oldStatus].removeAt(accLayouts[oldStatus].indexOf(selectedLayout));
    accLayouts[ai.getStatus()].push_back(selectedLayout);


    qobject_cast<QPushButton*>(selectedLayout->itemAt(1)->widget())->
        setText(editDialog.getInGameName());
    qobject_cast<QPushButton*>(selectedLayout->itemAt(2)->widget())->
        setText(editDialog.getUser());
    qobject_cast<QPushButton*>(selectedLayout->itemAt(3)->widget())->
        setText(editDialog.getPassword());
    QString statusMessage = ai.getStatus() == "Temp" ?
        ai.getStatus() + ": " + ai.getDate().toString() :
    ai.getStatus();
    qobject_cast<QPushButton*>(selectedLayout->itemAt(4)->widget())->
        setText(statusMessage);
    accounts.replace(accounts.indexOf(editDialog.getSelectedUser()), ai);
    updateDetails();
}





