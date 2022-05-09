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
    void selected();
    void onNewProfileClicked();
    void removeRole();
    void selectedView();
    void addPlayer();
public:
   QList<Profile*>* profiles;
   QList<QString>* whitelist;
   QPushButton* currentSelection;

   Profile* currentProfile;

    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
    void genButton(QString champ, QVBoxLayout* layout);
    void clearLayouts();
    void setCurrentProfile();
    void setView(int i);
    void setAll();
    void setWhitelist();
private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
