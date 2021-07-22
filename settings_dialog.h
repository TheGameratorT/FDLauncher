#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "resolution.h"

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class settings_dialog;
}

class settings_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit settings_dialog(QWidget *parent = nullptr);
    ~settings_dialog();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::settings_dialog *ui;

    void fillWidgetsWithSets(bool defSets);

	QVector<resolution::DisplayResolution> resForIndex;
};

#endif // SETTINGS_DIALOG_H
