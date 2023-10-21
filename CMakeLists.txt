cmake_minimum_required(VERSION 3.5)

project(FDLauncher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED)

if (WIN32)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")

    # find the required packages
    find_package(OpenAL CONFIG REQUIRED)
    find_package(Vorbis CONFIG REQUIRED)

    else()

    # tell openal that the static lib is in use
    add_definitions(-DAL_LIBTYPE_STATIC)

    set(LIB_DIR "${PROJECT_SOURCE_DIR}/libs")
    if(CMAKE_SIZEOF_VOID_P STREQUAL "4")
            set(LIB_ARCH "x86")
    else()
            set(LIB_ARCH "x64")
    endif()
    if (MSVC)
            set(LIB_SUFFIX ".lib")
    else()
            set(LIB_SUFFIX ".a")
    endif()

    add_library(OpenAL::OpenAL STATIC IMPORTED)
    set_target_properties(OpenAL::OpenAL PROPERTIES
            IMPORTED_LOCATION "${LIB_DIR}/OpenAL/${LIB_ARCH}/OpenAL32${LIB_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIB_DIR}/OpenAL/include"
    )

    add_library(Ogg::ogg STATIC IMPORTED)
    set_target_properties(Ogg::ogg PROPERTIES
            IMPORTED_LOCATION "${LIB_DIR}/libogg/${LIB_ARCH}/ogg${LIB_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIB_DIR}/libogg/include"
    )

    add_library(Vorbis::vorbis STATIC IMPORTED)
    set_target_properties(Vorbis::vorbis PROPERTIES
            IMPORTED_LOCATION "${LIB_DIR}/libvorbis/${LIB_ARCH}/vorbis${LIB_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIB_DIR}/libvorbis/include"
    )

    add_library(Vorbis::vorbisenc STATIC IMPORTED)
    set_target_properties(Vorbis::vorbisenc PROPERTIES
            IMPORTED_LOCATION "${LIB_DIR}/libvorbis/${LIB_ARCH}/vorbisenc${LIB_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIB_DIR}/libvorbis/include"
    )

    add_library(Vorbis::vorbisfile STATIC IMPORTED)
    set_target_properties(Vorbis::vorbisfile PROPERTIES
            IMPORTED_LOCATION "${LIB_DIR}/libvorbis/${LIB_ARCH}/vorbisfile${LIB_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIB_DIR}/libvorbis/include"
    )

    endif()

else()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenAL REQUIRED IMPORTED_TARGET openal)
    pkg_check_modules(Ogg REQUIRED IMPORTED_TARGET ogg)
    pkg_check_modules(Vorbis REQUIRED IMPORTED_TARGET vorbis)
    pkg_check_modules(VorbisEnc REQUIRED IMPORTED_TARGET vorbisenc)
    pkg_check_modules(VorbisFile REQUIRED IMPORTED_TARGET vorbisfile)
    pkg_check_modules(Xrandr REQUIRED IMPORTED_TARGET xrandr)
    pkg_check_modules(Xrender REQUIRED IMPORTED_TARGET xrender)

endif()

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} FDLauncher_pt.ts)

set(PROJECT_SOURCES
    fdsettings.cpp
    fdsettings.h
    gameselectwindow.cpp
    gameselectwindow.h
    gameselectwindow.ui
    gamewindow.cpp
    gamewindow.h
    gamewindow.ui
    host_dialog.cpp
    host_dialog.h
    host_dialog.ui
    ini.h
    join_dialog.cpp
    join_dialog.h
    join_dialog.ui
    launcher.cpp
    launcher.h
    main.cpp
    multiplayer_dialog.cpp
    multiplayer_dialog.h
    multiplayer_dialog.ui
    resolution.cpp
    resolution.h
    settings_dialog.cpp
    settings_dialog.h
    settings_dialog.ui
    audio.cpp
    audio.h
    resources.qrc
    app.rc
)

add_executable(FDLauncher WIN32 ${PROJECT_SOURCES})

if (WIN32)

target_link_libraries(FDLauncher PRIVATE
    winmm
    user32
    Qt5::Widgets
    OpenAL::OpenAL
    Ogg::ogg
    Vorbis::vorbis
    Vorbis::vorbisenc
    Vorbis::vorbisfile
)

else()

target_link_libraries(FDLauncher PRIVATE
    Qt5::Widgets
    PkgConfig::OpenAL
    PkgConfig::Ogg
    PkgConfig::Vorbis
    PkgConfig::VorbisEnc
    PkgConfig::VorbisFile
    PkgConfig::Xrandr
    PkgConfig::Xrender
)

endif()

set_target_properties(FDLauncher PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.thegameratort.FDLauncher
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
