#include "../headers/settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QPushButton* applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    connect(ui->Add, SIGNAL(clicked()), this, SLOT(onAddClicked()));
}

Settings::~Settings(){
    delete ui;
}

void Settings::apply(){

}

void Settings::onRemove(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString name = button->text();

    champList.remove(champList.indexOf(name));
    delete button;
}

void Settings::onAddClicked(){
    if(champList.size() >= 10)
        return;

    QString champName = ui->name->text();
    if(champList.indexOf(champName) >= 0 || champName == "")
        return;

    QVBoxLayout* champLayoutList = ui->champList;
    QPushButton* nameButton = new QPushButton(champName);
    QObject::connect(
        nameButton, &QPushButton::clicked,
        this, &Settings::onRemove
    );
    //nameButton->setMaximumSize(QSize{250, 25});

    champList.append(champName);
    nameButton->setToolTip(QString{"Click to remove"});
    //champLayoutList->addWidget(nameButton);
    champLayoutList->insertWidget(champLayoutList->count() - 1, nameButton);
    ui->name->setText("");
}


