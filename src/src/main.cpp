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