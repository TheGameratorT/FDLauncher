#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>

#include "audio.h"

namespace Ui {
class GameWindow;
}

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

private slots:
    void fd3_anim_loop();

    void static_anim_loop();

    void on_back_btn_clicked();

    void on_sp_btn_clicked();

    void on_mp_btn_clicked();

    void on_og_btn_clicked();

	void on_sets_btn_clicked();

private:
    Ui::GameWindow *ui;

    int fd3_anim_index;
    QTimer fd3_anim_timer;

    int static_anim_frame;
	QTimer static_anim_timer;

	AudioStream* bgMusic;

	int close_mode;

	void closeEvent(QCloseEvent* event);
	static void fade_music_out(AudioStream* stream);
	static void fade_music_out_quit(AudioStream* stream);
};

#endif // GAMEWINDOW_H
