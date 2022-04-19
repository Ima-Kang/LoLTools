#include "../headers/accountmanager.h"

AccountManager::AccountManager(QWidget *parent):
        QMainWindow(parent), ui(new Ui::AccountManager){
    ui->setupUi(this);
    setFixedSize(width(), height());
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(populateLayout()));
    setWindowIcon(QIcon(":/imgs/ChampionSquare.bmp"));
    accLayouts.insert(QString{"All"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Available"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Temp"}, QList<QHBoxLayout*>{});
    accLayouts.insert(QString{"Perma"}, QList<QHBoxLayout*>{});

    getKey();
    loadAccounts();
    updateDetails();
}

void AccountManager::getKey(){
    QFile file(QDir::currentPath().append("/.key"));
    if(!file.open(QFile::ReadOnly | QFile::Text))
        return;
    QTextStream in(&file);
    const_cast<QString&>(KEY) = in.readAll();
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

        generateAccountLayout(acc);
        accounts.push_back(acc);
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
    for(auto hbl : accLayouts[tabName]){
        hbl->setParent(nullptr);
        addToLayout(tab, hbl);
    }
    updateRowNumber();
}

void AccountManager::onButtonCopy(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(button->text());
}

void AccountManager::addToLayout(QVBoxLayout* layout, QHBoxLayout* newLayout){
    QVBoxLayout* rootLayout = qobject_cast<QVBoxLayout*>(layout->layout());
    rootLayout->insertLayout(layout->count() - 1, newLayout);
}

void AccountManager::generateAccountLayout(AccountInfo& acc){
    QVBoxLayout* tab = getCurrentLayout();
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

    mUserToLayoutMap.insert(acc.getUser(), newLayout);
    accLayouts[QString{"All"}].push_back(newLayout);
    accLayouts[acc.getStatus()].push_back(newLayout);

    if(acc.getInGameName() != "-" &&
        acc.getStatus() != "Perma" &&
        acc.getStatus() != "Temp"){
        acc.setRank(getRankDetails(acc));
    }
    qDebug() << acc.getRank();
    QLabel* rank = new QLabel{acc.getRank()};
    QString style = "QLabel { ";
    if(acc.getRank().contains("UNRANKED"))
        style += "background-color : #ffffff;";
    else if(acc.getRank().contains("IRON"))
        style += "background-color : #696969;";
    else if(acc.getRank().contains("BRONZE"))
        style += "background-color : #987654;";
    else if(acc.getRank().contains("SILVER"))
        style += "background-color : #bebebe;";
    else if(acc.getRank().contains("GOLD"))
        style += "background-color : #ffd700;";
    else if(acc.getRank().contains("PLATINUM"))
        style += "background-color : #88d8c0;";
    else if(acc.getRank().contains("DIAMOND"))
        style += "background-color : #00b7eb;";
    else if(acc.getRank().contains("GRANDMASTER"))
        style += "background-color : #dc143c;";
    else if(acc.getRank().contains("MASTER"))
        style += "background-color : #9f00c5;";
    else if(acc.getRank().contains("CHALLENGER"))
        style += "background-color : #ffea70;";
    else{
        style += "background-color : #ffffff;";
        acc.setRank("UNRANKED");
        rank->setText("UNRANKED");
    }
    style += "color : black;";
    style += "font-weight: bold;";
    style += "border-bottom-right-radius : 20px;";

    if(acc.getRank().contains("CHALLENGER"))
        style += "border: 2px solid #9bddff;}";
    else
        style += "border: 2px solid black;}";

    rank->setStyleSheet(style);
    rank->setAlignment(Qt::AlignCenter);
    newLayout->addWidget(rank);
    addToLayout(tab, newLayout);
}

//  might move this to AccountInfo
QString AccountManager::getRankDetails(AccountInfo acc){
    QString *stream = new QString{acc.getInGameName() + "?api_key=" + KEY};
    auto manager = new QNetworkAccessManager();
    QEventLoop loop;

    QObject::connect(manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) mutable -> auto{
            if (reply->error()) {
                qDebug() << reply->errorString();
                *stream = acc.getRank();
                return;
            }

            QString answer = (QString)reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();

            *stream = jsonObject["id"].toString() + "?api_key=" + KEY;
            reply->deleteLater();
        }
    );
    connect(manager, SIGNAL(finished(QNetworkReply*)),&loop, SLOT(quit()));
    manager->get(QNetworkRequest{QUrl{SUMMONER_BY_NAME + *stream}});
    loop.exec();

    QObject::disconnect(manager, nullptr, this, nullptr);
    QObject::connect(manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) mutable -> auto{
            if (reply->error()) {
                qDebug() << reply->errorString();
                *stream = acc.getRank();
                return;
            }

            QString answer = (QString)reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
            QJsonObject jsonObject = jsonResponse.array().at(0).toObject();

            if(!jsonObject["tier"].toString().isEmpty()){
                *stream = (jsonObject["tier"].toString() + " "
                    + jsonObject["rank"].toString() + ", "
                    + QString::number(jsonObject["leaguePoints"].toInt())
                    + " lp"
                );
            } else{
                *stream = acc.getRank();
            }
            reply->deleteLater();
        }
    );
    manager->get(QNetworkRequest{QUrl{LEAGUE_BY_SUMMONER + *stream}});
    loop.exec();

    return *stream;
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
    for(auto& hbl : list){
        qobject_cast<QLabel*>(hbl->itemAt(0)->widget())->
            setText("#" + QString{std::to_string(
            getCurrentLayout()->indexOf(hbl) + 1).c_str()}
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





