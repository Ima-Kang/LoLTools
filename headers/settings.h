#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAbstractItemView>
#include <QAbstractButton>

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

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();
    QList<QString> champList;
    QList<QString> banChampList;
private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
