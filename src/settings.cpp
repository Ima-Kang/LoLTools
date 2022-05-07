#include "../headers/settings.h"
#include "ui_settings.h"

Settings::Settings(
    QWidget *parent, QList<Profile*>* __profiles, Profile* __currentProfile) :
    profiles{*__profiles}, currentProfile{__currentProfile},
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

    if(__currentProfile->profileName->isEmpty() || !__currentProfile)
        ui->removeWidget->setVisible(false);
    else{
        for(const auto& c : *__currentProfile->champs)
            genChampButton(c, ui->champList);
        for(const auto& c : *__currentProfile->banChamps)
            genChampButton(c, ui->banChampList);
        for(const auto& p: *__profiles)
            ui->profileBox->insertItem(0, *p->profileName);
        ui->profileBox->setCurrentText(*__currentProfile->profileName);
        ui->profileLabel->setText(*__currentProfile->profileName);
    }
}

void Settings::removeRole(){
    if(currentProfile->profileName->isEmpty())
        return;

    auto i = profiles.indexOf(currentProfile);
    ui->profileBox->removeItem(ui->profileBox->currentIndex());
    profiles.remove(i);
    clearLayouts();

    if(i > 0)
        currentProfile = profiles.at(i - 1);
    else{
        delete currentProfile;
        currentProfile = new Profile{};
        ui->removeWidget->setVisible(false);
        return;
    }

    ui->profileLabel->setText(*currentProfile->profileName);
    ui->profileBox->setCurrentText(*currentProfile->profileName);

    for(auto& champ : *currentProfile->champs)
        genChampButton(champ, ui->champList);
    for(auto& champ : *currentProfile->banChamps)
        genChampButton(champ, ui->banChampList);
}

void Settings::selected(){
    QString selected{ui->profileBox->currentText()};

    ui->removeWidget->setVisible(true);
    ui->profileLabel->setText(selected);

    for(auto p : profiles)
        if(selected == *p->profileName)
            currentProfile = p;

    clearLayouts();

    for(auto& champ : *currentProfile->champs)
        genChampButton(champ, ui->champList);
    for(auto& champ : *currentProfile->banChamps)
        genChampButton(champ, ui->banChampList);
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

void Settings::genChampButton(QString champName, QVBoxLayout* layout){
    QPushButton* nameButton = new QPushButton(champName);
    QObject::connect(
        nameButton, &QPushButton::clicked,
        this, &Settings::onRemove
    );
    nameButton->setToolTip(QString{"Click to remove"});

    layout->insertWidget(layout->count() - 1, nameButton);
}

void Settings::onRemove(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString name = button->text();

    if(currentProfile->champs->indexOf(name) >= 0)
        currentProfile->champs->remove(currentProfile->champs->indexOf(name));
    else
        currentProfile->banChamps->remove(currentProfile->banChamps->indexOf(name));
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

    QVBoxLayout* champLayoutList = add ? ui->champList : ui->banChampList;
    QPushButton* nameButton = new QPushButton(champName);
    QObject::connect(
        nameButton, &QPushButton::clicked,
        this, &Settings::onRemove
    );
    //nameButton->setMaximumSize(QSize{250, 25});
    if(add)
        currentProfile->champs->append(champName);
    else
        currentProfile->banChamps->append(champName);
    nameButton->setToolTip(QString{"Click to remove"});

    champLayoutList->insertWidget(champLayoutList->count() - 1, nameButton);
    ui->name->setText("");
}

void Settings::onNewProfileClicked(){
    if(ui->profileName->text() == "")
        return;

    for(auto& p : profiles)
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
    profiles.append(currentProfile);
}
