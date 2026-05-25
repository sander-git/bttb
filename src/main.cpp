#include <gtk/gtk.h>
#include "ui_main.hpp"
#include "cli_engine.hpp"

static std::string g_initialFolder = "";

static void on_activate(GtkApplication* app, gpointer user_data) {
    auto* main_win = new bttb::MainWindow(app, g_initialFolder);
    main_win->show();
    
    // Bind main window lifecycle to GtkApplication
    g_object_set_data_full(G_OBJECT(app), "main_window", main_win, +[](gpointer data) {
        delete static_cast<bttb::MainWindow*>(data);
    });
}

int main(int argc, char* argv[]) {
    // 1. Detect if CLI mode is triggered
    if (bttb::isCliModeTriggered(argc, argv)) {
        return bttb::runCliEngine(argc, argv);
    }
    
    // 2. Handle GUI mode with an optional initial folder argument
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg != "-gui" && arg != "--gui" && !arg.empty() && arg[0] != '-') {
            g_initialFolder = arg;
            break;
        }
    }
    
    // Strip GUI-specific arguments from argv to avoid GTK GCommandLine warnings
    int gtk_argc = 1;
    char* gtk_argv[2] = { argv[0], nullptr };
    
    GtkApplication* app = gtk_application_new("com.antigravity.bttb", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), nullptr);
    
    int status = g_application_run(G_APPLICATION(app), gtk_argc, gtk_argv);
    g_object_unref(app);
    
    return status;
}
