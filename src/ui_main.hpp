#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include "bttb_logic.hpp"

namespace bttb {

class MainWindow {
public:
    MainWindow(GtkApplication* app, const std::string& initialFolder = "");
    ~MainWindow();

    void show();

    // UI Widgets
    GtkWidget* window;
    GtkWidget* source_entry;
    GtkWidget* target_entry;
    GtkWidget* semantic_entry;
    GtkWidget* move_check;
    GtkWidget* symlink_check;
    GtkWidget* span_check;
    GtkWidget* trace_check;
    GtkWidget* log_text_view;
    GtkWidget* test_button;
    GtkWidget* progress_bar_disc;
    GtkWidget* progress_label_disc;
    GtkWidget* start_button;
    GtkWidget* stop_button;
    GtkWidget* tree_view;
    GtkTreeStore* tree_store;

    // Solver settings (backed up in logic)
    BttbSolver solver;
    std::jthread solver_thread;

    void append_log(const std::string& message, int type);
    void update_progress(double disc_progress, double overall_progress);
    void solver_finished();
};

} // namespace bttb
