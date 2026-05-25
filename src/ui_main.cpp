#include "ui_main.hpp"
#include "ui_settings.hpp"
#include "ui_iso.hpp"
#include <iostream>
#include <filesystem>
#include <cstring>
#include <mutex>
#include <condition_variable>

namespace bttb {

struct LogPayload {
    MainWindow* win;
    std::string message;
    int type;
};

struct ProgressPayload {
    MainWindow* win;
    double disc;
    double overall;
};

class FolderListDialog {
public:
    static void run(GtkWindow* parent, GtkWidget* source_entry) {
        GtkWidget* dialog = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(dialog), "Manage Source Folders");
        gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 350);
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
        
        GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_widget_set_margin_start(vbox, 12);
        gtk_widget_set_margin_end(vbox, 12);
        gtk_widget_set_margin_top(vbox, 12);
        gtk_widget_set_margin_bottom(vbox, 12);
        gtk_window_set_child(GTK_WINDOW(dialog), vbox);
        
        GtkWidget* label = gtk_label_new("Complete list of source folders:");
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(vbox), label);
        
        GtkWidget* scroll = gtk_scrolled_window_new();
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_box_append(GTK_BOX(vbox), scroll);
        
        GtkWidget* list_box = gtk_list_box_new();
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), list_box);
        
        // Parse current entries from source_entry
        std::string current_text = gtk_editable_get_text(GTK_EDITABLE(source_entry));
        std::string path;
        for (char c : current_text) {
            if (c == ';') {
                if (!path.empty()) {
                    GtkWidget* row_label = gtk_label_new(path.c_str());
                    gtk_widget_set_halign(row_label, GTK_ALIGN_START);
                    gtk_list_box_append(GTK_LIST_BOX(list_box), row_label);
                    path.clear();
                }
            } else {
                path += c;
            }
        }
        if (!path.empty()) {
            GtkWidget* row_label = gtk_label_new(path.c_str());
            gtk_widget_set_halign(row_label, GTK_ALIGN_START);
            gtk_list_box_append(GTK_LIST_BOX(list_box), row_label);
        }
        
        // Horizontal box for list action buttons
        GtkWidget* btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        gtk_box_append(GTK_BOX(vbox), btn_box);
        
        GtkWidget* add_btn = gtk_button_new_with_label("Add...");
        gtk_box_append(GTK_BOX(btn_box), add_btn);
        
        GtkWidget* edit_btn = gtk_button_new_with_label("Edit...");
        gtk_box_append(GTK_BOX(btn_box), edit_btn);
        
        GtkWidget* remove_btn = gtk_button_new_with_label("Remove");
        gtk_box_append(GTK_BOX(btn_box), remove_btn);
        
        // Spacing
        GtkWidget* spacer = gtk_label_new("");
        gtk_widget_set_hexpand(spacer, TRUE);
        gtk_box_append(GTK_BOX(btn_box), spacer);
        
        GtkWidget* cancel_btn = gtk_button_new_with_label("Cancel");
        gtk_box_append(GTK_BOX(btn_box), cancel_btn);
        
        GtkWidget* ok_btn = gtk_button_new_with_label("OK");
        gtk_widget_add_css_class(ok_btn, "suggested-action");
        gtk_box_append(GTK_BOX(btn_box), ok_btn);
        
        struct DialogState {
            GtkWidget* dialog;
            GtkWidget* list_box;
            GtkWidget* source_entry;
        };
        auto* state = new DialogState{dialog, list_box, source_entry};
        
        g_signal_connect(dialog, "destroy", G_CALLBACK(+[](GtkWidget*, gpointer data) {
            delete static_cast<DialogState*>(data);
        }), state);
        
        // Add Button Action
        g_signal_connect(add_btn, "clicked", G_CALLBACK(+[](GtkWidget*, gpointer data) {
            auto* s = static_cast<DialogState*>(data);
            GtkFileDialog* file_dialog = gtk_file_dialog_new();
            gtk_file_dialog_set_title(file_dialog, "Add Source Directory");
            gtk_file_dialog_select_folder(file_dialog, GTK_WINDOW(s->dialog), nullptr, +[](GObject* src, GAsyncResult* res, gpointer d) {
                auto* s_inner = static_cast<DialogState*>(d);
                GError* error = nullptr;
                GFile* file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(src), res, &error);
                if (file) {
                    char* path = g_file_get_path(file);
                    GtkWidget* row_label = gtk_label_new(path);
                    gtk_widget_set_halign(row_label, GTK_ALIGN_START);
                    gtk_list_box_append(GTK_LIST_BOX(s_inner->list_box), row_label);
                    g_free(path);
                    g_object_unref(file);
                }
                if (error) g_error_free(error);
            }, s);
        }), state);
        
        g_signal_connect(edit_btn, "clicked", G_CALLBACK((+[](GtkWidget*, gpointer data) {
            auto* s = static_cast<DialogState*>(data);
            GtkListBoxRow* selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(s->list_box));
            if (!selected_row) return;
            
            GtkFileDialog* file_dialog = gtk_file_dialog_new();
            gtk_file_dialog_set_title(file_dialog, "Edit Source Directory");
            
            struct EditState {
                DialogState* ds;
                GtkListBoxRow* row;
            };
            auto* es = new EditState{s, selected_row};
            
            gtk_file_dialog_select_folder(file_dialog, GTK_WINDOW(s->dialog), nullptr, +[](GObject* src, GAsyncResult* res, gpointer d_edit) {
                auto* es_inner = static_cast<EditState*>(d_edit);
                GError* error = nullptr;
                GFile* file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(src), res, &error);
                if (file) {
                    char* path = g_file_get_path(file);
                    GtkWidget* child = gtk_list_box_row_get_child(es_inner->row);
                    if (GTK_IS_LABEL(child)) {
                        gtk_label_set_text(GTK_LABEL(child), path);
                    }
                    g_free(path);
                    g_object_unref(file);
                }
                if (error) g_error_free(error);
                delete es_inner;
            }, es);
        })), state);
        
        // Remove Button Action
        g_signal_connect(remove_btn, "clicked", G_CALLBACK(+[](GtkWidget*, gpointer data) {
            auto* s = static_cast<DialogState*>(data);
            GtkListBoxRow* selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(s->list_box));
            if (selected_row) {
                gtk_list_box_remove(GTK_LIST_BOX(s->list_box), GTK_WIDGET(selected_row));
            }
        }), state);
        
        // Cancel Button Action
        g_signal_connect(cancel_btn, "clicked", G_CALLBACK(+[](GtkWidget*, gpointer data) {
            auto* s = static_cast<DialogState*>(data);
            gtk_window_destroy(GTK_WINDOW(s->dialog));
        }), state);
        
        // OK Button Action
        g_signal_connect(ok_btn, "clicked", G_CALLBACK(+[](GtkWidget*, gpointer data) {
            auto* s = static_cast<DialogState*>(data);
            std::string joined_paths;
            
            int i = 0;
            while (true) {
                GtkListBoxRow* row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(s->list_box), i);
                if (!row) break;
                GtkWidget* child = gtk_list_box_row_get_child(row);
                if (GTK_IS_LABEL(child)) {
                    const char* text = gtk_label_get_text(GTK_LABEL(child));
                    if (text && strlen(text) > 0) {
                        if (!joined_paths.empty()) joined_paths += ";";
                        joined_paths += text;
                    }
                }
                i++;
            }
            
            gtk_editable_set_text(GTK_EDITABLE(s->source_entry), joined_paths.c_str());
            gtk_window_destroy(GTK_WINDOW(s->dialog));
        }), state);
        
        gtk_window_present(GTK_WINDOW(dialog));
    }
};

MainWindow::MainWindow(GtkApplication* app, const std::string& initialFolder) {
    // Create GtkWindow
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Burn to the Brim");
    gtk_window_set_default_size(GTK_WINDOW(window), 850, 600);
    
    // Set custom HeaderBar
    GtkWidget* header = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(window), header);
    
    // Preferences button
    GtkWidget* pref_btn = gtk_button_new_from_icon_name("preferences-system");
    gtk_widget_set_tooltip_text(pref_btn, "Preferences");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), pref_btn);
    
    g_signal_connect(pref_btn, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        SettingsDialog::run(GTK_WINDOW(self->window), self->solver);
    }), this);
    
    // Create ISO button
    GtkWidget* iso_btn = gtk_button_new_from_icon_name("media-optical");
    gtk_widget_set_tooltip_text(iso_btn, "Create ISO image...");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), iso_btn);
    
    g_signal_connect(iso_btn, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        std::string src = gtk_editable_get_text(GTK_EDITABLE(self->source_entry));
        IsoDialog::run(GTK_WINDOW(self->window), src);
    }), this);
    
    // About button
    GtkWidget* about_btn = gtk_button_new_from_icon_name("help-about");
    gtk_widget_set_tooltip_text(about_btn, "About Burn to the Brim");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), about_btn);
    
    g_signal_connect(about_btn, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        
        GtkWidget* about = gtk_about_dialog_new();
        gtk_window_set_transient_for(GTK_WINDOW(about), GTK_WINDOW(self->window));
        
        gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), "Burn to the Brim");
        gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), "3.3.0");
        gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "Copyright \u00a9 2001-2004 Sander Raaijmakers, Elwin Oost and the Burn to the Brim team");
        gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about), GTK_LICENSE_GPL_2_0);
        gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), "https://sourceforge.net/projects/bttb/");
        gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), 
            "Burn to the Brim (BTTB) is a modern C++20 port of the classic Delphi application designed to optimally fit files and folders onto target storage mediums (CDs, DVDs, Blu-rays, or USBs).\n\n"
            "Features in v3.3.0:\n"
            "- Unicode & Long Path (>256 characters) support\n"
            "- Hybrid GUI / CLI integrated binary execution\n"
            "- Windows Explorer Context Menu integration\n"
            "- Smart adaptive capacity recommendation & retrying\n\n"
            "Authors: Sander Raaijmakers, Elwin Oost and the Burn to the Brim team");
            
        // Load and set logo texture
        GFile* file = g_file_new_for_path("src/bttb.png");
        GdkTexture* texture = gdk_texture_new_from_file(file, NULL);
        g_object_unref(file);
        if (texture) {
            gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), GDK_PAINTABLE(texture));
            g_object_unref(texture);
        } else {
            file = g_file_new_for_path("bttb.png");
            texture = gdk_texture_new_from_file(file, NULL);
            g_object_unref(file);
            if (texture) {
                gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), GDK_PAINTABLE(texture));
                g_object_unref(texture);
            }
        }

        gtk_window_present(GTK_WINDOW(about));
    }), this);
    
    // Start / Stop buttons
    start_button = gtk_button_new_with_label("Start");
    gtk_widget_add_css_class(start_button, "suggested-action");
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), start_button);
    
    stop_button = gtk_button_new_with_label("Stop");
    gtk_widget_add_css_class(stop_button, "destructive-action");
    gtk_widget_set_sensitive(stop_button, FALSE);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), stop_button);
    
    // Help button
    GtkWidget* help_btn = gtk_button_new_from_icon_name("help-browser");
    gtk_widget_set_tooltip_text(help_btn, "Help Documentation");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), help_btn);
    
    g_signal_connect(help_btn, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        
        GtkWidget* help_dlg = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(help_dlg), "Help - Burn to the Brim");
        gtk_window_set_default_size(GTK_WINDOW(help_dlg), 520, 420);
        gtk_window_set_modal(GTK_WINDOW(help_dlg), TRUE);
        gtk_window_set_transient_for(GTK_WINDOW(help_dlg), GTK_WINDOW(self->window));
        
        GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_widget_set_margin_start(vbox, 16);
        gtk_widget_set_margin_end(vbox, 16);
        gtk_widget_set_margin_top(vbox, 16);
        gtk_widget_set_margin_bottom(vbox, 16);
        gtk_window_set_child(GTK_WINDOW(help_dlg), vbox);
        
        GtkWidget* label_title = gtk_label_new(nullptr);
        gtk_label_set_markup(GTK_LABEL(label_title), "<b><span size='large'>Burn to the Brim (BTTB) Help Guide</span></b>");
        gtk_widget_set_halign(label_title, GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(vbox), label_title);
        
        GtkWidget* scroll = gtk_scrolled_window_new();
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_box_append(GTK_BOX(vbox), scroll);
        
        GtkWidget* text_view = gtk_text_view_new();
        gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), text_view);
        
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        const char* help_text = 
            "1. Directory Split Depth\n"
            "Determines the folder nesting level at which items are split:\n"
            " - Depth 0 (Default): Top-level files and folders are treated as separate items.\n"
            " - Depth 1: Splitting occurs one level deeper, keeping top-level folders intact but splitting their immediate subfolders.\n\n"
            "2. Max Search Time\n"
            "The maximum seconds the backtracking solver is allowed to run. If reached, the best selection found up to that point is used.\n\n"
            "3. Spanning Slack\n"
            "Allows early solver termination once a volume is packed within this number of bytes from the absolute maximum capacity (e.g. 2048 bytes).\n\n"
            "4. File/Folder Grouping Rules\n"
            "Force matching items to remain grouped together on the same volume (e.g., matching '*.mp3' or regex '^album_.*').\n\n"
            "5. Multiple Source Folders (+)\n"
            "Click '+' to specify multiple source folders. BTTB acts as if they are in a single root folder. Nested source paths are ignored.\n\n"
            "6. Create Symbolic Links\n"
            "Instead of copying/moving files to the target folder, BTTB creates lightweight symbolic links pointing back to your original files.";
            
        gtk_text_buffer_set_text(buffer, help_text, -1);
        
        GtkWidget* close_btn = gtk_button_new_with_label("Close");
        gtk_widget_set_halign(close_btn, GTK_ALIGN_END);
        gtk_box_append(GTK_BOX(vbox), close_btn);
        
        g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_window_destroy), help_dlg);
        
        gtk_window_present(GTK_WINDOW(help_dlg));
    }), this);
    
    // Solver thread triggers
    g_signal_connect(start_button, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        
        std::string src = gtk_editable_get_text(GTK_EDITABLE(self->source_entry));
        std::string dest = gtk_editable_get_text(GTK_EDITABLE(self->target_entry));
        bool move = gtk_check_button_get_active(GTK_CHECK_BUTTON(self->move_check));
        bool symlink = gtk_check_button_get_active(GTK_CHECK_BUTTON(self->symlink_check));
        bool span = gtk_check_button_get_active(GTK_CHECK_BUTTON(self->span_check));
        bool trace = gtk_check_button_get_active(GTK_CHECK_BUTTON(self->trace_check));
        
        if (src.empty()) {
            self->append_log("Error: Source directory list must not be empty.\n", 2);
            return;
        }
        
        // Parse semicolon separated list of source directories
        self->solver.sourceDirectories.clear();
        std::string current;
        for (char c : src) {
            if (c == ';') {
                if (!current.empty()) {
                    self->solver.sourceDirectories.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }
        if (!current.empty()) {
            self->solver.sourceDirectories.push_back(current);
        }
        
        for (const auto& d : self->solver.sourceDirectories) {
            if (!std::filesystem::exists(d)) {
                self->append_log("Error: Source directory does not exist: " + d + "\n", 2);
                return;
            }
        }
        
        self->solver.sourceDirectory = self->solver.sourceDirectories.front(); // backward compatibility fallback
        self->solver.targetDirectory = dest;
        self->solver.moveFiles = move;
        self->solver.createSymlinks = symlink;
        self->solver.spanMultipleVolumes = span;
        self->solver.enableTrace = trace;
        
        // UI updates before starting
        gtk_widget_set_sensitive(self->start_button, FALSE);
        gtk_widget_set_sensitive(self->stop_button, TRUE);
        gtk_widget_set_sensitive(self->source_entry, FALSE);
        gtk_widget_set_sensitive(self->target_entry, FALSE);
        gtk_widget_set_sensitive(self->move_check, FALSE);
        gtk_widget_set_sensitive(self->symlink_check, FALSE);
        gtk_widget_set_sensitive(self->span_check, FALSE);
        gtk_widget_set_sensitive(self->trace_check, FALSE);
        
        // Clear old logs and tree
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->log_text_view));
        gtk_text_buffer_set_text(buffer, "", 0);
        gtk_tree_store_clear(self->tree_store);
        
        // Spawn C++ solver thread
        self->solver_thread = std::jthread([self]() {
            self->solver.run();
            
            // Notify completion to GTK thread
            g_idle_add([](gpointer data) -> gboolean {
                auto* win = static_cast<MainWindow*>(data);
                win->solver_finished();
                return G_SOURCE_REMOVE;
            }, self);
        });
        
    }), this);
    
    g_signal_connect(stop_button, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        self->solver.stopRequested = true;
        self->append_log("\nCancellation requested by user...\n", 2);
        gtk_widget_set_sensitive(self->stop_button, FALSE);
    }), this);
    
    // Main Split Layout
    GtkWidget* paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_window_set_child(GTK_WINDOW(window), paned);
    
    // Sidebar for tree results
    GtkWidget* sidebar_scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(sidebar_scroll, 240, -1);
    gtk_paned_set_start_child(GTK_PANED(paned), sidebar_scroll);
    
    tree_store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tree_store));
    g_object_unref(tree_store);
    
    GtkCellRenderer* text_renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
                                gtk_tree_view_column_new_with_attributes("Relative Path / Category", text_renderer, "text", 0, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
                                gtk_tree_view_column_new_with_attributes("Size", text_renderer, "text", 1, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
                                gtk_tree_view_column_new_with_attributes("Fitted Status", text_renderer, "text", 2, NULL));
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sidebar_scroll), tree_view);
    
    // Main vertical box on the right
    GtkWidget* right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(right_box, 16);
    gtk_widget_set_margin_end(right_box, 16);
    gtk_widget_set_margin_top(right_box, 16);
    gtk_widget_set_margin_bottom(right_box, 16);
    gtk_paned_set_end_child(GTK_PANED(paned), right_box);
    
    // Folder Input section
    GtkWidget* input_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(input_grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(input_grid), 12);
    gtk_box_append(GTK_BOX(right_box), input_grid);
    
    // Source Browse
    GtkWidget* src_label = gtk_label_new("Source Folder:");
    gtk_widget_set_halign(src_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(input_grid), src_label, 0, 0, 1, 1);
    
    GtkWidget* src_add = gtk_button_new_from_icon_name("list-add");
    gtk_widget_set_tooltip_text(src_add, "Manage multiple source folders...");
    gtk_grid_attach(GTK_GRID(input_grid), src_add, 1, 0, 1, 1);
    
    source_entry = gtk_entry_new();
    gtk_widget_set_hexpand(source_entry, TRUE);
    gtk_grid_attach(GTK_GRID(input_grid), source_entry, 2, 0, 1, 1);
    
    GtkWidget* src_browse = gtk_button_new_with_label("Browse...");
    gtk_grid_attach(GTK_GRID(input_grid), src_browse, 3, 0, 1, 1);
    
    g_signal_connect(src_add, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        FolderListDialog::run(GTK_WINDOW(self->window), self->source_entry);
    }), this);
    
    g_signal_connect(src_browse, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        
        GtkFileDialog* dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(dialog, "Select Source Directory");
        
        gtk_file_dialog_select_folder(dialog, GTK_WINDOW(self->window), nullptr, +[](GObject* source, GAsyncResult* res, gpointer data) {
            auto* self = static_cast<MainWindow*>(data);
            GError* error = nullptr;
            GFile* file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), res, &error);
            if (file) {
                char* path = g_file_get_path(file);
                gtk_editable_set_text(GTK_EDITABLE(self->source_entry), path);
                g_free(path);
                g_object_unref(file);
            }
            if (error) {
                g_error_free(error);
            }
        }, self);
    }), this);
    
    // Target Browse
    GtkWidget* dest_label = gtk_label_new("Target Folder:");
    gtk_widget_set_halign(dest_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(input_grid), dest_label, 0, 1, 2, 1);
    
    target_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(input_grid), target_entry, 2, 1, 1, 1);
    
    GtkWidget* dest_browse = gtk_button_new_with_label("Browse...");
    gtk_grid_attach(GTK_GRID(input_grid), dest_browse, 3, 1, 1, 1);
    
    g_signal_connect(dest_browse, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        
        GtkFileDialog* dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(dialog, "Select Target Directory");
        
        gtk_file_dialog_select_folder(dialog, GTK_WINDOW(self->window), nullptr, +[](GObject* source, GAsyncResult* res, gpointer data) {
            auto* self = static_cast<MainWindow*>(data);
            GError* error = nullptr;
            GFile* file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), res, &error);
            if (file) {
                char* path = g_file_get_path(file);
                gtk_editable_set_text(GTK_EDITABLE(self->target_entry), path);
                g_free(path);
                g_object_unref(file);
            }
            if (error) {
                g_error_free(error);
            }
        }, self);
    }), this);
    
    // Checkbox for move
    move_check = gtk_check_button_new_with_label("Move fitted folders/files to target folder");
    gtk_grid_attach(GTK_GRID(input_grid), move_check, 2, 2, 2, 1);
    
    // Checkbox for symlinks
    symlink_check = gtk_check_button_new_with_label("Create symbolic links in target folder (Default)");
    gtk_grid_attach(GTK_GRID(input_grid), symlink_check, 2, 3, 2, 1);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(symlink_check), TRUE);
    
    // Checkbox for span
    span_check = gtk_check_button_new_with_label("Span across multiple volumes (Volume_1, Volume_2, etc.)");
    gtk_grid_attach(GTK_GRID(input_grid), span_check, 2, 4, 2, 1);
    
    // Checkbox for trace
    trace_check = gtk_check_button_new_with_label("Enable detailed solver diagnostic tracing (Trace)");
    gtk_grid_attach(GTK_GRID(input_grid), trace_check, 2, 5, 2, 1);
    
    // Exclusivity between move_check and symlink_check
    g_signal_connect(move_check, "toggled", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        if (gtk_check_button_get_active(GTK_CHECK_BUTTON(self->move_check))) {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(self->symlink_check), FALSE);
        }
    }), this);
    
    g_signal_connect(symlink_check, "toggled", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        if (gtk_check_button_get_active(GTK_CHECK_BUTTON(self->symlink_check))) {
            gtk_check_button_set_active(GTK_CHECK_BUTTON(self->move_check), FALSE);
        }
    }), this);
    
    // Progress Section
    GtkWidget* progress_frame = gtk_frame_new("Fitted Medium capacity");
    gtk_box_append(GTK_BOX(right_box), progress_frame);
    
    GtkWidget* progress_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start(progress_box, 8);
    gtk_widget_set_margin_end(progress_box, 8);
    gtk_widget_set_margin_top(progress_box, 8);
    gtk_widget_set_margin_bottom(progress_box, 8);
    gtk_frame_set_child(GTK_FRAME(progress_frame), progress_box);
    
    progress_bar_disc = gtk_progress_bar_new();
    gtk_box_append(GTK_BOX(progress_box), progress_bar_disc);
    
    progress_label_disc = gtk_label_new("Filled: 0.00%");
    gtk_widget_set_halign(progress_label_disc, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(progress_box), progress_label_disc);
    
    // Rich Text Logs
    GtkWidget* log_frame = gtk_frame_new("Status and Solver Logs");
    gtk_widget_set_vexpand(log_frame, TRUE);
    gtk_box_append(GTK_BOX(right_box), log_frame);
    
    GtkWidget* log_scroll = gtk_scrolled_window_new();
    gtk_frame_set_child(GTK_FRAME(log_frame), log_scroll);
    
    log_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(log_text_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(log_text_view), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(log_scroll), log_text_view);
    
    // Create text tags for colored logs
    GtkTextBuffer* log_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_text_view));
    gtk_text_buffer_create_tag(log_buffer, "normal", "foreground", "black", NULL);
    gtk_text_buffer_create_tag(log_buffer, "success", "foreground", "green", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(log_buffer, "error", "foreground", "darkred", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(log_buffer, "important", "foreground", "blue", "weight", PANGO_WEIGHT_BOLD, NULL);
    
    // Wire thread-safe notifications into solver
    solver.logNotify = [this](const std::string& msg, int type) {
        g_idle_add([](gpointer data) -> gboolean {
            auto* p = static_cast<LogPayload*>(data);
            p->win->append_log(p->message, p->type);
            delete p;
            return G_SOURCE_REMOVE;
        }, new LogPayload{this, msg, type});
    };
    
    solver.progressNotify = [this](double disc, double overall) {
        g_idle_add([](gpointer data) -> gboolean {
            auto* p = static_cast<ProgressPayload*>(data);
            p->win->update_progress(p->disc, p->overall);
            delete p;
            return G_SOURCE_REMOVE;
        }, new ProgressPayload{this, disc, overall});
    };

    struct GtkDialogSync {
        std::mutex mtx;
        std::condition_variable cv;
        bool ready = false;
        bool response = false;
    };

    solver.recommendCapacityNotify = [this](int64_t recommendedBytes) -> bool {
        GtkDialogSync sync;
        
        struct DialogPayload {
            MainWindow* win;
            GtkDialogSync* sync;
            int64_t recommendedBytes;
        };

        g_idle_add([](gpointer data) -> gboolean {
            auto* p = static_cast<DialogPayload*>(data);
            auto* win = p->win;
            auto* cs = p->sync;
            int64_t recommendedBytes = p->recommendedBytes;
            
            double recMB = (double)recommendedBytes / (1024.0 * 1024.0);
            
            GtkWidget* dialog = gtk_message_dialog_new(
                GTK_WINDOW(win->window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_WARNING,
                GTK_BUTTONS_YES_NO,
                "The largest scanned item requires a volume capacity of at least %.2f MB.\n\nWould you like to automatically increase the volume capacity to %.2f MB and retry packing?",
                recMB, recMB
            );
            
            gtk_window_set_title(GTK_WINDOW(dialog), "Volume Capacity Recommendation");
            
            g_signal_connect(dialog, "response", G_CALLBACK(+[](GtkDialog* dlg, int response_id, gpointer user_data) {
                auto* cs_inner = static_cast<GtkDialogSync*>(user_data);
                {
                    std::lock_guard<std::mutex> lock(cs_inner->mtx);
                    cs_inner->response = (response_id == GTK_RESPONSE_YES);
                    cs_inner->ready = true;
                }
                cs_inner->cv.notify_one();
                gtk_window_destroy(GTK_WINDOW(dlg));
            }), cs);
            
            gtk_widget_show(dialog);
            delete p;
            return G_SOURCE_REMOVE;
        }, new DialogPayload{this, &sync, recommendedBytes});
        
        std::unique_lock<std::mutex> lock(sync.mtx);
        sync.cv.wait(lock, [&]() { return sync.ready; });
        return sync.response;
    };

    if (!initialFolder.empty()) {
        gtk_editable_set_text(GTK_EDITABLE(source_entry), initialFolder.c_str());
    }
}

MainWindow::~MainWindow() {
    if (solver_thread.joinable()) {
        solver_thread.join();
    }
}

void MainWindow::show() {
    gtk_widget_set_visible(window, TRUE);
}

void MainWindow::append_log(const std::string& message, int type) {
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_text_view));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    
    const char* tag = "normal";
    if (type == 1) tag = "success";
    else if (type == 2) tag = "error";
    else if (type == 3) tag = "important";
    
    gtk_text_buffer_insert_with_tags_by_name(buffer, &end, (message + "\n").c_str(), -1, tag, NULL);
    
    // Auto scroll log to bottom
    GtkAdjustment* adj = gtk_scrolled_window_get_vadjustment(
        GTK_SCROLLED_WINDOW(gtk_widget_get_parent(log_text_view)));
    if (adj) {
        gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
    }
}

void MainWindow::update_progress(double disc_progress, double overall_progress) {
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar_disc), disc_progress);
    
    char label_buf[64];
    snprintf(label_buf, sizeof(label_buf), "Filled: %.2f%%", disc_progress * 100.0);
    gtk_label_set_text(GTK_LABEL(progress_label_disc), label_buf);
}

void MainWindow::solver_finished() {
    // Re-enable inputs
    gtk_widget_set_sensitive(start_button, TRUE);
    gtk_widget_set_sensitive(stop_button, FALSE);
    gtk_widget_set_sensitive(source_entry, TRUE);
    gtk_widget_set_sensitive(target_entry, TRUE);
    gtk_widget_set_sensitive(move_check, TRUE);
    gtk_widget_set_sensitive(symlink_check, TRUE);
    gtk_widget_set_sensitive(span_check, TRUE);
    gtk_widget_set_sensitive(trace_check, TRUE);
    
    // Join background thread securely
    if (solver_thread.joinable()) {
        solver_thread.join();
    }
    
    // Populate Results Tree
    gtk_tree_store_clear(tree_store);
    
    // 1. Walk through each solved volume in packedVolumes
    for (const auto& vol : solver.packedVolumes) {
        GtkTreeIter vol_parent;
        gtk_tree_store_append(tree_store, &vol_parent, nullptr);
        
        char vol_name[128];
        snprintf(vol_name, sizeof(vol_name), "Volume %d (Total: %.2f MB)", vol.volumeIndex, (double)vol.totalBytes / (1024.0 * 1024.0));
        
        gtk_tree_store_set(tree_store, &vol_parent,
                           0, vol_name,
                           1, "",
                           2, "Fitted",
                           -1);
        
        for (size_t i = 0; i < vol.itemPaths.size(); ++i) {
            GtkTreeIter child;
            gtk_tree_store_append(tree_store, &child, &vol_parent);
            gtk_tree_store_set(tree_store, &child,
                               0, vol.itemPaths[i].c_str(),
                               1, std::to_string(vol.itemSizes[i]).c_str(),
                               2, "Fitted",
                               -1);
        }
    }
    
    // 2. Add remaining (unfitted) items
    if (!solver.itemsToSplit.empty()) {
        GtkTreeIter unfitted_parent;
        gtk_tree_store_append(tree_store, &unfitted_parent, nullptr);
        
        int64_t unfitted_bytes = 0;
        for (const auto& item : solver.itemsToSplit) {
            unfitted_bytes += item->sizeBytes;
        }
        
        char unfitted_label[128];
        snprintf(unfitted_label, sizeof(unfitted_label), "Remaining Items (Total: %.2f MB)", (double)unfitted_bytes / (1024.0 * 1024.0));
        
        gtk_tree_store_set(tree_store, &unfitted_parent,
                           0, unfitted_label,
                           1, "",
                           2, "Not Fitted",
                           -1);
        
        for (const auto& item : solver.itemsToSplit) {
            GtkTreeIter child;
            gtk_tree_store_append(tree_store, &child, &unfitted_parent);
            gtk_tree_store_set(tree_store, &child,
                               0, item->relativePath.c_str(),
                               1, std::to_string(item->sizeBytes).c_str(),
                               2, "Not Fitted",
                               -1);
        }
    }
}

} // namespace bttb
