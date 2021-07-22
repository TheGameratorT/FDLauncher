#include "multiplayer_dialog.h"
#include "ui_multiplayer_dialog.h"

#include "join_dialog.h"
#include "host_dialog.h"

multiplayer_dialog::multiplayer_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::multiplayer_dialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void multiplayer_dialog::on_join_btn_clicked()
{
    join_dialog jd(this);
    jd.exec();
}

void multiplayer_dialog::on_host_btn_clicked()
{
    host_dialog hd(this);
    hd.exec();
}

multiplayer_dialog::~multiplayer_dialog()
{
    delete ui;
}
