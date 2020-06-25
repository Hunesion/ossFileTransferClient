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


#include <gtk/gtk.h>
#include "ui/ftcapplication.h"


int main(int argc , char **argv){
    int rv = 0 ; 

    FtcApplication *app = NULL ; 
    app = ftc_application_new(); 
    if (app == NULL) return rv ; 
    rv =  g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app); 
    return rv ; 
}