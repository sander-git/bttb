#include <gtk/gtk.h>
#include "ui_main.hpp"

static void on_activate(GtkApplication* app, gpointer user_data) {
    auto* main_win = new bttb::MainWindow(app);
    main_win->show();
    
    // Bind main window lifecycle to GtkApplication
    g_object_set_data_full(G_OBJECT(app), "main_window", main_win, +[](gpointer data) {
        delete static_cast<bttb::MainWindow*>(data);
    });
}

int main(int argc, char* argv[]) {
    GtkApplication* app = gtk_application_new("com.antigravity.bttb", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), nullptr);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
