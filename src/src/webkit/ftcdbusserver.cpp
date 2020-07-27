/*
* Copyright (c) 2020 Hunesion Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; 
* either version 2.1 of the License, or (at your option) any later version.
* See the file COPYING included with this distribution for more information.
* https://github.com/HuneOpenUp/ossFileTransferClient/blob/master/COPYING
*/

#include "../core/core.h"
#include "../dbus/ftcappdbus.h"
#include "ftcdbusserver.h"
#include "ftcwebkit.h"

static OrgHunesionAppPlayback *s_play_back = NULL;
static guint s_mBusId = -1;

static gboolean on_handle_invoke_client_action_callback(OrgHunesionAppPlayback *interface,
                                                        GDBusMethodInvocation *invocation,
                                                        guint seq , 
                                                        const gchar *json,
                                                        gpointer user_data)
{
    gboolean rv = FALSE;
    FTC_LOG_DEBUG("------------------------> handle_invoke_client_action_callback seq : %d, Msg %s PID = (%d)", seq , json, getPID());
    FTC_LOG_DEBUG("------------------------> IS MAIN THREAD (%s)", ftc_is_main_thread() ? "TRUE" : "FALSE"); 
    ftc_web_kit_dialog_hide(seq, json); 
    org_hunesion_app_playback_complete_invoke_client_action_callback(interface, invocation);
    rv = TRUE;
    return rv;
}

static void on_bus_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    GError *error = NULL;
    FTC_LOG_DEBUG("on_bus_acquired = %s", name);
    s_play_back = org_hunesion_app_playback_skeleton_new();
    if (!s_play_back)
    {
        FTC_LOG("org_hunesion_app_playback_skeleton_new failed");
        return;
    }

    g_signal_connect(s_play_back, "handle-invoke-client-action-callback", G_CALLBACK(on_handle_invoke_client_action_callback), NULL);
    if (!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(s_play_back), connection, FTC_APP_BUS_OBJ_NAME, &error))
    {
        FTC_LOG("g_dbus_interface_skeleton_export failed"); 
        g_object_unref(s_play_back);
        s_play_back = NULL;
    }

    if (error) {
        g_error_free(error);
    }
    
}

static void on_name_acquired(GDBusConnection *connection,
                             const gchar *name,
                             gpointer user_data)
{

    FTC_LOG("on_name_acquired = %s", name);
}

static void on_name_lost(GDBusConnection *connection,
                         const gchar *name,
                         gpointer user_data)
{
    FTC_LOG("on_name_lost = %s", name);
}

bool ftc_dbus_server_init()
{
    bool rv = false;
    s_mBusId = g_bus_own_name(G_BUS_TYPE_SESSION, FTC_APP_BUS_NAME,
                              (GBusNameOwnerFlags)(G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT | G_BUS_NAME_OWNER_FLAGS_REPLACE),
                              on_bus_acquired,
                              on_name_acquired,
                              on_name_lost,
                              NULL,
                              NULL);

    if (s_mBusId <= 0){
        FTC_LOG("g_bus_own_name failed (name : %s)", FTC_APP_BUS_NAME); 
        return rv;
    }
        
    FTC_LOG_DEBUG("ftc_dbus_server_init successful");
    rv = true;
    return rv;
}

bool ftc_dbus_server_destory()
{
    bool rv = false;

    if (s_play_back)
    {
        g_dbus_interface_skeleton_unexport(G_DBUS_INTERFACE_SKELETON(s_play_back));
        s_play_back = NULL;
    }

    if (s_mBusId <= 0)
        return rv;

    g_bus_unown_name(s_mBusId);
    s_mBusId = 0;
    FTC_LOG_DEBUG("ftc_dbus_server_destory successful");
    rv = true;
    return rv;
}