#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QApplication>
#define exeDir QCoreApplication::applicationDirPath()

class launcher
{
public:
	enum installState
	{
		missing,
		broken,
		installed,
		unreleased
	};

	struct installData
	{
		float version;
		QVector<QString> fileNames;
		installState state;
	};

	struct startupData
	{
		enum startupType
		{
			game,
			singleplayer,
			host,
			join
		} type;

		//HOST TYPE
		int port;
		int playerCount;
		int netmode;
		QString map;

		//JOIN TYPE
		QString ip;
	};

	static int currentGame;
	static installData installDataForGame[7]; //+1 because of padding
    static QString mapForSelection[4][13];
	static void updateInstallDataForGame(int gameId);
	static bool launchGame(const startupData& startData, QWidget* parent = nullptr);
	static bool enableCheatsKeyDown();
};

#endif // LAUNCHER_H
