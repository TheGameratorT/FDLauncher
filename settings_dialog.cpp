#include "settings_dialog.h"
#include "ui_settings_dialog.h"

#include "launcher.h"
#include "fdsettings.h"

#include <QPushButton>

static QString languageForIndex[7] = { "auto", "en", "ptg", "ptb", "de", "fr", "ita" };

settings_dialog::settings_dialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::settings_dialog)
{
	ui->setupUi(this);
	this->setFixedSize(this->size());
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// WARN: Since FNAF Doom 1 v4.0, materials are always on, remove the line below to revert it (and in fdsettings.cpp too)
	ui->mtls_cb->setVisible(false);

	ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Apply"));
	ui->buttonBox->button(QDialogButtonBox::Discard)->setText(tr("Discard"));
	ui->buttonBox->button(QDialogButtonBox::RestoreDefaults)->setText(tr("Restore Defaults"));

	resForIndex = resolution::getAvailableDisplayResolutions();
	for(const resolution::DisplayResolution& res : qAsConst(resForIndex))
	{
		ui->res_cb->addItem(QString::number(res.width) + "x" + QString::number(res.height));
	}

	fillWidgetsWithSets(false);
}

settings_dialog::~settings_dialog()
{
	delete ui;
}

void settings_dialog::fillWidgetsWithSets(bool defSets)
{
	FDSettings fdsets(launcher::currentGame, this);
	FDSettings::SettingsData sd = fdsets.getSettingsData(defSets);
	ui->lp_mdls_cb->setChecked(sd.lowpoly);
	ui->mtls_cb->setChecked(sd.materials);
	ui->bloom_cb->setChecked(sd.gl_bloom);
	ui->ssao_cb->setCurrentIndex(sd.gl_ssao);
	ui->msaa_cb->setCurrentIndex(sd.gl_multisample);
	ui->fxaa_cb->setCurrentIndex(sd.gl_fxaa);

	if(sd.vid_scalemode == 5)
	{
		int index = resForIndex.indexOf(resolution::DisplayResolution(sd.vid_scale_customwidth, sd.vid_scale_customheight));
		if(index == -1)
		{
			index = resForIndex.length() - 1;
		}
		ui->res_cb->setCurrentIndex(index);
	}
	else
	{
		ui->res_cb->setCurrentIndex(resForIndex.length() - 1);
	}

	int i = 0;
	for(const QString& lang : languageForIndex)
	{
		if(lang == sd.language)
		{
			ui->lang_cb->setCurrentIndex(i);
			break;
		}
		i++;
	}
}

void settings_dialog::on_buttonBox_clicked(QAbstractButton *button)
{
	if((QPushButton*)button == ui->buttonBox->button(QDialogButtonBox::Apply))
	{
		FDSettings fdsets(launcher::currentGame, this);
		FDSettings::SettingsData sd;
		sd.gl_ssao = ui->ssao_cb->currentIndex();
		sd.gl_multisample = ui->msaa_cb->currentIndex();
		sd.gl_fxaa = ui->fxaa_cb->currentIndex();

		bool isMaxRes = ui->res_cb->currentIndex() == resForIndex.length() - 1;
		if(isMaxRes)
		{
			sd.vid_scalemode = 0;
		}
		else
		{
			sd.vid_scalemode = 5;
			sd.vid_scale_customwidth = resForIndex[ui->res_cb->currentIndex()].width;
			sd.vid_scale_customheight = resForIndex[ui->res_cb->currentIndex()].height;
		}

		sd.gl_bloom = ui->bloom_cb->isChecked();
		sd.language = languageForIndex[ui->lang_cb->currentIndex()];
		sd.materials = ui->mtls_cb->isChecked();
		sd.lowpoly = ui->lp_mdls_cb->isChecked();
		fdsets.setSettingsData(sd);

		this->close();
	}
	else if ((QPushButton*)button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults))
	{
		fillWidgetsWithSets(true);
	}
	else if ((QPushButton*)button == ui->buttonBox->button(QDialogButtonBox::Discard))
	{
		this->close();
	}
}
