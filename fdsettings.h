#ifndef FDSETTINGS_H
#define FDSETTINGS_H

#include <QString>
#include <QWidget>

class FDSettings
{
public:
    FDSettings(int gameId, QWidget *parent = nullptr);

    int gameId;

    struct SettingsData {
        //GlobalSettings
        int gl_ssao;
        int gl_multisample;
        int gl_fxaa;
        int vid_scalemode;
        int vid_scale_customwidth;
        int vid_scale_customheight;
        //Doom.ConsoleVariables
        bool gl_bloom;
        QString language;
        //FDLauncher
        bool materials;
        bool lowpoly;
    };

    SettingsData getSettingsData(bool defaultSets = false);
    void setSettingsData(SettingsData sd);

    QString getSetsPrefix();
    QString getSetsFile();

private:
	QWidget* parent;
};

#endif // FDSETTINGS_H
