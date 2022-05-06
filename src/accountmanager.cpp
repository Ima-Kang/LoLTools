#include "../headers/accountmanager.h"

AccountManager::AccountManager(QWidget *parent):
        QMainWindow(parent), ui(new Ui::AccountManager){
    ui->setupUi(this);
    setFixedSize(width(), height());
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(populateLayout()));
    setWindowIcon(QIcon(":/imgs/ChampionSquare.bmp"));
    accLayouts.insert(QString{"All"}, QList<QFrame*>{});
    accLayouts.insert(QString{"Available"}, QList<QFrame*>{});
    accLayouts.insert(QString{"Temp"}, QList<QFrame*>{});
    accLayouts.insert(QString{"Perma"}, QList<QFrame*>{});

    scripts = new Script{};
    hotkeyA = new Hotkey();
    hotkeyR = new Hotkey();

    hotkeyA->registerHotkey(QKeySequence { "Ctrl+Shift+A" });
    hotkeyR->registerHotkey(QKeySequence { "Ctrl+Shift+R" });

    connect(hotkeyA, SIGNAL(activated()), this, SLOT(on_actionEnableAccept_triggered()));
    connect(hotkeyR, SIGNAL(activated()), this, SLOT(on_actionEnableReport_triggered()));

    loadAccounts();
    updateDetails();
}

AccountManager::~AccountManager(){  delete ui;}

void AccountManager::loadAccounts(){
    QFile file(QDir::currentPath().append("/.accounts"));
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
    QFile file(QDir::currentPath().append("/.accounts"));
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
    updateRowNumber();
    if(index == 0){
        tabName = "All";
    } else if(index == 1){
        tabName = "Available";
    } else if(index == 2){
        tabName = "Temp";
    } else if(index == 3){
        tabName = "Perma";
    }

    auto list = accLayouts[tabName];
    for(auto& hbl : list){
        //hbl->setParent(nullptr);
        addToLayout(tab, hbl);
    }
    updateRowNumber();
}

void AccountManager::onButtonCopy(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QFrame* selectedFrame = qobject_cast<QFrame*>(button -> parent());
    QString style{".QFrame{background-color: rgba(255, 255, 255, 20%);"};
    style += "border-radius: 3px;";
    style += " border: 1px solid #2ee6e6;}";
    for(auto& frame : mUserToLayoutMap){
        if(frame != selectedFrame)
            frame -> setStyleSheet("");
    }
    selectedFrame -> setStyleSheet(style);

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(button->text());
}

void AccountManager::addToLayout(QVBoxLayout* layout, QFrame* newLayout){
    QVBoxLayout* rootLayout = qobject_cast<QVBoxLayout*>(layout->layout());
    rootLayout->insertWidget(layout->count() - 1, newLayout);
}

void AccountManager::generateAccountLayout(AccountInfo& acc){
    QVBoxLayout* tab = getCurrentLayout();
    QHBoxLayout* newLayout = new QHBoxLayout();
    newLayout -> setContentsMargins(4, 4, 4, 4);

    QLabel* number = new QLabel{tr("#%1").arg(accounts.count())};

    QFrame* frame = new QFrame();

    frame -> setLayoutDirection(Qt::LayoutDirection::LeftToRight);
    frame -> setLayout(newLayout);
    frame -> setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    number->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    number->setMinimumWidth(25);
    newLayout->addWidget(number);
    QPushButton* nameButton = new QPushButton(acc.getInGameName());
    QObject::connect(
        nameButton, &QPushButton::clicked,
        this, &AccountManager::onButtonCopy
    );
    nameButton->setToolTip(QString{"Copy name"});
    newLayout->addWidget(nameButton);
    QPushButton* userButton = new QPushButton(acc.getUser());
    QObject::connect(
        userButton, &QPushButton::clicked,
        this, &AccountManager::onButtonCopy
    );
    userButton->setToolTip(QString{"Copy user"});
    newLayout->addWidget(userButton);
    QPushButton* passwordButton = new QPushButton(acc.getPassword());
    QObject::connect(
        passwordButton, &QPushButton::clicked,
        this, &AccountManager::onButtonCopy
    );
    passwordButton->setToolTip(QString{"Copy password"});
    newLayout->addWidget(passwordButton);
    QString statusMessage = acc.getStatus() == "Temp" ?
        acc.getStatus() + ": " + acc.getDate().toString() :
    acc.getStatus();
    QPushButton* statusButton = new QPushButton(statusMessage);
    QObject::connect(
        statusButton, &QPushButton::clicked,
        this, &AccountManager::onStatusChange
    );
    statusButton->setObjectName(QString{acc.getUser()});
    statusButton->setToolTip(QString{"Change status"});
    newLayout->addWidget(statusButton);
    newLayout->setSpacing(5);

    mUserToLayoutMap.insert(acc.getUser(), frame);
    accLayouts[QString{"All"}].push_back(frame);
    accLayouts[acc.getStatus()].push_back(frame);

    addToLayout(tab, frame);
}

void AccountManager::onStatusChange(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
//    QString user = qobject_cast<QPushButton*>(
//        button->parent()->
//    children().at(2))->text();
    QString user = button->objectName();
    QString status;
    QDate banDate = QDate::currentDate().addDays(14);
    if(button->text() == "Available"){
        button->setText("Temp: " + banDate.toString());
        status = "Temp";
    } else if(button->text() == "Perma"){
        button->setText("Available");
        status = "Available";
    } else{
        button->setText("Perma");
        status = "Perma";
    }

    auto oldStatus = accounts.at(accounts.indexOf(user)).getStatus();
    auto layout = mUserToLayoutMap[user];
    accLayouts[oldStatus].removeAt(accLayouts[oldStatus].indexOf(layout));
    accLayouts[status].push_back(layout);
    accounts[accounts.indexOf(user)].setStatus(status);
    accounts[accounts.indexOf(user)].setDate(banDate);
    updateDetails();
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
    QFrame* selectedFrame = mUserToLayoutMap.take(usr);
    QHBoxLayout* selectedLayout = (QHBoxLayout*)selectedFrame->layout();
    accounts.remove(accounts.indexOf(usr));

    for(auto list = accLayouts.begin(); list != accLayouts.end(); ++list){
        auto idx = (*list).indexOf(selectedFrame);
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
    delete selectedFrame;
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
    updateRowNumber();
}

void AccountManager::updateRowNumber(){
    auto index = ui->tabWidget->currentIndex();
    QString tabName;
    if(index == 0){
        tabName = "All";
    } else if(index == 1){
        tabName = "Available";
    } else if(index == 2){
        tabName = "Temp";
    } else if(index == 3){
        tabName = "Perma";
    }
    auto list = accLayouts[tabName];
    for(auto& frame : list){
        auto hbl = frame->layout();
        qobject_cast<QLabel*>(hbl->itemAt(0)->widget())->
            setText("#" + QString{std::to_string(
            getCurrentLayout()->indexOf(frame) + 1).c_str()}
        );
    }
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
    QFrame* selectedFrame = mUserToLayoutMap.take(editDialog.getSelectedUser());
    QHBoxLayout* selectedLayout = (QHBoxLayout*)selectedFrame->layout();
    AccountInfo ai = (AccountInfo)editDialog;
    auto oldStatus = accounts.at(accounts.indexOf(editDialog.getSelectedUser())).getStatus();

    mUserToLayoutMap[editDialog.getUser()] = selectedFrame;
    accLayouts[oldStatus].removeAt(accLayouts[oldStatus].indexOf(selectedFrame));
    accLayouts[ai.getStatus()].push_back(selectedFrame);


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

void AccountManager::on_actionEnableAccept_triggered(){
    scripts -> trigger(Script::type::Accept);
}

void AccountManager::on_actionEnableReport_triggered(){
    scripts -> trigger(Script::type::Report);
}

void AccountManager::on_actionEnableInsert_triggered(){
    scripts -> trigger(Script::type::Select);
}


void AccountManager::on_actionSettings_4_triggered(){
    Settings settings{nullptr};
    settings.setModal(true);
    if(settings.exec() == QDialog::DialogCode::Rejected)
        return;
}

