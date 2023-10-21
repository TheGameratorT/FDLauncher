#include "gameselectwindow.h"
#include "ui_gameselectwindow.h"

#include "launcher.h"
#include "gamewindow.h"

#include <QPropertyAnimation>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#ifndef _WIN32
#include <QScreen>
#endif

GameSelectWindow* GameSelectWindow::instance;

GameSelectWindow::GameSelectWindow(QWidget *parent)
    : QMainWindow(parent)
	, ui(new Ui::GameSelectWindow)
	, audio_fade_timer(this)
{
    instance = this;

    ui->setupUi(this);
	this->setFixedSize(this->size());
#ifndef _WIN32
    this->move(QApplication::primaryScreen()->geometry().center() - this->rect().center());
#endif

    // Hide FD5 and FD6 widgets, but keep them just in case
    ui->fd5_btn->setVisible(false);
    ui->fd6_btn->setVisible(false);

	audioDevice.openDevice();

	connect(&audio_fade_timer, &QTimer::timeout, this, [this]{ audioFader.update(); });
	audio_fade_timer.start(1);

    for(int i = 1; i <= 6; i++)
    {
        launcher::updateInstallDataForGame(i);

        launcher::installState state = launcher::installDataForGame[i].state;
        if(state != launcher::installState::installed)
        {
            QPushButton* btn = ui->centralwidget->findChild<QPushButton*>("fd" + QString::number(i) + "_btn");
            if(state == launcher::installState::broken)
                btn->setIcon(QIcon(":/gameselect/corrupted/fd" + QString::number(i) + "_btn"));
            else
                btn->setIcon(QIcon(":/gameselect/grayed_out/fd" + QString::number(i) + "_btn"));
        }
    }
}

void GameSelectWindow::showEvent(QShowEvent*)
{
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(500);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameSelectWindow::gameButtonClicked()
{
    QPushButton* btn = (QPushButton*)sender();
    launcher::currentGame = btn->objectName().remove("fd").remove("_btn").toInt();

    bool sendToDownloadWebsite = false;

    switch(launcher::installDataForGame[launcher::currentGame].state)
    {
    case launcher::installState::installed:
    {
        GameWindow* gw = new GameWindow();
        gw->move((this->x() + (this->width() / 2)) - (gw->width() / 2),
                 (this->y() + (this->height() / 2)) - (gw->height() / 2));
        gw->show();
        this->hide();
        break;
    }
    case launcher::installState::broken:
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Installation corrupted."), tr("This installation is corrupted or is missing files. Would you like to reinstall it?"));
        if(reply == QMessageBox::Yes)
            sendToDownloadWebsite = true;
        break;
    }
    case launcher::installState::missing:
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Installation missing."), tr("No installation of this game was found, would you like to download it?"));
        if(reply == QMessageBox::Yes)
            sendToDownloadWebsite = true;
        break;
    }
    case launcher::installState::unreleased:
		QMessageBox::information(this, tr("Not available."), tr("Not released."));
        break;
    }

    if(sendToDownloadWebsite)
    {
        QString url;
        switch (launcher::currentGame)
        {
        case 1:
            url = "https://gamejolt.com/games/five-nights-at-freddy-s-1-doom-mod/228159";
            break;
        case 2:
            url = "https://gamejolt.com/games/five-nights-at-freddy-s-2-doom-mod/228163";
            break;
        case 3:
            url = "https://gamejolt.com/games/five-nights-at-freddy-s-3-doom-mod/228169";
            break;
        case 4:
            url = "https://gamejolt.com/games/five-nights-at-freddy-s-4-doom-mod/230296";
            break;
        }
        QDesktopServices::openUrl(url);
		QApplication::quit();
    }
}

void GameSelectWindow::on_patreon_link_clicked()
{
	/*
	 * just make the button do nothing and call it a day
	QDesktopServices::openUrl(ui->patreon_link->text());
	*/
}

void GameSelectWindow::on_youtube_btn_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.youtube.com/SkorneDemon"));
}

void GameSelectWindow::on_patreon_btn_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.patreon.com/Skornedemon"));
}

void GameSelectWindow::on_info_btn_clicked()
{
    QMessageBox::about(this, tr("About FDLauncher"), tr(
"<p><strong>Five Nights at Freddy&#39;s Doom Launcher</strong></p>"
"<p>Coded by TheGameratorT</p>"
"<p>&nbsp;</p>"
"<p>Built with Qt 5.12.2, libvorbis, libogg and OpenAL Soft.</p>"
"<p><a style=\"color: #FF802B;\" href=\"https://github.com/TheGameratorT/FDLauncher\">https://github.com/TheGameratorT/FDLauncher</a></p>"
));
}

void GameSelectWindow::closeEvent(QCloseEvent* event)
{
	QApplication::quit();
}

GameSelectWindow::~GameSelectWindow()
{
	audio_fade_timer.stop();
	audioFader.clear();
	audioDevice.closeDevice();
    delete ui;
}
