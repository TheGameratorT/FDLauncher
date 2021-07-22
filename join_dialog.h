#ifndef JOIN_DIALOG_H
#define JOIN_DIALOG_H

#include <QDialog>

namespace Ui {
class join_dialog;
}

class join_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit join_dialog(QWidget *parent = nullptr);
    ~join_dialog();

private slots:
    void on_ok_btn_clicked();

    void on_cancel_btn_clicked();

private:
    Ui::join_dialog *ui;
};

#endif // JOIN_DIALOG_H
