#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAbstractItemView>
#include <QAbstractButton>
#include <QLayout>
#include "profile.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT
private slots:
    void apply();
    void onAddClicked();
    void onRemove();
    void onNewProfileClicked();
public:
   QList<Profile>& profiles;
   Profile& currentProfile;

    explicit Settings(QWidget *parent = nullptr,
        QList<Profile>* = nullptr,  Profile* = nullptr);
    ~Settings();
    void genChampButton(QString champ, QVBoxLayout* layout);
private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
