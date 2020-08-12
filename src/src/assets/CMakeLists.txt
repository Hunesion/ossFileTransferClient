cmake_minimum_required (VERSION 3.8)

set(RESOURCE_LIST
app-tray-menu.ui
ftc-main-window.ui
ftc-login-page.ui
ftc-home-page.ui
ftc-home-dnd-sub-page.ui
ftc-home-header-recv-box.ui
ftc-home-header-send-box.ui
ftc-home-header-send-history-box.ui
ftc-home-recv-list-row.ui
ftc-home-recv-list-sub-page.ui
style.css
ADD-ICON-16PX.png
APPROVAL-ICON-16PX.png
APPROVAL-LIST-ICON-16PX.png
ARROW-DOWN-16PX.png
ARROW-DOWN.png
ARROW-LEFT-ES.png
ARROW-LEFT-IS.png
ARROW-RIGHT-ES.png
ARROW-RIGHT-IS.png
ARROW-UP-16PX.png
ARROW-UP.png
CLOSE-ICON-16PX.png
COPY-ICON-16PX.png
DETAIL-ICON-16PX.png
DOWNLOAD-ICON-ES.png
DOWNLOAD-ICON-IS.png
LIST-ICON-16PX.png
LIST02-ICON-16PX.png
LIST03-ICON.png
LOGOUT-ICON-16PX.png
PASSWORD-ICON-16PX.png
REFRESH-ICON-16PX.png
SEND-ICON-16PX.png
SETTING-ICON-16PX.png
UESER-ICON-ES.png
UESER-ICON-IS.png
URL-ICON-16PX.png
USERCHECK-ICON-16PX.png
USER-ICON-16PX.png
nor-128.png
SNS-ICON-24PX.png
)

compile_gresources(RESOURCE_LIST
XML_OUT
TYPE EMBED_C
PREFIX /com/hunesion/ftcapp/
COMPRESS_ALL
RESOURCES ${RESOURCE_LIST})

add_custom_target(resource ALL DEPENDS ${RESOURCE_LIST})

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
message(${PARENT_DIR})
add_custom_command(TARGET resource 
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy 
                ${CMAKE_CURRENT_BINARY_DIR}/resources.c
                ${PARENT_DIR}/resources.cpp
                )


