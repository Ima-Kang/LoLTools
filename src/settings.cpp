#include "../headers/settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    profiles{new QList<Profile*>}, whitelist{new QList<QString>},
    currentSelection{nullptr}, currentProfile{new Profile},
    ui(new Ui::Settings)
{
    ui->setupUi(this);        
    setFixedSize(width(), height());
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QPushButton* applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    connect(ui->profileBox, SIGNAL(activated(int)), this, SLOT(selected()));

    connect(ui->Add, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    connect(ui->newProfile, SIGNAL(clicked()), this, SLOT(onNewProfileClicked()));
    connect(ui->removeRole, SIGNAL(clicked()), this, SLOT(removeRole()));

    connect(ui->selectButton, SIGNAL(clicked()), this, SLOT(selectedView()));
    connect(ui->reportButton, SIGNAL(clicked()), this, SLOT(selectedView()));
    connect(ui->whiteListAdd, SIGNAL(clicked()), this, SLOT(addPlayer()));
}

void Settings::saveSettings(){
    QJsonObject json;
    QJsonObject currentProfileObject;

    currentProfile->write(currentProfileObject);
    json["currentProfile"] = currentProfileObject;

    QJsonArray profilesArray;
    foreach(Profile* p, *profiles){
        QJsonObject profileObject;
        p->write(profileObject);
        profilesArray.append(profileObject);
    }
    json["profiles"] = profilesArray;

    QJsonArray whitelistArray;
    foreach(QString val, *whitelist){
        whitelistArray.append(val);
    }
    json["whitelist"] = whitelistArray;

    QFile saveFile(QStringLiteral("settings.json"));
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't save.");
        return;
    }

    QJsonDocument saveDoc{json};
    saveFile.write(saveDoc.toJson());
    saveFile.close();
}

void Settings::loadSettings(){
    QFile loadFile(QStringLiteral("settings.json"));
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't load.");
        return;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject json = loadDoc.object();

    QJsonObject currentProfileObject = json["currentProfile"].toObject();
    currentProfile->read(currentProfileObject);

    QJsonArray profilesArray = json["profiles"].toArray();
    for(int i = 0; i < profilesArray.size(); i++){
        QJsonObject profileObject = profilesArray[i].toObject();
        auto p = new Profile{};
        p->read(profileObject);
        profiles->append(p);
    }

    QJsonArray whitelistArray = json["whitelist"].toArray();
    foreach(QJsonValueRef val, whitelistArray)
        whitelist->append(val.toString());
    json["whitelist"] = whitelistArray;

    loadFile.close();
}

void Settings::addPlayer(){
    QString name = ui->nameWhitelist->text();

    if(whitelist->indexOf(name) >= 0)
        return;
    else if(name == "")
        return;

    whitelist->append(name);
    genButton(name, ui->whitelist);
    ui->nameWhitelist->setText("");
}

void Settings::setView(int i){
    QString style{".QPushButton{background-color: rgba(255, 255, 255, 20%);"};
    style += "border-radius: 3px;";
    style += " border: 1px solid #2ee6e6;}";

    if(currentSelection)
        currentSelection->setStyleSheet("");
    else
        currentSelection = ui->selectButton;

    switch(i){
        case 0:
            setCurrentProfile();
            selected();
            currentSelection = ui->selectButton;
            break;
        case 1:
            currentSelection = ui->reportButton;
            break;
        default:
            break;
    }

    currentSelection->setStyleSheet(style);
    ui->stackedWidget->setCurrentIndex(i);
}

void Settings::selectedView(){
    auto buttonView = qobject_cast<QPushButton*>(sender());
    QString style{".QPushButton{background-color: rgba(255, 255, 255, 20%);"};
    style += "border-radius: 3px;";
    style += " border: 1px solid #2ee6e6;}";

    if(currentSelection)
        currentSelection->setStyleSheet("");

    currentSelection = buttonView;
    currentSelection -> setStyleSheet(style);

    if(currentSelection->objectName() == "selectButton")
        ui->stackedWidget->setCurrentIndex(0);
    else if(currentSelection->objectName() == "reportButton")
        ui->stackedWidget->setCurrentIndex(1);
}

void Settings::setAll(){
    setWhitelist();
    setCurrentProfile();
}

void Settings::setWhitelist(){
    if(whitelist->size() == ui->whitelist->count())
        return;

    for(const auto& p : *whitelist)
        genButton(p, ui->whitelist);
}

void Settings::setCurrentProfile(){
    if(profiles->size() == ui->profileBox->count())
        return;

    if(currentProfile->profileName->isEmpty() || !currentProfile)
        ui->removeWidget->setVisible(false);
    else{
        for(const auto& c : *currentProfile->banChamps)
            genButton(c, ui->banChampList);
        for(const auto& p: *profiles)
            ui->profileBox->insertItem(0, *p->profileName);
        ui->profileBox->setCurrentText(*currentProfile->profileName);
        ui->profileLabel->setText(*currentProfile->profileName);
    }
}

void Settings::removeRole(){
    auto i = profiles->indexOf(currentProfile);

    ui->profileBox->removeItem(ui->profileBox->currentIndex());
    profiles->remove(i);
    clearLayouts();

    if(i > 0)
        currentProfile = profiles->at(i - 1);
    else if(profiles->size() > 0)
        currentProfile = profiles->at(i);
    else{
        delete currentProfile;
        currentProfile = new Profile{};
        ui->removeWidget->setVisible(false);
        return;
    }

    ui->profileLabel->setText(*currentProfile->profileName);
    ui->profileBox->setCurrentText(*currentProfile->profileName);

    for(auto& champ : *currentProfile->champs)
        genButton(champ, ui->champList);
    for(auto& champ : *currentProfile->banChamps)
        genButton(champ, ui->banChampList);
}

void Settings::selected(){
    if(currentProfile->profileName->isEmpty()){
        ui->removeWidget->setVisible(false);
        return;
    }
    QString selected{ui->profileBox->currentText()};

    ui->removeWidget->setVisible(true);
    ui->profileLabel->setText(selected);

    for(auto p : *profiles)
        if(selected == *p->profileName)
            currentProfile = p;

    clearLayouts();

    for(auto& champ : *currentProfile->champs)
        genButton(champ, ui->champList);
    for(auto& champ : *currentProfile->banChamps)
        genButton(champ, ui->banChampList);
}

void Settings::clearLayouts(){
    auto selectedLayout = ui->champList;
    while(selectedLayout->count() != 0){
        QLayoutItem* item = selectedLayout->takeAt(0);
        delete item->widget();
        delete item;
    }

    selectedLayout = ui->banChampList;
    while(selectedLayout->count() != 0){
        QLayoutItem* item = selectedLayout->takeAt(0);
        delete item->widget();
        delete item;
    }
}

Settings::~Settings(){
    delete ui;
}

void Settings::apply(){

}

void Settings::genButton(QString champName, QVBoxLayout* layout){
    QPushButton* nameButton = new QPushButton(champName);
    QObject::connect(
        nameButton, &QPushButton::clicked,
        this, &Settings::onRemove
    );
    nameButton->setToolTip(QString{"Click to remove"});
    nameButton->setFont(QFont{"Arial", 12});
    layout->insertWidget(layout->count() - 1, nameButton);
}

void Settings::onRemove(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString name = button->text();

    if(!currentSelection)
        return;

    if(currentSelection->objectName() == "reportButton"){
        whitelist->remove(whitelist->indexOf(name));
    } else{
        if(currentProfile->champs->indexOf(name) >= 0)
            currentProfile->champs->remove(currentProfile->champs->indexOf(name));
        else
            currentProfile->banChamps->remove(currentProfile->banChamps->indexOf(name));
    }

    delete button;
}

void Settings::onAddClicked(){
    auto add = ui->comboBox->currentText() == "Add";
    QString champName = ui->name->text();

    if(currentProfile->champs->indexOf(champName) >= 0 ||
        currentProfile->banChamps->indexOf(champName) >= 0)
        return;
    else if(champName == "")
        return;

    if(add)
        currentProfile->champs->append(champName);
    else
        currentProfile->banChamps->append(champName);

    QVBoxLayout* champLayoutList = add ? ui->champList : ui->banChampList;
    genButton(champName, champLayoutList);
    ui->name->setText("");
}

void Settings::onNewProfileClicked(){
    if(ui->profileName->text() == "")
        return;

    for(auto& p : *profiles)
        if(ui->profileName->text() == *p->profileName){
            return;
        }

    clearLayouts();
    currentProfile = new Profile{};
    *currentProfile->profileName = ui->profileName->text();

    ui->profileBox->insertItem(0, ui->profileName->text());
    ui->removeWidget->setVisible(true);
    ui->profileLabel->setText(ui->profileName->text());
    ui->profileBox->setCurrentText(ui->profileName->text());
    ui->profileName->setText("");
    profiles->append(currentProfile);
}
