#include "resolution.h"

#ifdef _WIN32

#include <windows.h>

QVector<resolution::DisplayResolution> resolution::getAvailableDisplayResolutions()
{
	DISPLAY_DEVICE disp;
	DISPLAY_DEVICE adapter;
	DEVMODE mode;
	QVector<DisplayResolution> displayResolutions;
    int i = 0, j = 0, /*display_count = 0,*/ k = 0;

	// Initialize win32 structs.
	memset(&disp, 0, sizeof(DISPLAY_DEVICE));
	disp.cb = sizeof(DISPLAY_DEVICE);
	memset(&adapter, 0, sizeof(DISPLAY_DEVICE));
	adapter.cb = sizeof(DISPLAY_DEVICE);
	memset(&mode, 0, sizeof(DEVMODE));
	mode.dmSize = sizeof(DEVMODE);

	//memset(the_displays, 0, sizeof(the_displays));

	// adapter loop:
	while(EnumDisplayDevices(NULL, i, &adapter, 0))
	{   // multiple displays on single adapter loop:
		j = 0;
		while(EnumDisplayDevices((TCHAR*)&adapter.DeviceName, j, &disp, 0))
		{
			// The device must be active, attached to desktop and not a mirroring device.
			if((disp.StateFlags & DISPLAY_DEVICE_ACTIVE)
				&& (disp.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
				&& !(disp.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
			{
				// read display settings.
				k = 0;
				while(EnumDisplaySettingsEx((TCHAR*)&adapter.DeviceName, k, &mode, 0))
				{
					displayResolutions.push_back( DisplayResolution(mode.dmPelsWidth,
																	mode.dmPelsHeight) );
					// Reset
					memset(&mode, 0, sizeof(DEVMODE));
					mode.dmSize = sizeof(DEVMODE);
                    //++display_count;
					++k;
				}

			}
			// Reset
			memset(&disp, 0, sizeof(DISPLAY_DEVICE));
			disp.cb = sizeof(DISPLAY_DEVICE);
			++j;
		}
		// Reset
		memset(&adapter, 0, sizeof(DISPLAY_DEVICE));
		adapter.cb = sizeof(DISPLAY_DEVICE);
		++i;
	}

	QVector<resolution::DisplayResolution> added;
	for(resolution::DisplayResolution res : displayResolutions)
	{
		if(!added.contains(res))
			added.append(res);
	}
	return added;
}

#else

#include <QMessageBox>
#include <X11/extensions/Xrandr.h>

QVector<resolution::DisplayResolution> resolution::getAvailableDisplayResolutions()
{
	QVector<resolution::DisplayResolution> result;

	Display* dpy = XOpenDisplay(nullptr);
	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);

	int eventBase, errorBase, major, minor;
	if (!XRRQueryExtension(dpy, &eventBase, &errorBase) || !XRRQueryVersion(dpy, &major, &minor))
	{
		QMessageBox::warning(nullptr, "Warning", "Could not fetch available resolutions.\n\nReason: RandR extension missing");
		return result;
	}

	XRRScreenResources* res = XRRGetScreenResourcesCurrent(dpy, root);

	for (int m = 0; m < res->nmode; m++)
	{
		XRRModeInfo* mode = &res->modes[m];

		bool alreadyInList = false;
		for (resolution::DisplayResolution r : result)
		{
			if (r.width == mode->width && r.height == mode->height)
			{
				alreadyInList = true;
				break;
			}
		}
		if (!alreadyInList)
			result.append(resolution::DisplayResolution(mode->width, mode->height));
	}

	return result;
}

#endif
