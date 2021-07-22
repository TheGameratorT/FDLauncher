#include "gamewindow.h"
#include "ui_gamewindow.h"

#include "launcher.h"
#include "gameselectwindow.h"
#include "multiplayer_dialog.h"
#include "settings_dialog.h"

#include <QFile>
#include <QPainter>

static int bg_static_anim_speed_for_game[6] = {25, 25, 50, 100, 25, 25};
static QString bg_static_anim_path_for_game[6] = {
    "fd1_2_bg_static",
    "fd1_2_bg_static",
    "fd3_bg_static",
    "fd4_bg_static",
    "fd1_2_bg_static",
    "fd1_2_bg_static"
};
static qreal bg_static_anim_opacity_for_game[6] = {0.375, 0.375, 1, 1, 25, 25};

GameWindow::GameWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    this->setAttribute(Qt::WA_DeleteOnClose);

    fd3_anim_index = 0;
    static_anim_frame = 0;
	close_mode = 0;

	QString currentGameStr = QString::number(launcher::currentGame);

	QPixmap bkgnd(":/background/fd" + currentGameStr + "_bg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
	palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

	QPixmap pixmap = QPixmap(":/background/fd" + currentGameStr + "_bg_l");
    pixmap.scaled(ui->bg_layer_label->size());
    ui->bg_layer_label->setPixmap(pixmap);

    static_anim_timer.setSingleShot(false);
    static_anim_timer.start(bg_static_anim_speed_for_game[launcher::currentGame - 1]);
    connect(&static_anim_timer, &QTimer::timeout, this, &GameWindow::static_anim_loop);

    if(launcher::currentGame == 3)
    {
        fd3_anim_timer.setSingleShot(false);
        fd3_anim_timer.start(50);
        connect(&fd3_anim_timer, &QTimer::timeout, this, &GameWindow::fd3_anim_loop);
    }

	// Play music

	QFile bgMusicFile(":/music/fd" + currentGameStr + "_menu");
	bgMusicFile.open(QFile::ReadOnly);
	QByteArray bgMusicData = bgMusicFile.readAll();
	bgMusicFile.close();

	bgMusic = new AudioStream;
	bgMusic->loadOgg(bgMusicData.constData(), bgMusicData.size());
	bgMusic->setLooping(true);
	bgMusic->setVolume(0.0f);
	bgMusic->play();

	AudioFader& audioFader = GameSelectWindow::instance->audioFader;
	audioFader.fadeIn(bgMusic, 0.20f, 0.01f, nullptr);
}

void GameWindow::fd3_anim_loop()
{
    if(fd3_anim_index >= 0 && fd3_anim_index <= 6)
    {
        QPixmap pixmap;
        if(fd3_anim_index < 5)
        {
            pixmap = QPixmap(":/fd3_anim/frame" + QString::number(fd3_anim_index + 1));
        }
        else
        {
            pixmap = QPixmap(":/fd3_anim/frame1");
            fd3_anim_index = -41;
        }
        pixmap.scaled(ui->fd3_anim_label->size());
        ui->fd3_anim_label->setPixmap(pixmap);
    }

    fd3_anim_index++;
}

void GameWindow::static_anim_loop()
{
    QString imgPath = bg_static_anim_path_for_game[launcher::currentGame - 1];
    QPixmap pixmap = QPixmap(":/" + imgPath + "/frame" + QString::number(static_anim_frame + 1));
    pixmap.scaled(ui->static_anim_label->size());
    QPixmap result_pixmap = QPixmap(QSize(650, 300));
    result_pixmap.fill(QColor(0, 0, 0, 0));

    QPainter painter;
    painter.begin(&result_pixmap);
    painter.setOpacity(bg_static_anim_opacity_for_game[launcher::currentGame - 1]);
    painter.drawPixmap(0, 0, pixmap);
    painter.end();

    ui->static_anim_label->setPixmap(result_pixmap);

    if(launcher::currentGame == 3 || launcher::currentGame == 4)
    {
        static_anim_frame = rand() % 7;
    }
    else
    {
        static_anim_frame++;
        if(static_anim_frame > 6)
            static_anim_frame = 0;
    }
}

void GameWindow::on_back_btn_clicked()
{
    GameSelectWindow* gsw = GameSelectWindow::instance;
    gsw->show();
	close_mode = 1;
    this->close();
}

void GameWindow::on_sp_btn_clicked()
{
    launcher::startupData data;
    data.type = data.singleplayer;
    if(launcher::launchGame(data, this))
		close();
}

void GameWindow::on_mp_btn_clicked()
{
    multiplayer_dialog mp_dialog(this);
    mp_dialog.exec();
}

void GameWindow::on_og_btn_clicked()
{
    launcher::startupData data;
    data.type = data.game;
    if(launcher::launchGame(data, this))
		close();
}

void GameWindow::on_sets_btn_clicked()
{
    settings_dialog sw(this);
    sw.exec();
}

void GameWindow::closeEvent(QCloseEvent* event)
{
	if (close_mode == 0)
		close_mode = 2;
}

void GameWindow::fade_music_out(AudioStream* stream)
{
	stream->stop();
	stream->destroy();
	delete stream;
}

void GameWindow::fade_music_out_quit(AudioStream* stream)
{
	stream->stop();
	stream->destroy();
	delete stream;
	QApplication::quit();
}

GameWindow::~GameWindow()
{
	AudioFader& audioFader = GameSelectWindow::instance->audioFader;
	audioFader.fadeOut(bgMusic, 0.0f, 0.001f, close_mode == 2 ? fade_music_out_quit : fade_music_out);
	delete ui;
}
