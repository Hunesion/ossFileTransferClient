cmake_minimum_required (VERSION 3.8)


add_library(hune_ui 
            FtcEventCallback.cpp
            ftcuicommon.cpp
            ftcapplication.cpp
            ftcmainwindow.cpp
            ftcloginpage.cpp
            ftchomepage.cpp
            ftchomeheaderrecvbox.cpp
            ftchomeheadersendbox.cpp
            ftchomeheadersendhistorybox.cpp
            ftchomedndsubpage.cpp
            ../dbus/playback.c
            ../webkit/ftcwebkit.cpp
            ../webkit/ftcdbusserver.cpp
            )

TARGET_LINK_LIBRARIES(hune_ui ${GTK3_LIBRARIES} ${WEBKIT_LIBRARIES} ${OPENSSL_LIBRARIES} hune_core json-glib-1.0)