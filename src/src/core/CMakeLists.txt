cmake_minimum_required (VERSION 3.8)

add_library(hune_core  
            core.cpp
            base64.cpp
            StringUtils.cpp
            TimeUtils.cpp
            EventManager.cpp
            urlencode.cpp
            JsonUtils.cpp
            FileInfo.cpp
            http.cpp
            Response.cpp
            Logger.cpp
            FileUtils.cpp
            NetUtils.cpp
            sys_utils.cpp
            SocketServer.cpp
            model/ValueObject.cpp
            model/ClientConfig.cpp
            model/GlobalVar.cpp
            model/User.cpp
            model/Properties.cpp
            model/GlobalStruct.cpp
            )

IF (CMAKE_BUILD_TYPE MATCHES Debug)
    TARGET_COMPILE_DEFINITIONS( hune_core PUBLIC -DFTC_DEBUG=1 )
ENDIF()

TARGET_LINK_LIBRARIES(hune_core hune_request ${GTK3_LIBRARIES} ${WEBKIT_LIBRARIES} ${OPENSSL_LIBRARIES} ${X11_LIBRARIES} -luuid -lpthread json-glib-1.0)

