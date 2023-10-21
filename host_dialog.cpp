#include "host_dialog.h"
#include "ui_host_dialog.h"

#include <QMessageBox>

#include "launcher.h"

static QString map_for_selection[4][13] = {
    {"e1m1", "e1m0", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6", "e1m7", "e1m8"},
    {"e1m1", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6", "e2m1", "e2m2", "e2m3", "e2m4", "e2m5", "e2m6", "e2m7"},
	{"e1m1", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6", "e1m7", "e1m8"},
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

    ui->netmodewarn_btn->setVisible(false);
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

void host_dialog::on_netmode_cb_currentIndexChanged(int index)
{
    ui->netmodewarn_btn->setVisible(index == 1);
}

void host_dialog::on_netmodewarn_btn_clicked()
{
    QMessageBox::information(this, tr("About Packet Server mode"), tr(
"<p>Please consider using Peer-to-Peer mode instead.</p>"
"<p>Packet Server mode is <strong>not recommended</strong>, as it is known to solve connectivity issues but comes at the cost of reduced stability, lag increase and synchronization issues.</p>"
"<p>We strongly recommend taking the proper measures to host the game in Peer-to-Peer mode. This alternative can provide a more stable gaming experience while maintaining better overall performance.</p>"
"<p>Learn more about ZDoom networking here:<br />"
"<a style=\"color: #FF802B;\" href=\"https://forum.zdoom.org/viewtopic.php?t=45124\">https://forum.zdoom.org/viewtopic.php?t=45124</a></p>"
));
}

host_dialog::~host_dialog()
{
    delete ui;
}
