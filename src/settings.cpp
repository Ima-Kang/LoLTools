#include "../headers/settings.h"
#include "ui_settings.h"

Settings::Settings(
    QWidget *parent, QList<Profile>* __profiles, Profile* __currentProfile) :
    profiles{*__profiles}, currentProfile{*__currentProfile},
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QPushButton* applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    connect(ui->Add, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    for(auto& c : *__currentProfile->champs){
        genChampButton(c, ui->champList);
    }
    for(auto& c : *__currentProfile->banChamps){
        genChampButton(c, ui->banChampList);
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

    if(currentProfile.champs->indexOf(name) >= 0)
        currentProfile.champs->remove(currentProfile.champs->indexOf(name));
    else
        currentProfile.banChamps->remove(currentProfile.banChamps->indexOf(name));
    delete button;
}

void Settings::onAddClicked(){
    auto add = ui->comboBox->currentText() == "Add";
    QString champName = ui->name->text();

    if(currentProfile.champs->indexOf(champName) >= 0 ||
        currentProfile.banChamps->indexOf(champName) >= 0)
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
        currentProfile.champs->append(champName);
    else
        currentProfile.banChamps->append(champName);
    nameButton->setToolTip(QString{"Click to remove"});

    champLayoutList->insertWidget(champLayoutList->count() - 1, nameButton);
    ui->name->setText("");
}


