cmake_minimum_required (VERSION 3.8)


add_library(hune_ui 
            ftcuicommon.cpp
            ftcapplication.cpp
            ftcmainwindow.cpp
            )

TARGET_LINK_LIBRARIES(hune_ui ${GTK3_LIBRARIES} ${WEBKIT_LIBRARIES} ${OPENSSL_LIBRARIES} hune_core json-glib-1.0)