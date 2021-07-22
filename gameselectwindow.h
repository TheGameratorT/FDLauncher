#ifndef GAMESELECTWINDOW_H
#define GAMESELECTWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "audio.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameSelectWindow; }
QT_END_NAMESPACE

class GameSelectWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameSelectWindow(QWidget *parent = nullptr);
    ~GameSelectWindow();

    static GameSelectWindow* instance;

	AudioFader audioFader;

private:
    Ui::GameSelectWindow *ui;

	AudioDevice audioDevice;

	QTimer audio_fade_timer;

	void closeEvent(QCloseEvent* event);

private slots:
    void gameButtonClicked();

    void showEvent(QShowEvent*);
	void on_info_btn_clicked();
	void on_patreon_link_clicked();
};
#endif // GAMESELECTWINDOW_H
