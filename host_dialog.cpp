#include "host_dialog.h"
#include "ui_host_dialog.h"

#include "launcher.h"

static QString map_for_selection[4][13] = {
    {"e1m1", "e1m0", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6", "e1m7", "e1m8"},
    {"e1m1", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6", "e2m1", "e2m2", "e2m3", "e2m4", "e2m5", "e2m6", "e2m7"},
	{"e1m1", "e1m2", "e1m3", "e1m4", "e1m5", "e1m7", "e1m8"},
    {"e1m1", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6"}
};

host_dialog::host_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::host_dialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    for(int i = 1; i <= 4; i++)
        if(i != launcher::currentGame)
            ui->tabWidget->removeTab(ui->tabWidget->indexOf(this->findChild<QWidget*>("fd" + QString::number(i) + "_tab")));
}

void host_dialog::on_host_btn_clicked()
{
    launcher::startupData data;
    data.type = data.host;
    data.playerCount = ui->nplayers_sb->value();
    data.netmode = ui->netmode_cb->currentIndex();
    data.port = ui->port_sb->value();

    QComboBox* map_cb = this->findChild<QComboBox*>("fd" + QString::number(launcher::currentGame) + "_map_cb");
    data.map = map_for_selection[launcher::currentGame-1][map_cb->currentIndex()];

    if(launcher::launchGame(data, this))
	{
		// bad practice I know, but sound fading
		close();
		QWidget* parent = parentWidget();
		parent->close();
		parent->parentWidget()->close();
	}
}

void host_dialog::on_cancel_btn_clicked()
{
    this->close();
}

host_dialog::~host_dialog()
{
    delete ui;
}
