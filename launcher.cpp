#include "launcher.h"

#include "fdsettings.h"

#include <QMetaEnum>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QFile>

int launcher::currentGame = 0;
launcher::installData launcher::installDataForGame[7];

launcher::installData ReadInstallDataToStruct(int gameId)
{
    launcher::installData data;

    QString gameDir = exeDir + "/game/wads/fd" + QString::number(gameId);

    QFile file(gameDir + "/install.dat");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);

    bool filesLineFound = false;
    bool insideFilesArray = false;
    while(!in.atEnd())
    {
        QString line = in.readLine();
        if(line == "version=")
            data.version = line.split("version=")[1].toFloat();
        else if(line == "files=")
            filesLineFound = true;
        else if(filesLineFound && line == "[")
            insideFilesArray = true;
        else if(filesLineFound && line == "]")
            insideFilesArray = false;
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

    QString gameDir = exeDir + "/game/wads/fd" + QString::number(gameId);

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

bool launcher::launchGame(launcher::startupData startData, QWidget* parent)
{
	FDSettings fdsets(launcher::currentGame, parent);
    FDSettings::SettingsData sd = fdsets.getSettingsData();

    QString args;

    args += "-file \"" + exeDir + "/game/wads/global/*\" ";

    for(int i = 0; i < installDataForGame[currentGame].fileNames.count(); i++)
    {
        QString fileName = installDataForGame[currentGame].fileNames[i];
        if(fileName.contains("|MATERIALS") && sd.materials)
        {
            fileName = fileName.remove("|MATERIALS");
            args += "-file \"";
            args += fileName;
            args += "\" ";
        }
        else if(fileName.contains("|LOWPOLY") && sd.lowpoly)
        {
            fileName = fileName.remove("|LOWPOLY");
            args += "-file \"";
            args += fileName;
            args += "\" ";
        }
        else
        {
            args += "-file \"";
            args += fileName;
            args += "\" ";
        }
    }

    args += "-skill 1 ";
    args += "-config \"" + fdsets.getSetsFile() + "\" ";

    switch(startData.type)
    {
    case startupData::startupType::singleplayer:
        args += "-host 1 ";
        args += "-map e1m1 ";
        break;
    case startupData::startupType::host:
        args += "-host ";
        args += QString::number(startData.playerCount);
        args += " ";
        args += "+map ";
        args += startData.map;
        args += " ";
        args += "-netmode ";
        args += QString::number(startData.netmode);
        args += " ";
        args += "-port ";
        args += QString::number(startData.port);
        args += " ";
        break;
    case startupData::startupType::join:
        args += "-join ";
        args += startData.ip;
        args += " ";
        args += "-port ";
        args += QString::number(startData.port);
        args += " ";
        break;
    default:
        break;
    }

    QProcess* gzdoom = new QProcess();
    gzdoom->setProgram(exeDir + "/game/gzdoom");
    gzdoom->setNativeArguments(args);
    if(gzdoom->startDetached())
        return true;
    else
    {
		//QApplication::beep();
        QString errorMsg = QObject::tr("An error has occurred while trying to launch the game.\n\nError details:\n");
        QFile file(gzdoom->program());
        if(!file.exists())
            errorMsg += "\"" + file.fileName() + "\" " + QObject::tr("wasn't found.");
        else
        {
            QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessError>();
            errorMsg += metaEnum.valueToKey(gzdoom->error());
        }
        QMessageBox::critical(parent, QObject::tr("Launch error!"), errorMsg);
        return false;
    }
}
