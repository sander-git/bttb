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

    GtkWidget* activity_spinner;
    GtkWidget* time_left_label;

    // Solver settings (backed up in logic)
    BttbSolver solver;
    std::jthread solver_thread;

    void append_log(const std::string& message, int type);
    void update_progress(double disc_progress, double overall_progress);
    void update_time_left(double secondsLeft);
    void solver_finished();

    struct TreeInsertTask {
        enum class Type {
            CREATE_VOLUME_PARENT,
            CREATE_FILE_CHILD,
            CREATE_SUBPATH_GRANDCHILD,
            CREATE_REMAINING_PARENT,
            CREATE_REMAINING_CHILD,
            CREATE_REMAINING_SUBPATH_GRANDCHILD,
            CREATE_SKIPPED_PARENT,
            CREATE_SKIPPED_CHILD
        };
        Type type;
        int volumeIndex = 0;
        int fileIndex = -1;
        int grandchildIndex = -1;
        int64_t totalBytes = 0;
        std::string path;
        int64_t sizeBytes = 0;
        std::string statusTag;
    };

    std::vector<TreeInsertTask> render_tasks;
    size_t render_task_index = 0;
    GtkTreeIter current_volume_parent_iter;
    GtkTreeIter current_file_child_iter;
    GtkTreeIter current_remaining_parent_iter;
    GtkTreeIter current_remaining_child_iter;
    GtkTreeIter current_skipped_parent_iter;
    GtkTreeIter current_skipped_child_iter;
    std::string importedJsonDir;

    void start_rendering(bool includeUnfitted, const std::string& statusTag = "Fitted");
    bool process_render_batch();
    void set_ui_sensitive(gboolean sensitive);
    void restore_item(int type, int volIdx, int fileIdx, int gcIdx);
    void start_tutorial();
};

} // namespace bttb
