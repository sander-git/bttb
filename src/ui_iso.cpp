#include "ui_iso.hpp"
#include "bttb_locale.hpp"
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <cstdio>
#include <iostream>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace bttb {

struct IsoDialogData {
    GtkWindow* dialog;
    GtkWidget* src_entry;
    GtkWidget* iso_entry;
    GtkWidget* vol_entry;
    GtkWidget* log_view;
    GtkWidget* gen_button;
    std::thread worker;
};

struct IsoLogPayload {
    GtkTextView* log_view;
    std::string text;
};

static gboolean append_iso_log_idle(gpointer data) {
    auto* payload = static_cast<IsoLogPayload*>(data);
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(payload->log_view);
    GtkTextIter end_iter;
    gtk_text_buffer_get_end_iter(buffer, &end_iter);
    
    gtk_text_buffer_insert(buffer, &end_iter, payload->text.c_str(), -1);
    
    // Auto scroll to bottom
    GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment(
        GTK_SCROLLED_WINDOW(gtk_widget_get_parent(GTK_WIDGET(payload->log_view))));
    if (adj) {
        gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
    }
    
    delete payload;
    return G_SOURCE_REMOVE;
}

static void run_genisoimage(IsoDialogData* data, std::string src_dir, std::string iso_path, std::string vol_label) {
    // Generate ISO command
    std::string cmd = "genisoimage -o \"" + iso_path + "\" -V \"" + vol_label + "\" -J -R \"" + src_dir + "\" 2>&1";
    
    g_idle_add(append_iso_log_idle, new IsoLogPayload{GTK_TEXT_VIEW(data->log_view), "Executing: " + cmd + "\n\n"});
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        g_idle_add(append_iso_log_idle, new IsoLogPayload{GTK_TEXT_VIEW(data->log_view), "ERROR: Failed to run genisoimage process.\n"});
        return;
    }
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        g_idle_add(append_iso_log_idle, new IsoLogPayload{GTK_TEXT_VIEW(data->log_view), std::string(buffer)});
    }
    
    int status = pclose(pipe);
    if (status == 0) {
        g_idle_add(append_iso_log_idle, new IsoLogPayload{GTK_TEXT_VIEW(data->log_view), "\nSUCCESS: ISO Image successfully created.\n"});
    } else {
        g_idle_add(append_iso_log_idle, new IsoLogPayload{GTK_TEXT_VIEW(data->log_view), "\nFAILURE: Process exited with status code " + std::to_string(status) + "\n"});
    }
    
    // Re-enable generate button
    g_idle_add([](gpointer btn_ptr) -> gboolean {
        gtk_widget_set_sensitive(GTK_WIDGET(btn_ptr), TRUE);
        return G_SOURCE_REMOVE;
    }, data->gen_button);
}

void IsoDialog::run(GtkWindow* parent, const std::string& defaultSourceDir) {
    GtkWidget* dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), _T("iso_title", "Create ISO Image").c_str());
    gtk_window_set_default_size(GTK_WINDOW(dialog), 550, 400);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    
    auto* data = new IsoDialogData();
    data->dialog = GTK_WINDOW(dialog);
    
    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(main_box, 16);
    gtk_widget_set_margin_end(main_box, 16);
    gtk_widget_set_margin_top(main_box, 16);
    gtk_widget_set_margin_bottom(main_box, 16);
    gtk_window_set_child(GTK_WINDOW(dialog), main_box);
    
    // Form Grid
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_box_append(GTK_BOX(main_box), grid);
    
    // Source Dir
    GtkWidget* label_src = gtk_label_new(_T("source_folder", "Source Directory:").c_str());
    gtk_widget_set_halign(label_src, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_src, 0, 0, 1, 1);
    
    data->src_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(data->src_entry), defaultSourceDir.c_str());
    gtk_widget_set_hexpand(data->src_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), data->src_entry, 1, 0, 1, 1);
    
    // Target ISO Path
    GtkWidget* label_iso = gtk_label_new(_T("target_iso_file", "Target ISO File:").c_str());
    gtk_widget_set_halign(label_iso, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_iso, 0, 1, 1, 1);
    
    data->iso_entry = gtk_entry_new();
    std::string default_iso = defaultSourceDir.empty() ? "/tmp/output.iso" : (std::filesystem::path(defaultSourceDir).parent_path() / "output.iso").string();
    gtk_editable_set_text(GTK_EDITABLE(data->iso_entry), default_iso.c_str());
    gtk_grid_attach(GTK_GRID(grid), data->iso_entry, 1, 1, 1, 1);
    
    // Volume Label
    GtkWidget* label_vol = gtk_label_new(_T("volume_label", "Volume Label:").c_str());
    gtk_widget_set_halign(label_vol, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_vol, 0, 2, 1, 1);
    
    data->vol_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(data->vol_entry), "BTTB_BACKUP");
    gtk_grid_attach(GTK_GRID(grid), data->vol_entry, 1, 2, 1, 1);
    
    // Logs ScrolledWindow
    GtkWidget* log_frame = gtk_frame_new(_T("execution_log", "Execution Log").c_str());
    gtk_widget_set_vexpand(log_frame, TRUE);
    gtk_box_append(GTK_BOX(main_box), log_frame);
    
    GtkWidget* scroll_win = gtk_scrolled_window_new();
    gtk_frame_set_child(GTK_FRAME(log_frame), scroll_win);
    
    data->log_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(data->log_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(data->log_view), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_win), data->log_view);
    
    // Buttons box
    GtkWidget* action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(action_box, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(main_box), action_box);
    
    GtkWidget* close_btn = gtk_button_new_with_label(_T("close_btn", "Close").c_str());
    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_box_append(GTK_BOX(action_box), close_btn);
    
    data->gen_button = gtk_button_new_with_label(_T("generate_btn", "Generate").c_str());
    gtk_box_append(GTK_BOX(action_box), data->gen_button);
    
    // Handle Generate click
    g_signal_connect(data->gen_button, "clicked", G_CALLBACK(+[](GtkButton* btn, gpointer user_data) {
        auto* data = static_cast<IsoDialogData*>(user_data);
        
        std::string src = gtk_editable_get_text(GTK_EDITABLE(data->src_entry));
        std::string iso = gtk_editable_get_text(GTK_EDITABLE(data->iso_entry));
        std::string vol = gtk_editable_get_text(GTK_EDITABLE(data->vol_entry));
        
        if (src.empty() || iso.empty()) return;
        
        gtk_widget_set_sensitive(GTK_WIDGET(btn), FALSE);
        
        // Clear log
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->log_view));
        gtk_text_buffer_set_text(buffer, "", 0);
        
        // Spawn background ISO creation worker
        if (data->worker.joinable()) {
            data->worker.join();
        }
        data->worker = std::thread(run_genisoimage, data, src, iso, vol);
    }), data);
    
    // Free dialog resources on destroy
    g_signal_connect(dialog, "destroy", G_CALLBACK(+[](GtkWidget* widget, gpointer user_data) {
        auto* data = static_cast<IsoDialogData*>(user_data);
        if (data->worker.joinable()) {
            data->worker.join();
        }
        delete data;
    }), data);
    
    gtk_widget_set_visible(dialog, TRUE);
}

} // namespace bttb
