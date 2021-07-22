#ifndef HOST_DIALOG_H
#define HOST_DIALOG_H

#include <QDialog>

namespace Ui {
class host_dialog;
}

class host_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit host_dialog(QWidget *parent = nullptr);
    ~host_dialog();

private slots:
    void on_host_btn_clicked();

    void on_cancel_btn_clicked();

private:
    Ui::host_dialog *ui;
};

#endif // HOST_DIALOG_H
