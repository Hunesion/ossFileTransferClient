
cmake_minimum_required (VERSION 3.8)
add_subdirectory ("core")
add_subdirectory("assets")
add_subdirectory ("ui")
add_subdirectory("webkitextension")
add_executable (ftc main.cpp resources.cpp)
TARGET_LINK_LIBRARIES(ftc ${GTK3_LIBRARIES} ${WEBKIT_LIBRARIES} ${OPENSSL_LIBRARIES} ${UUID_LIBRARIES} hune_core hune_ui json-glib-1.0 pthread)