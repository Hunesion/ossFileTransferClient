cmake_minimum_required (VERSION 3.8)

add_library(hune_core  
            StringUtils.cpp
            TimeUtils.cpp
            EventManager.cpp
            urlencode.cpp
            JsonUtils.cpp
			FileInfo.cpp
            )

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_COMPILE_DEFINITIONS( hune_core PUBLIC -DFTC_DEBUG=1 )
ENDIF()

TARGET_LINK_LIBRARIES(hune_core ${GTK3_LIBRARIES} ${WEBKIT_LIBRARIES} ${OPENSSL_LIBRARIES} ${X11_LIBRARIES} -luuid -lpthread json-glib-1.0)

