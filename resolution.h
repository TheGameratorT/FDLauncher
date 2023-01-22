#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <QCoreApplication>

#include <stdio.h>

#define DEFAULT_DISPLAY_WIDTH 0
#define DEFAULT_DISPLAY_HEIGHT 0
#define DEFAULT_DISPLAY_FREQUENCY 60
#define DEFAULT_DISPLAY_SOURCE 0

class resolution
{
public:

    struct DisplayResolution
    {
        DisplayResolution( int _width = DEFAULT_DISPLAY_WIDTH,
                           int _height = DEFAULT_DISPLAY_HEIGHT)
        {
            width = _width;
            height = _height;
        }

        bool operator==(const DisplayResolution& other) const
        {
            return width == other.width &&
                   height == other.height;
        }

        int width;
        int height;

    };

    static QVector<DisplayResolution> getAvailableDisplayResolutions();
};

#endif // RESOLUTION_H
