#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <clocale>
#include "ui_main.hpp"
#include "cli_engine.hpp"

#ifdef __linux__
#include <cstdlib>
#include <cstdint>
#include <unistd.h>
#include <dlfcn.h>

extern "C" {
    uint32_t arc4random(void) {
        uint32_t val = 0;
        if (getentropy(&val, sizeof(val)) == 0) {
            return val;
        }
        return static_cast<uint32_t>(std::rand());
    }

    long int __isoc23_strtol(const char *nptr, char **endptr, int base) {
        typedef long int (*fn_t)(const char*, char**, int);
        static fn_t real_fn = nullptr;
        if (!real_fn) {
            real_fn = (fn_t)dlsym(RTLD_NEXT, "strtol");
        }
        return real_fn(nptr, endptr, base);
    }

    long long int __isoc23_strtoll(const char *nptr, char **endptr, int base) {
        typedef long long int (*fn_t)(const char*, char**, int);
        static fn_t real_fn = nullptr;
        if (!real_fn) {
            real_fn = (fn_t)dlsym(RTLD_NEXT, "strtoll");
        }
        return real_fn(nptr, endptr, base);
    }

    unsigned long int __isoc23_strtoul(const char *nptr, char **endptr, int base) {
        typedef unsigned long int (*fn_t)(const char*, char**, int);
        static fn_t real_fn = nullptr;
        if (!real_fn) {
            real_fn = (fn_t)dlsym(RTLD_NEXT, "strtoul");
        }
        return real_fn(nptr, endptr, base);
    }

    unsigned long long int __isoc23_strtoull(const char *nptr, char **endptr, int base) {
        typedef unsigned long long int (*fn_t)(const char*, char**, int);
        static fn_t real_fn = nullptr;
        if (!real_fn) {
            real_fn = (fn_t)dlsym(RTLD_NEXT, "strtoull");
        }
        return real_fn(nptr, endptr, base);
    }
}
#endif

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
#ifdef __linux__
    setenv("GTK_USE_PORTAL", "0", 1);
#endif
    std::setlocale(LC_ALL, "");
    std::setlocale(LC_NUMERIC, "C");
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
