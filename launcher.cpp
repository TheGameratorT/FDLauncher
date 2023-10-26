#include "launcher.h"

#include "fdsettings.h"

#include <QMetaEnum>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QFile>

#ifdef _WIN32
#define ENGINE_EXE "gzdoom.exe"
#elif __APPLE__
#define ENGINE_EXE "gzdoom_macos"
#else
#define ENGINE_EXE "gzdoom_linux"
#endif

int launcher::currentGame = 0;
launcher::installData launcher::installDataForGame[7];

QString launcher::mapForSelection[4][13] = {
	{"e1m1", "e1m0", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6", "e1m7", "e1m8"},
	{"e1m1", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6", "e2m1", "e2m2", "e2m3", "e2m4", "e2m5", "e2m6", "e2m7"},
	{"e1m1", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6", "e1m7", "e1m8"},
	{"e1m1", "e1m2", "e1m3", "e1m4", "e1m5", "e1m6"}
};

launcher::installData ReadInstallDataToStruct(int gameId)
{
	launcher::installData data;

	QString gameDir = exeDir + "/game/wads/FD" + QString::number(gameId);

	QFile file(gameDir + "/install.dat");
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);

	bool filesLineFound = false;
	bool insideFilesArray = false;
	while(!in.atEnd())
	{
		QString line = in.readLine();
		if(line == "version=")
		{
			data.version = line.split("version=")[1].toFloat();
		}
		else if(line == "files=")
		{
			filesLineFound = true;
		}
		else if(filesLineFound && line == "[")
		{
			insideFilesArray = true;
		}
		else if(filesLineFound && line == "]")
		{
			insideFilesArray = false;
		}
		else if(insideFilesArray)
		{
			QString filePath = gameDir + "/" + line.remove('\t').remove(',');
			if(filePath != "")
				data.fileNames.push_back(filePath);
		}
	}

	file.close();

	return data;
}

void launcher::updateInstallDataForGame(int gameId)
{
	if(gameId >= 5)
	{
		launcher::installDataForGame[gameId].state = installState::unreleased;
		return;
	}

	QString gameDir = exeDir + "/game/wads/FD" + QString::number(gameId);

	if(QFile::exists(gameDir + "/install.dat"))
	{
		launcher::installData data = ReadInstallDataToStruct(gameId);
		installDataForGame[gameId] = data;
		if(data.fileNames.count())
		{
			for(int i = 0; i < data.fileNames.count(); i++)
			{
				QString fileName = data.fileNames[i].remove("|MATERIALS").remove("|LOWPOLY");
				if(!QFile::exists(fileName))
				{
					installDataForGame[gameId].state = installState::broken;
					return;
				}
			}
			installDataForGame[gameId].state = installState::installed;
		}
		else
		{
			installDataForGame[gameId].state = installState::broken;
		}
	}
	else
	{
		installDataForGame[gameId].state = installState::missing;
	}
}

bool launcher::launchGame(const launcher::startupData& startData, QWidget* parent)
{
	bool cheatsOn = enableCheatsKeyDown();

	FDSettings fdsets(currentGame, parent);
	FDSettings::SettingsData sd = fdsets.getSettingsData();

	// WARN: Since FNAF Doom 1 v4.0, materials are always on, remove the line below to revert it (and in settings_dialog.cpp too)
	sd.materials = true;

	QStringList args;

	args += "-file";
	args += (exeDir + "/game/wads/Global/*");

	for(int i = 0; i < installDataForGame[currentGame].fileNames.count(); i++)
	{
		QString fileName = installDataForGame[currentGame].fileNames[i];
		if(fileName.contains("|MATERIALS") && sd.materials)
		{
			fileName = fileName.remove("|MATERIALS");
		}
		else if(fileName.contains("|LOWPOLY") && sd.lowpoly)
		{
			fileName = fileName.remove("|LOWPOLY");
		}
		args += "-file";
		args += fileName;
	}

	args += "-iwad";
	args += (exeDir + "/game/FNAFDoom.ipk3");
	args += "-skill";
	args += "1";
	args += "-config";
	args += fdsets.getSetsFile();

	switch(startData.type)
	{
	case startupData::startupType::singleplayer:
		args += "-host";
		args += "1";
		args += "-map";
		args += mapForSelection[currentGame-1][0];
		break;
	case startupData::startupType::host:
		args += "-host";
		args += QString::number(startData.playerCount);
		args += "+map";
		args += startData.map;
		args += "-netmode";
		args += QString::number(startData.netmode);
		args += "-port";
		args += QString::number(startData.port);
		break;
	case startupData::startupType::join:
		args += "-join";
		args += startData.ip;
		args += "-port";
		args += QString::number(startData.port);
		break;
	default:
		break;
	}

	if (cheatsOn)
	{
		args += "+sv_cheats";
		args += "1";
	}

	QProcess* gzdoom = new QProcess();
	gzdoom->setProgram(exeDir + "/game/" ENGINE_EXE);
	gzdoom->setArguments(args);
	if(gzdoom->startDetached())
	{
		return true;
	}
	else
	{
		//QApplication::beep();
		QString errorMsg = QObject::tr("An error has occurred while trying to launch the game.\n\nError details:\n");
		QFile file(gzdoom->program());
		if(!file.exists())
		{
			errorMsg += "\"" + file.fileName() + "\" " + QObject::tr("wasn't found.");
		}
		else
		{
			QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessError>();
			errorMsg += metaEnum.valueToKey(gzdoom->error());
		}
		QMessageBox::critical(parent, QObject::tr("Launch error!"), errorMsg);
		return false;
	}
}

bool launcher::enableCheatsKeyDown()
{
	return QApplication::keyboardModifiers() & Qt::KeyboardModifier::ShiftModifier;
}
