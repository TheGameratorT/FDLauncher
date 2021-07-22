#include "fdsettings.h"

#include "launcher.h"
#include "ini.h"

#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QObject>
#include <QDir>

FDSettings::FDSettings(int gameId, QWidget* parent)
{
    this->gameId = gameId;
	this->parent = parent;
    if(!QFile::exists(getSetsFile()))
    {
        QFile defSets(getSetsPrefix() + ".ini");
        if(defSets.exists())
        {
            defSets.copy(getSetsFile());
        }
        else
        {
            QMessageBox::critical(parent, QObject::tr("Configuration error."), QObject::tr("The configuration files are missing, please reinstall the game."));
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
			QApplication::quit();
		}
    }
}

QString FDSettings::getSetsPrefix() { return exeDir + "/game/fd" + QString::number(gameId) + "_gzdoom"; }
QString FDSettings::getSetsFile() { return getSetsPrefix() + "-" + QDir::home().dirName() + ".ini"; }

FDSettings::SettingsData FDSettings::getSettingsData(bool defaultSets)
{
    SettingsData sd;
    QString setsFilePath = defaultSets ? getSetsPrefix() + ".ini" : getSetsFile();

	mINI::INIFile file(setsFilePath.toStdWString());
	mINI::INIStructure ini;
	if (!file.read(ini))
	{
		QMessageBox::critical(parent, QObject::tr("Configuration error."), QObject::tr("An error occured trying to open the configuration."));
		QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
		QApplication::quit();
	}

    sd.gl_ssao = QString::fromStdString(ini["GlobalSettings"]["gl_ssao"]).toInt();
    sd.gl_multisample = QString::fromStdString(ini["GlobalSettings"]["gl_multisample"]).toInt();
    sd.gl_fxaa = QString::fromStdString(ini["GlobalSettings"]["gl_fxaa"]).toInt();
    sd.vid_scalemode = QString::fromStdString(ini["GlobalSettings"]["vid_scalemode"]).toInt();
    sd.vid_scale_customwidth = QString::fromStdString(ini["GlobalSettings"]["vid_scale_customwidth"]).toInt();
    sd.vid_scale_customheight = QString::fromStdString(ini["GlobalSettings"]["vid_scale_customheight"]).toInt();
    sd.gl_bloom = QString::fromStdString(ini["Doom.ConsoleVariables"]["gl_bloom"]).toLower() == "true";
    sd.language = QString::fromStdString(ini["Doom.ConsoleVariables"]["language"]);

    sd.materials = QString::fromStdString(ini["FDLauncher"]["materials"]).toLower() == "true";
    sd.lowpoly = QString::fromStdString(ini["FDLauncher"]["lowpoly"]).toLower() == "true";

    return sd;
}

void FDSettings::setSettingsData(FDSettings::SettingsData sd)
{
	mINI::INIFile file(getSetsFile().toStdWString());
    mINI::INIStructure ini;
	if (!file.read(ini))
	{
		QMessageBox::critical(parent, QObject::tr("Configuration error."), QObject::tr("An error occured trying to open the configuration."));
		QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
		QApplication::quit();
	}

    ini["GlobalSettings"]["gl_ssao"] = QString::number(sd.gl_ssao).toStdString();
    ini["GlobalSettings"]["gl_multisample"] = QString::number(sd.gl_multisample).toStdString();
    ini["GlobalSettings"]["gl_fxaa"] = QString::number(sd.gl_fxaa).toStdString();
    ini["GlobalSettings"]["vid_scalemode"] = QString::number(sd.vid_scalemode).toStdString();
    ini["GlobalSettings"]["vid_scale_customwidth"] = QString::number(sd.vid_scale_customwidth).toStdString();
    ini["GlobalSettings"]["vid_scale_customheight"] = QString::number(sd.vid_scale_customheight).toStdString();
    ini["Doom.ConsoleVariables"]["gl_bloom"] = sd.gl_bloom ? "true" : "false";
    ini["Doom.ConsoleVariables"]["language"] = sd.language.toStdString();

    ini["FDLauncher"]["materials"] = sd.materials ? "true" : "false";
    ini["FDLauncher"]["lowpoly"] = sd.lowpoly ? "true" : "false";

    file.write(ini);
}
