cmake_minimum_required(VERSION 3.16)

project(QMusicPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Multimedia)

# SVG
find_package(Qt6 REQUIRED COMPONENTS Svg)

# QSingleApplication
find_library(QSINGLEAPPLICATION_LIBRARY qtsingleapplication PATHS "${CMAKE_SOURCE_DIR}/3rdparty")
find_path(QSINGLEAPPLICATION_INCLUDE_DIR qtsingleapplication.h PATHS "${CMAKE_SOURCE_DIR}/3rdparty/include")

# FFMPEG
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/include)
link_directories(${CMAKE_SOURCE_DIR}/3rdparty)

set(PROJECT_SOURCES
        main.cpp
        mainframe.cpp
        mainframe.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QMusicPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        res.qrc
        appinit.h
        appinit.cpp
        centralwidget.h centralwidget.cpp centralwidget.ui
        icon.rc
        databasemanager.h databasemanager.cpp
        parsemusic.h parsemusic.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QMusicPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(QMusicPlayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QMusicPlayer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# QSingleApplication
target_include_directories(QMusicPlayer PRIVATE ${QSINGLEAPPLICATION_INCLUDE_DIR})

target_link_libraries(QMusicPlayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${QSINGLEAPPLICATION_LIBRARY} Qt6::Sql Qt6::Svg Qt6::Multimedia libavcodec.dll.a libavdevice.dll.a libavfilter.dll.a libavformat.dll.a libavutil.dll.a libswresample.dll.a libswscale.dll.a)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QMusicPlayer)
endif()
set_target_properties(QMusicPlayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/res/icons/logo_32.ico"
)

include(GNUInstallDirs)
install(TARGETS QMusicPlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QMusicPlayer)
endif()
