# FDLauncher
The Five Nights at Freddy's Doom Mod launcher.

### Building
This launcher can only be built using the `CMakeLists.txt`, not the `FDLauncher.pro` file.
The `FDLauncher.pro` file only exists to allow the creation of translation files.

To build the executable you need Qt 5.12.2 Static and MSVC 2019, the static builds of libvorbis, libogg and OpenAL Soft are already included in the repository.

To build the translations execute the `translate.bat` file.

### Note
Some DPI scaling issues might be visible in some monitor settings.
The code might look very different in some places, this is because I started this launcher when I was 15 years old and only continued development of it now 2 years later, so my coding styles changed a bit.
