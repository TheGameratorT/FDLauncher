#include "infodialog.h"
#include "ui_infodialog.h"

#include <QDesktopWidget>
#include <QPushButton>

InfoDialog::InfoDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::InfoDialog)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

	// dpi moment, dpi = pain
	// pain = bad aaaaaaaaaaaaaaaaaaaaaaaaaaa

	float dpi = QApplication::desktop()->logicalDpiX() / 96.0;
	float factor = 1 - (dpi - 1);

	QTextCursor cursor = ui->textBrowser->textCursor();
	ui->textBrowser->selectAll();
	ui->textBrowser->setFontPointSize(ui->textBrowser->fontPointSize() * factor);
	ui->textBrowser->setTextCursor(cursor);
}

InfoDialog::~InfoDialog()
{
	delete ui;
}
