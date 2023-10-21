#include "join_dialog.h"
#include "ui_join_dialog.h"

#include "launcher.h"

#include <QMessageBox>

join_dialog::join_dialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::join_dialog)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

join_dialog::~join_dialog()
{
	delete ui;
}

void join_dialog::on_ok_btn_clicked()
{
	if(ui->ip_text->text().isEmpty())
	{
		//QApplication::beep();
		QMessageBox::information(this, tr("Oops!"), tr("The IP address must not be empty!"));
	}
	else
	{
		QStringList ip_port = ui->ip_text->text().split(':');
		launcher::startupData data;
		data.type = data.join;
		data.ip = ip_port[0];
		if(ip_port.length() > 1)
			data.port = ip_port[1].toInt();
		else
			data.port = 5029;
		if(launcher::launchGame(data, this))
		{
			// bad practice I know, but sound fading
			close();
			QWidget* parent = parentWidget();
			parent->close();
			parent->parentWidget()->close();
		}
	}
}

void join_dialog::on_cancel_btn_clicked()
{
	this->close();
}
