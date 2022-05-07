#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAbstractItemView>
#include <QAbstractButton>
#include <QLayout>

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
    QList<QString>& champList;
    QList<QString>& banChampList;
    explicit Settings(QWidget *parent = nullptr,
        QList<QString>* = nullptr, QList<QString>* = nullptr);
    ~Settings();
    void genChampButton(QString champ, QVBoxLayout* layout);

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
