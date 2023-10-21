#ifndef MULTIPLAYER_DIALOG_H
#define MULTIPLAYER_DIALOG_H

#include <QDialog>

namespace Ui {
class multiplayer_dialog;
}

class multiplayer_dialog : public QDialog
{
	Q_OBJECT

public:
	explicit multiplayer_dialog(QWidget *parent = nullptr);
	~multiplayer_dialog();

private slots:
	void on_join_btn_clicked();

	void on_host_btn_clicked();

private:
	Ui::multiplayer_dialog *ui;
};

#endif // MULTIPLAYER_DIALOG_H
