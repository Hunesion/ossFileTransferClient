cmake_minimum_required (VERSION 3.8)


set (CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(ftcwebextension MODULE
            ftcwebkitextension.cpp
            ../dbus/playback.c
            )
TARGET_LINK_LIBRARIES(ftcwebextension ${WEBKIT_EXT_LIBRARIES} json-glib-1.0 -fPIC)