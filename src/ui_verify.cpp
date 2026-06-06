#include "ui_verify.hpp"
#include "bttb_locale.hpp"
#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <cstdio>
#include <iostream>

namespace bttb {

struct VerifyDialogData {
    GtkWindow* dialog;
    GtkWidget* vol_entry;
    GtkWidget* dest_entry;
    GtkWidget* par_entry;
    GtkWidget* log_view;
    GtkWidget* verify_btn;
    GtkWidget* restore_btn;
    std::thread worker;
};

struct VerifyLogPayload {
    GtkTextView* log_view;
    std::string text;
};

static gboolean append_verify_log_idle(gpointer data) {
    auto* payload = static_cast<VerifyLogPayload*>(data);
    
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

static void run_verify_or_restore(VerifyDialogData* data, std::string volume_path, std::string dest_path, std::string par_base, bool is_restore) {
    if (is_restore) {
        g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_start_restore", "Starting volume restoration/repair...\n")});
        g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_volume_path", "Volume path: ") + volume_path + "\n"});
        g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_recovery_path", "Recovery path: ") + dest_path + "\n"});
        g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_par3_base", "PAR3 base name: ") + par_base + "\n"});
        g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_copying_repair", "Copying volume contents and running repair. Please wait...\n")});
        
        std::string logOutput;
        bool res = restoreVolumePar3(volume_path, dest_path, par_base, logOutput);
        
        if (!logOutput.empty()) {
            g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_errors", "Logs/Errors: ") + logOutput + "\n"});
        }
        
        if (res) {
            g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_success_restore", "\nSUCCESS: Volume successfully copied and repaired in separate recovery folder!\n")});
        } else {
            g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_fail_restore", "\nFAILURE: Restoration or repair failed.\n")});
        }
    } else {
        g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_start_verify", "Starting volume verification...\n")});
        g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_volume_path", "Volume path: ") + volume_path + "\n"});
        g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_par3_base", "PAR3 base name: ") + par_base + "\n"});
        
        std::vector<std::string> damaged;
        std::string logOutput;
        int status = verifyVolumePar3(volume_path, par_base, damaged, logOutput);
        
        if (!logOutput.empty()) {
            g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_errors", "Logs/Errors: ") + logOutput + "\n"});
        }
        
        if (status == 0) {
            g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_success_verify", "\nSUCCESS: All files verified and are clean/bit-perfect!\n")});
        } else {
            g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_fail_verify_status", "\nVerification detected issues (status ") + std::to_string(status) + ").\n"});
            if (!damaged.empty()) {
                g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_damaged_files", "Damaged or missing files found:\n")});
                for (const auto& f : damaged) {
                    g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), " * " + f + "\n"});
                }
            } else {
                g_idle_add(append_verify_log_idle, new VerifyLogPayload{GTK_TEXT_VIEW(data->log_view), _T("log_index_fail", "No individual damaged files identified, but the index verification failed. The PAR3 archive itself might be damaged.\n")});
            }
        }
    }
    
    // Re-enable buttons
    g_idle_add([](gpointer user_data) -> gboolean {
        auto* data = static_cast<VerifyDialogData*>(user_data);
        gtk_widget_set_sensitive(data->verify_btn, TRUE);
        gtk_widget_set_sensitive(data->restore_btn, TRUE);
        return G_SOURCE_REMOVE;
    }, data);
}

static void start_verify_worker(VerifyDialogData* data, bool is_restore) {
    std::string vol = gtk_editable_get_text(GTK_EDITABLE(data->vol_entry));
    std::string dest = gtk_editable_get_text(GTK_EDITABLE(data->dest_entry));
    std::string par = gtk_editable_get_text(GTK_EDITABLE(data->par_entry));
    
    if (vol.empty()) {
        return;
    }
    if (is_restore && dest.empty()) {
        return;
    }
    if (par.empty()) {
        return;
    }
    
    gtk_widget_set_sensitive(data->verify_btn, FALSE);
    gtk_widget_set_sensitive(data->restore_btn, FALSE);
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->log_view));
    gtk_text_buffer_set_text(buffer, "", 0);
    
    if (data->worker.joinable()) {
        data->worker.join();
    }
    data->worker = std::thread(run_verify_or_restore, data, vol, dest, par, is_restore);
}

void VerifyDialog::run(GtkWindow* parent) {
    GtkWidget* dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), _T("verify_title", "Verify & Restore Volumes").c_str());
    gtk_window_set_default_size(GTK_WINDOW(dialog), 550, 420);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    
    auto* data = new VerifyDialogData();
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
    
    // Volume Dir
    GtkWidget* label_vol = gtk_label_new(_T("label_vol_directory", "Volume Directory:").c_str());
    gtk_widget_set_halign(label_vol, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_vol, 0, 0, 1, 1);
    
    data->vol_entry = gtk_entry_new();
    gtk_widget_set_hexpand(data->vol_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), data->vol_entry, 1, 0, 1, 1);
    
    GtkWidget* vol_browse = gtk_button_new_with_label(_T("browse_btn", "Browse...").c_str());
    gtk_grid_attach(GTK_GRID(grid), vol_browse, 2, 0, 1, 1);
    
    g_signal_connect(vol_browse, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* data = static_cast<VerifyDialogData*>(user_data);
        GtkFileDialog* file_dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(file_dialog, _T("select_vol_dir_title", "Select Volume Directory").c_str());
        gtk_file_dialog_select_folder(file_dialog, data->dialog, nullptr, +[](GObject* source, GAsyncResult* res, gpointer d) {
            auto* data = static_cast<VerifyDialogData*>(d);
            GError* error = nullptr;
            GFile* file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), res, &error);
            if (file) {
                char* path = g_file_get_path(file);
                gtk_editable_set_text(GTK_EDITABLE(data->vol_entry), path);
                
                // Try to auto-populate PAR3 base name
                std::filesystem::path p(path);
                std::string base_name = p.filename().string();
                if (!base_name.empty()) {
                    gtk_editable_set_text(GTK_EDITABLE(data->par_entry), base_name.c_str());
                }
                
                g_free(path);
                g_object_unref(file);
            }
            if (error) {
                g_error_free(error);
            }
        }, data);
    }), data);
    
    // Recovery Dir
    GtkWidget* label_dest = gtk_label_new(_T("label_rec_directory", "Recovery Directory:").c_str());
    gtk_widget_set_halign(label_dest, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_dest, 0, 1, 1, 1);
    
    data->dest_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), data->dest_entry, 1, 1, 1, 1);
    
    GtkWidget* dest_browse = gtk_button_new_with_label(_T("browse_btn", "Browse...").c_str());
    gtk_grid_attach(GTK_GRID(grid), dest_browse, 2, 1, 1, 1);
    
    g_signal_connect(dest_browse, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* data = static_cast<VerifyDialogData*>(user_data);
        GtkFileDialog* file_dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(file_dialog, _T("select_rec_dir_title", "Select Recovery Directory").c_str());
        gtk_file_dialog_select_folder(file_dialog, data->dialog, nullptr, +[](GObject* source, GAsyncResult* res, gpointer d) {
            auto* data = static_cast<VerifyDialogData*>(d);
            GError* error = nullptr;
            GFile* file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), res, &error);
            if (file) {
                char* path = g_file_get_path(file);
                gtk_editable_set_text(GTK_EDITABLE(data->dest_entry), path);
                g_free(path);
                g_object_unref(file);
            }
            if (error) {
                g_error_free(error);
            }
        }, data);
    }), data);
    
    // PAR3 Base Name
    GtkWidget* label_par = gtk_label_new(_T("label_par3_base", "PAR3 Base Name:").c_str());
    gtk_widget_set_halign(label_par, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_par, 0, 2, 1, 1);
    
    data->par_entry = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(data->par_entry), "Volume_1");
    gtk_grid_attach(GTK_GRID(grid), data->par_entry, 1, 2, 1, 1);
    
    // Logs ScrolledWindow
    GtkWidget* log_frame = gtk_frame_new(_T("log_frame_title_verify", "Verification & Restoration Log").c_str());
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
    
    data->verify_btn = gtk_button_new_with_label(_T("verify_only_btn", "Verify Only").c_str());
    gtk_box_append(GTK_BOX(action_box), data->verify_btn);
    
    data->restore_btn = gtk_button_new_with_label(_T("restore_repair_btn", "Restore & Repair").c_str());
    gtk_box_append(GTK_BOX(action_box), data->restore_btn);
    
    g_signal_connect(data->verify_btn, "clicked", G_CALLBACK(+[](GtkButton* btn, gpointer user_data) {
        start_verify_worker(static_cast<VerifyDialogData*>(user_data), false);
    }), data);
    
    g_signal_connect(data->restore_btn, "clicked", G_CALLBACK(+[](GtkButton* btn, gpointer user_data) {
        start_verify_worker(static_cast<VerifyDialogData*>(user_data), true);
    }), data);
    
    // Free dialog resources on destroy
    g_signal_connect(dialog, "destroy", G_CALLBACK(+[](GtkWidget* widget, gpointer user_data) {
        auto* data = static_cast<VerifyDialogData*>(user_data);
        if (data->worker.joinable()) {
            data->worker.join();
        }
        delete data;
    }), data);
    
    gtk_widget_set_visible(dialog, TRUE);
}

} // namespace bttb
