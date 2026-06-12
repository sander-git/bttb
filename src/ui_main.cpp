#include "ui_main.hpp"
#include "ui_settings.hpp"
#include "ui_iso.hpp"
#include "ui_verify.hpp"
#include "bttb_locale.hpp"
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
        gtk_window_set_title(GTK_WINDOW(dialog), _T("manage_src_folders_title", "Manage Source Folders").c_str());
        gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 350);
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
        
        GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
        gtk_widget_set_margin_start(vbox, 12);
        gtk_widget_set_margin_end(vbox, 12);
        gtk_widget_set_margin_top(vbox, 12);
        gtk_widget_set_margin_bottom(vbox, 12);
        gtk_window_set_child(GTK_WINDOW(dialog), vbox);
        
        GtkWidget* label = gtk_label_new(_T("complete_list_src_folders", "Complete list of source folders:").c_str());
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
        
        GtkWidget* add_btn = gtk_button_new_with_label(_T("add_btn", "Add...").c_str());
        gtk_box_append(GTK_BOX(btn_box), add_btn);
        
        GtkWidget* edit_btn = gtk_button_new_with_label(_T("edit_btn", "Edit...").c_str());
        gtk_box_append(GTK_BOX(btn_box), edit_btn);
        
        GtkWidget* remove_btn = gtk_button_new_with_label(_T("remove_btn", "Remove").c_str());
        gtk_box_append(GTK_BOX(btn_box), remove_btn);
        
        // Spacing
        GtkWidget* spacer = gtk_label_new("");
        gtk_widget_set_hexpand(spacer, TRUE);
        gtk_box_append(GTK_BOX(btn_box), spacer);
        
        GtkWidget* cancel_btn = gtk_button_new_with_label(_T("cancel_btn", "Cancel").c_str());
        gtk_box_append(GTK_BOX(btn_box), cancel_btn);
        
        GtkWidget* ok_btn = gtk_button_new_with_label(_T("ok_btn", "OK").c_str());
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
            gtk_file_dialog_set_title(file_dialog, _T("add_src_dir_title", "Add Source Directory").c_str());
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
            gtk_file_dialog_set_title(file_dialog, _T("edit_src_dir_title", "Edit Source Directory").c_str());
            
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

struct HelpCallbackData {
    MainWindow* self;
    GtkWidget* help_dlg;
};

static void on_help_clicked(GtkWidget* btn, gpointer user_data) {
    auto* self = static_cast<MainWindow*>(user_data);
    
    GtkWidget* help_dlg = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(help_dlg), _T("help_title", "Help - Burn to the Brim").c_str());
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
    gtk_label_set_markup(GTK_LABEL(label_title), ("<b><span size='large'>" + _T("help_guide_title", "Burn to the Brim (BTTB) Help Guide") + "</span></b>").c_str());
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
        "Instead of copying/moving files to the target folder, BTTB creates lightweight symbolic links pointing back to your original files.\n\n"
        "7. Neural Semantic Packing & MiniLM Setup Guide\n"
        "By specifying a semantic prompt, BTTB groups files with similar content using context-aware deep learning embeddings.\n"
        "To use the preferred, high-accuracy MiniLM neural model, you must install Python 3 and sentence-transformers:\n"
        " - Step 1: Ensure Python 3 & pip are installed.\n"
        "   (Linux: run 'sudo apt install python3 python3-pip python3-venv')\n"
        "   (Windows: Install from https://www.python.org/ and check 'Add Python to PATH')\n"
        " - Step 2: Install sentence-transformers via terminal or command prompt:\n"
        "   Option A (Recommended for simplicity):\n"
        "     pip install sentence-transformers\n"
        "   Option B (Virtual environment isolation):\n"
        "     python3 -m venv bttb_env\n"
        "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\n"
        "     pip install sentence-transformers\n"
        " - Step 3: Restart Burn to the Brim to automatically load MiniLM! If not found, BTTB falls back gracefully to a localized character TF-IDF projector.";
        
    gtk_text_buffer_set_text(buffer, _T("help_guide_text", help_text).c_str(), -1);
    
    GtkWidget* btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(vbox), btn_box);

    GtkWidget* start_tut_btn = gtk_button_new_with_label(_T("start_tutorial_btn", "Interactive Tutorial").c_str());
    gtk_box_append(GTK_BOX(btn_box), start_tut_btn);

    GtkWidget* close_btn = gtk_button_new_with_label(_T("close_btn", "Close").c_str());
    gtk_box_append(GTK_BOX(btn_box), close_btn);

    auto* cb_data = new HelpCallbackData{self, help_dlg};
    g_object_set_data_full(G_OBJECT(help_dlg), "help_cb_data", cb_data, [](gpointer data) {
        delete static_cast<HelpCallbackData*>(data);
    });

    g_signal_connect(start_tut_btn, "clicked", G_CALLBACK((+[](GtkWidget*, gpointer data) {
        auto* cb = static_cast<HelpCallbackData*>(data);
        MainWindow* self = cb->self;
        GtkWidget* help_dlg = cb->help_dlg;
        gtk_window_destroy(GTK_WINDOW(help_dlg));
        self->start_tutorial();
    })), cb_data);

    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_window_destroy), help_dlg);
    
    gtk_window_present(GTK_WINDOW(help_dlg));
}

struct ContextMenuData {
    MainWindow* self;
    int type;
    int volIdx;
    int fileIdx;
    int gcIdx;
    GtkWidget* popover;
};

static void on_treeview_pressed(GtkGestureClick* gesture, int n_press, double x, double y, gpointer user_data) {
    auto* self = static_cast<MainWindow*>(user_data);
    GtkTreePath* path = nullptr;
    if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(self->tree_view), (int)x, (int)y, &path, nullptr, nullptr, nullptr)) {
        GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->tree_view));
        gtk_tree_selection_select_path(selection, path);
        
        GtkTreeIter iter;
        if (gtk_tree_model_get_iter(GTK_TREE_MODEL(self->tree_store), &iter, path)) {
            int type = -1;
            int volIdx = -1;
            int fileIdx = -1;
            int gcIdx = -1;
            gtk_tree_model_get(GTK_TREE_MODEL(self->tree_store), &iter,
                               3, &type,
                               4, &volIdx,
                               5, &fileIdx,
                               6, &gcIdx,
                               -1);
            
            if (type == 0 || type == 1 || type == 2) {
                GtkWidget* popover = gtk_popover_new();
                gtk_widget_set_parent(popover, self->tree_view);
                
                GdkRectangle rect;
                rect.x = (int)x;
                rect.y = (int)y;
                rect.width = 1;
                rect.height = 1;
                gtk_popover_set_pointing_to(GTK_POPOVER(popover), &rect);
                
                GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
                GtkWidget* button = gtk_button_new_with_label(_T("menu_restore", "Restore to Original Location").c_str());
                gtk_button_set_has_frame(GTK_BUTTON(button), FALSE);
                
                auto* cmd = new ContextMenuData{self, type, volIdx, fileIdx, gcIdx, popover};
                
                g_object_set_data_full(G_OBJECT(popover), "menu_data", cmd, [](gpointer data) {
                    delete static_cast<ContextMenuData*>(data);
                });
                
                g_signal_connect(button, "clicked", G_CALLBACK((+[](GtkWidget*, gpointer data) {
                    auto* pop = GTK_WIDGET(data);
                    auto* cmd = static_cast<ContextMenuData*>(g_object_get_data(G_OBJECT(pop), "menu_data"));
                    if (cmd) {
                        cmd->self->restore_item(cmd->type, cmd->volIdx, cmd->fileIdx, cmd->gcIdx);
                    }
                    gtk_popover_popdown(GTK_POPOVER(pop));
                })), popover);
                
                gtk_box_append(GTK_BOX(box), button);
                gtk_popover_set_child(GTK_POPOVER(popover), box);
                
                g_signal_connect(popover, "closed", G_CALLBACK((+[](GtkWidget* p, gpointer) {
                    gtk_widget_unparent(p);
                })), nullptr);
                
                gtk_popover_popup(GTK_POPOVER(popover));
            }
        }
        gtk_tree_path_free(path);
    }
}

MainWindow::MainWindow(GtkApplication* app, const std::string& initialFolder) {
    std::cout << "[GUI MainWindow] Solver language: '" << solver.language 
              << "', BttbLocale active language: '" << BttbLocale::getInstance().getLanguage() << "'" << std::endl;
    // Apply dark theme preference on startup
    GtkSettings *settings = gtk_settings_get_default();
    g_object_set(settings, "gtk-application-prefer-dark-theme", solver.enableDarkTheme ? TRUE : FALSE, NULL);

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

    // Import JSON Index Button
    GtkWidget* import_btn = gtk_button_new_from_icon_name("document-open");
    gtk_widget_set_tooltip_text(import_btn, "Import JSON Index...");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), import_btn);
    
    g_signal_connect(import_btn, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        
        GtkFileDialog* dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(dialog, "Select JSON Index File");
        
        gtk_file_dialog_open(dialog, GTK_WINDOW(self->window), nullptr, +[](GObject* source, GAsyncResult* res, gpointer data) {
            auto* self = static_cast<MainWindow*>(data);
            GError* error = nullptr;
            GFile* file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), res, &error);
            if (file) {
                char* path = g_file_get_path(file);
                std::string jsonPath = path;
                g_free(path);
                g_object_unref(file);
                
                std::string errMsg;
                if (parseIndexJson(jsonPath, self->solver.packedVolumes, errMsg, &self->solver.skippedFilePaths)) {
                    std::filesystem::path p(utf8Path(jsonPath));
                    self->importedJsonDir = toUtf8Str(p.parent_path());
                    self->append_log("Successfully imported JSON index file: " + jsonPath, 1);
                    
                    self->start_rendering(false, "Imported");
                } else {
                    self->append_log("Failed to parse JSON index: " + errMsg, 3);
                    GtkWidget* msg_dialog = gtk_message_dialog_new(GTK_WINDOW(self->window),
                                                                 GTK_DIALOG_MODAL,
                                                                 GTK_MESSAGE_ERROR,
                                                                 GTK_BUTTONS_OK,
                                                                 "Failed to parse JSON index:\n%s", errMsg.c_str());
                    g_signal_connect(msg_dialog, "response", G_CALLBACK(+[](GtkWidget* dlg, int response, gpointer) {
                        gtk_window_destroy(GTK_WINDOW(dlg));
                    }), nullptr);
                    gtk_window_present(GTK_WINDOW(msg_dialog));
                }
            }
            if (error) {
                g_error_free(error);
            }
        }, self);
    }), this);
    
    // Verify & Restore Button
    GtkWidget* verify_btn = gtk_button_new_from_icon_name("system-run");
    gtk_widget_set_tooltip_text(verify_btn, _T("verify_title", "Verify & Restore Volumes...").c_str());
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), verify_btn);
    
    g_signal_connect(verify_btn, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        VerifyDialog::run(GTK_WINDOW(self->window));
    }), this);
    
    // About button
    GtkWidget* about_btn = gtk_button_new_from_icon_name("help-about");
    gtk_widget_set_tooltip_text(about_btn, _T("about_title", "About Burn to the Brim").c_str());
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), about_btn);
    
    g_signal_connect(about_btn, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        
        GtkWidget* about = gtk_about_dialog_new();
        gtk_window_set_transient_for(GTK_WINDOW(about), GTK_WINDOW(self->window));
        
        gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about), "Burn to the Brim");
        gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), "4.7.0");
        gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "Copyright \u00a9 2001-2026 Sander Raaijmakers, Elwin Oost and the Burn to the Brim team");
        gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about), GTK_LICENSE_GPL_2_0);
        gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), "https://sourceforge.net/projects/bttb/");
        
        const char* default_comments = 
            "Burn to the Brim (BTTB) is a modern C++20 port of the classic Delphi application designed to optimally fit files and folders onto target storage mediums (CDs, DVDs, Blu-rays, or USBs).\n\n"
            "Features in v4.7.0:\n"
            "- Modeless interactive tutorial (translated in 13 languages) with highlighted/focused guidance controls\n"
            "- Resizable Win32 main window layout adjustments & mouse-draggable TreeView splitter bar\n"
            "- GTK4 popover context menu for item restoration under Linux\n"
            "- Bottom-pinned button alignments in preferences dialogs\n"
            "- Brand new high-resolution application icon (bttb.ico) and unified website logo (bttb.png)\n"
            "- Minimized search state stack frames & 16MB Win32 stack limit (fixing 0xC00000FD overflows)\n"
            "- Expanded logging buffer limits to 10MB to avoid trace log truncation\n"
            "- Non-blocking incremental GUI rendering & skip file capacity warnings\n"
            "- Offline JSON Index creation and interactive parser\n"
            "- Optional PAR3 parity file generation and verification\n"
            "- Bit-perfect PAR3 copy-based restoration and repair\n"
            "- Theme support including standard dark theme options on Linux GTK4\n"
            "- Improved Estimated Time Left calculation immediately at startup\n"
            "- Named Custom Volume profiles & dynamic Auto Volume sizing\n"
            "- Settings memory restoring the last selected volume configuration\n"
            "- Rule conflict overrides allowing rule-based or semantic grouping to win\n"
            "- Transfer-rate estimated Time Left & status activity spinner\n"
            "- Entropy-Aware Semantic Packing based on MiniLM embeddings\n"
            "- Explorer Context Menu integration & long path support\n\n"
            "BTTB is fully localized and dynamically translates the entire user interface based on standard gettext .po templates in German, Dutch, French, and Spanish.\n\n"
            "Libraries and Attributions Used:\n"
            "- libpar3 (by Yutaka Sawada, LGPL v2.1+): https://github.com/Parchive/par3cmdline\n"
            "- BLAKE3 (by BLAKE3 team, CC0/Apache-2.0): https://github.com/BLAKE3-team/BLAKE3\n"
            "- Leopard-RS (by Christopher A. Taylor, BSD 3-Clause): https://github.com/catid/leopard\n"
            "- Galois Field library (by James S. Plank): http://web.eecs.utk.edu/~jplank/plank/www/software.html\n\n"
            "Authors: Sander Raaijmakers, Elwin Oost and the Burn to the Brim team";
            
        gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about), _T("about_comments", default_comments).c_str());
            
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
            } else {
                file = g_file_new_for_path("/usr/share/bttb/bttb.png");
                texture = gdk_texture_new_from_file(file, NULL);
                g_object_unref(file);
                if (texture) {
                    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), GDK_PAINTABLE(texture));
                    g_object_unref(texture);
                }
            }
        }

        gtk_window_present(GTK_WINDOW(about));
    }), this);
    
    // Start / Test / Stop buttons
    start_button = gtk_button_new_with_label(_T("start_btn", "Start").c_str());
    gtk_widget_add_css_class(start_button, "suggested-action");
    gtk_widget_set_tooltip_text(start_button, _T("start_tooltip", "Start organizing files into optimal volumes").c_str());
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), start_button);

    test_button = gtk_button_new_with_label(_T("test_btn", "Test").c_str());
    gtk_widget_add_css_class(test_button, "suggested-action");
    gtk_widget_set_tooltip_text(test_button, _T("test_tooltip", "Simulate organizing files and calculate metrics without modifying files on disk").c_str());
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), test_button);
    
    stop_button = gtk_button_new_with_label(_T("stop_btn", "Stop").c_str());
    gtk_widget_add_css_class(stop_button, "destructive-action");
    gtk_widget_set_tooltip_text(stop_button, _T("stop_tooltip", "Stop the current solver or copy operation").c_str());
    gtk_widget_set_sensitive(stop_button, FALSE);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), stop_button);
    
    // Help button
    GtkWidget* help_btn = gtk_button_new_from_icon_name("help-browser");
    gtk_widget_set_tooltip_text(help_btn, _T("help_title", "Help Documentation").c_str());
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), help_btn);
    
    g_signal_connect(help_btn, "clicked", G_CALLBACK(on_help_clicked), this);
    
    // Solver thread triggers
    // Solver thread triggers
    auto start_solver = [](MainWindow* self, bool testMode) {
        std::string src = gtk_editable_get_text(GTK_EDITABLE(self->source_entry));
        std::string dest = gtk_editable_get_text(GTK_EDITABLE(self->target_entry));
        std::string semantic = gtk_editable_get_text(GTK_EDITABLE(self->semantic_entry));
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

        // Semantic Packing
        self->solver.semanticPrompt = semantic;
        self->solver.enableSemanticPacking = !semantic.empty();
        self->solver.testOnlyMode = testMode;
        
        // UI updates before starting
        gtk_spinner_start(GTK_SPINNER(self->activity_spinner));
        gtk_label_set_text(GTK_LABEL(self->time_left_label), "Time Left: Calculating...");

        gtk_widget_set_sensitive(self->stop_button, TRUE);
        gtk_widget_grab_focus(self->stop_button);
        
        gtk_widget_set_sensitive(self->start_button, FALSE);
        gtk_widget_set_sensitive(self->test_button, FALSE);
        gtk_widget_set_sensitive(self->source_entry, FALSE);
        gtk_widget_set_sensitive(self->target_entry, FALSE);
        gtk_widget_set_sensitive(self->semantic_entry, FALSE);
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
    };

    g_signal_connect(start_button, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        auto* launch_fn = static_cast<decltype(start_solver)*>(g_object_get_data(G_OBJECT(btn), "launch_fn"));
        (*launch_fn)(self, false);
    }), this);

    g_signal_connect(test_button, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        auto* launch_fn = static_cast<decltype(start_solver)*>(g_object_get_data(G_OBJECT(btn), "launch_fn"));
        (*launch_fn)(self, true);
    }), this);

    // Save launch lambda inside GTK objects to bypass lifetime constraints
    auto* p_launch = new decltype(start_solver)(start_solver);
    g_object_set_data_full(G_OBJECT(start_button), "launch_fn", p_launch, +[](gpointer data) {
        delete static_cast<decltype(start_solver)*>(data);
    });
    g_object_set_data(G_OBJECT(test_button), "launch_fn", p_launch);
    
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
    
    tree_store = gtk_tree_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tree_store));
    g_object_unref(tree_store);
    
    GtkCellRenderer* text_renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
                                gtk_tree_view_column_new_with_attributes(_T("rel_path_col", "Relative Path / Category").c_str(), text_renderer, "text", 0, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
                                gtk_tree_view_column_new_with_attributes(_T("size_col", "Size").c_str(), text_renderer, "text", 1, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
                                gtk_tree_view_column_new_with_attributes(_T("fitted_status_col", "Fitted Status").c_str(), text_renderer, "text", 2, NULL));
    
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sidebar_scroll), tree_view);
    
    GtkGesture* gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), GDK_BUTTON_SECONDARY);
    g_signal_connect(gesture, "pressed", G_CALLBACK(on_treeview_pressed), this);
    gtk_widget_add_controller(tree_view, GTK_EVENT_CONTROLLER(gesture));
    
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
    GtkWidget* src_label = gtk_label_new(_T("source_folder", "Source Folder:").c_str());
    gtk_widget_set_halign(src_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(input_grid), src_label, 0, 0, 1, 1);
    
    GtkWidget* src_add = gtk_button_new_from_icon_name("list-add");
    gtk_widget_set_tooltip_text(src_add, _T("manage_src_folders_title", "Manage Source Folders").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), src_add, 1, 0, 1, 1);
    
    source_entry = gtk_entry_new();
    gtk_widget_set_hexpand(source_entry, TRUE);
    gtk_widget_set_tooltip_text(source_entry, _T("source_tooltip", "Source directory containing the files to organize").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), source_entry, 2, 0, 1, 1);
    
    GtkWidget* src_browse = gtk_button_new_with_label(_T("browse_btn", "Browse...").c_str());
    gtk_widget_set_tooltip_text(src_browse, _T("browse_src_tooltip", "Browse for source directory").c_str());
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
    GtkWidget* dest_label = gtk_label_new(_T("target_folder", "Target Folder:").c_str());
    gtk_widget_set_halign(dest_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(input_grid), dest_label, 0, 1, 2, 1);
    
    target_entry = gtk_entry_new();
    gtk_widget_set_tooltip_text(target_entry, _T("target_tooltip", "Destination directory where organized volumes will be created").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), target_entry, 2, 1, 1, 1);
    
    GtkWidget* dest_browse = gtk_button_new_with_label(_T("browse_btn", "Browse...").c_str());
    gtk_widget_set_tooltip_text(dest_browse, _T("browse_dest_tooltip", "Browse for destination directory").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), dest_browse, 3, 1, 1, 1);
    
    g_signal_connect(dest_browse, "clicked", G_CALLBACK(+[](GtkWidget* btn, gpointer user_data) {
        auto* self = static_cast<MainWindow*>(user_data);
        
        GtkFileDialog* dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(dialog, _T("select_dest_dir_title", "Select Target Directory").c_str());
        
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
    
    // Semantic Prompt (v4.0.0)
    GtkWidget* semantic_label = gtk_label_new(_T("semantic_prompt", "Semantic Prompt:").c_str());
    gtk_widget_set_halign(semantic_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(input_grid), semantic_label, 0, 2, 2, 1);
    
    semantic_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(semantic_entry), _T("semantic_placeholder", "e.g. keep similar content together").c_str());
    gtk_widget_set_tooltip_text(semantic_entry, _T("semantic_tooltip", "Specify a semantic description to cluster similar files using MiniLM AI").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), semantic_entry, 2, 2, 2, 1);

    // Checkbox for move
    move_check = gtk_check_button_new_with_label(_T("move_files_chk", "Move fitted folders/files to target folder").c_str());
    gtk_widget_set_tooltip_text(move_check, _T("move_tooltip", "Move files to their destination volumes instead of copying or symlinking them").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), move_check, 2, 3, 2, 1);
    
    // Checkbox for symlinks
    symlink_check = gtk_check_button_new_with_label(_T("create_symlinks_chk", "Create symbolic links in target folder (Default)").c_str());
    gtk_widget_set_tooltip_text(symlink_check, _T("symlink_tooltip", "Create symbolic links of the files at their destination instead of copying them").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), symlink_check, 2, 4, 2, 1);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(symlink_check), TRUE);
    
    // Checkbox for span
    span_check = gtk_check_button_new_with_label(_T("span_chk", "Span across multiple volumes (Volume_1, Volume_2, etc.)").c_str());
    gtk_widget_set_tooltip_text(span_check, _T("span_tooltip", "Fit remaining files into multiple volumes sequentially instead of just the first volume").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), span_check, 2, 5, 2, 1);
    
    // Checkbox for trace
    trace_check = gtk_check_button_new_with_label(_T("trace_chk", "Enable detailed solver diagnostic tracing (Trace)").c_str());
    gtk_widget_set_tooltip_text(trace_check, _T("trace_tooltip", "Show extra verbose tracing and performance metrics in the logs").c_str());
    gtk_grid_attach(GTK_GRID(input_grid), trace_check, 2, 6, 2, 1);
    
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
    GtkWidget* progress_frame = gtk_frame_new(_T("progress_frame_title", "Fitted Medium capacity").c_str());
    gtk_box_append(GTK_BOX(right_box), progress_frame);
    
    GtkWidget* progress_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start(progress_box, 8);
    gtk_widget_set_margin_end(progress_box, 8);
    gtk_widget_set_margin_top(progress_box, 8);
    gtk_widget_set_margin_bottom(progress_box, 8);
    gtk_frame_set_child(GTK_FRAME(progress_frame), progress_box);
    
    progress_bar_disc = gtk_progress_bar_new();
    gtk_box_append(GTK_BOX(progress_box), progress_bar_disc);
    
    progress_label_disc = gtk_label_new(_T("filled_label", "Filled: 0.00%").c_str());
    gtk_widget_set_halign(progress_label_disc, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(progress_box), progress_label_disc);
    
    // Rich Text Logs
    GtkWidget* log_frame = gtk_frame_new(_T("log_frame_title", "Status and Solver Logs").c_str());
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
 
    // Status bar with Spinner and Time Left
    GtkWidget* status_bar_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_start(status_bar_box, 8);
    gtk_widget_set_margin_end(status_bar_box, 8);
    gtk_widget_set_margin_top(status_bar_box, 4);
    gtk_widget_set_margin_bottom(status_bar_box, 4);
    gtk_box_append(GTK_BOX(right_box), status_bar_box);
 
    activity_spinner = gtk_spinner_new();
    gtk_box_append(GTK_BOX(status_bar_box), activity_spinner);
 
    time_left_label = gtk_label_new(_T("time_left_label", "Time Left: --:--").c_str());
    gtk_widget_set_halign(time_left_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(status_bar_box), time_left_label);
    
    // Wire thread-safe notifications into solver
    solver.logNotify = [this](const std::string& msg, int type) {
        g_idle_add([](gpointer data) -> gboolean {
            auto* p = static_cast<LogPayload*>(data);
            p->win->append_log(p->message, p->type);
            delete p;
            return G_SOURCE_REMOVE;
        }, new LogPayload{this, msg, type});
    };

    struct TimeLeftPayload {
        MainWindow* win;
        double secondsLeft;
    };
    solver.timeLeftNotify = [this](double secondsLeft) {
        g_idle_add([](gpointer data) -> gboolean {
            auto* p = static_cast<TimeLeftPayload*>(data);
            p->win->update_time_left(p->secondsLeft);
            delete p;
            return G_SOURCE_REMOVE;
        }, new TimeLeftPayload{this, secondsLeft});
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
        CapacityRecommendResult response = CapacityRecommendResult::CANCEL;
    };

    solver.recommendCapacityNotify = [this](int64_t recommendedBytes) -> CapacityRecommendResult {
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
            
            char msgBuf[512];
            std::string promptFormat = _T("capacity_recommend_prompt_gtk", "The largest scanned item requires a volume capacity of at least %.2f MB.\n\nChoose an action:");
            snprintf(msgBuf, sizeof(msgBuf), promptFormat.c_str(), recMB);

            GtkWidget* dialog = gtk_message_dialog_new(
                GTK_WINDOW(win->window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_WARNING,
                GTK_BUTTONS_NONE,
                "%s",
                msgBuf
            );
            
            gtk_window_set_title(GTK_WINDOW(dialog), "Volume Capacity Recommendation");
            
            gtk_dialog_add_button(GTK_DIALOG(dialog), _T("btn_resize", "Resize").c_str(), GTK_RESPONSE_YES);
            gtk_dialog_add_button(GTK_DIALOG(dialog), _T("btn_skip", "Skip Files").c_str(), GTK_RESPONSE_ACCEPT);
            gtk_dialog_add_button(GTK_DIALOG(dialog), _T("btn_cancel", "Cancel").c_str(), GTK_RESPONSE_NO);
            
            g_signal_connect(dialog, "response", G_CALLBACK(+[](GtkDialog* dlg, int response_id, gpointer user_data) {
                auto* cs_inner = static_cast<GtkDialogSync*>(user_data);
                {
                    std::lock_guard<std::mutex> lock(cs_inner->mtx);
                    if (response_id == GTK_RESPONSE_YES) {
                        cs_inner->response = CapacityRecommendResult::RESIZE;
                    } else if (response_id == GTK_RESPONSE_ACCEPT) {
                        cs_inner->response = CapacityRecommendResult::SKIP_LARGER;
                    } else {
                        cs_inner->response = CapacityRecommendResult::CANCEL;
                    }
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
    
    char label_buf[128];
    snprintf(label_buf, sizeof(label_buf), "Filled: %.2f%%", disc_progress * 100.0);
    gtk_label_set_text(GTK_LABEL(progress_label_disc), label_buf);
}

void MainWindow::update_time_left(double secondsLeft) {
    if (secondsLeft < 0) {
        gtk_label_set_text(GTK_LABEL(time_left_label), "Time Left: Estimating...");
    } else {
        int mins = static_cast<int>(secondsLeft) / 60;
        int secs = static_cast<int>(secondsLeft) % 60;
        char buf[64];
        snprintf(buf, sizeof(buf), "Time Left: %02d:%02d", mins, secs);
        gtk_label_set_text(GTK_LABEL(time_left_label), buf);
    }
}

void MainWindow::set_ui_sensitive(gboolean sensitive) {
    gtk_widget_set_sensitive(start_button, sensitive);
    gtk_widget_set_sensitive(test_button, sensitive);
    if (sensitive) {
        gtk_widget_grab_focus(start_button);
        gtk_widget_set_sensitive(stop_button, FALSE);
    } else {
        gtk_widget_set_sensitive(stop_button, TRUE);
    }
    gtk_widget_set_sensitive(source_entry, sensitive);
    gtk_widget_set_sensitive(target_entry, sensitive);
    gtk_widget_set_sensitive(move_check, sensitive);
    gtk_widget_set_sensitive(symlink_check, sensitive);
    gtk_widget_set_sensitive(span_check, sensitive);
    gtk_widget_set_sensitive(trace_check, sensitive);
    gtk_widget_set_sensitive(semantic_entry, sensitive);
}

void MainWindow::start_rendering(bool includeUnfitted, const std::string& statusTag) {
    // Empty the tree store
    gtk_tree_store_clear(tree_store);
    render_tasks.clear();
    render_task_index = 0;

    // 1. Walk through each solved volume in packedVolumes
    for (const auto& vol : solver.packedVolumes) {
        TreeInsertTask task;
        task.type = TreeInsertTask::Type::CREATE_VOLUME_PARENT;
        task.volumeIndex = vol.volumeIndex;
        task.totalBytes = vol.totalBytes;
        task.statusTag = statusTag;
        render_tasks.push_back(task);
        
        for (size_t i = 0; i < vol.itemPaths.size(); ++i) {
            TreeInsertTask child;
            child.type = TreeInsertTask::Type::CREATE_FILE_CHILD;
            child.path = vol.itemPaths[i];
            child.sizeBytes = vol.itemSizes[i];
            child.volumeIndex = vol.volumeIndex;
            child.fileIndex = (int)i;
            child.statusTag = statusTag;
            render_tasks.push_back(child);
            
            if (i < vol.itemGroupedPaths.size() && !vol.itemGroupedPaths[i].empty()) {
                for (size_t j = 0; j < vol.itemGroupedPaths[i].size(); ++j) {
                    TreeInsertTask subChild;
                    subChild.type = TreeInsertTask::Type::CREATE_SUBPATH_GRANDCHILD;
                    subChild.path = vol.itemGroupedPaths[i][j];
                    subChild.volumeIndex = vol.volumeIndex;
                    subChild.fileIndex = (int)i;
                    subChild.grandchildIndex = (int)j;
                    subChild.statusTag = statusTag;
                    render_tasks.push_back(subChild);
                }
            }
        }
    }
    
    // 2. Add remaining (unfitted) items if requested
    if (includeUnfitted && !solver.itemsToSplit.empty()) {
        int64_t unfitted_bytes = 0;
        for (const auto& item : solver.itemsToSplit) {
            unfitted_bytes += item->sizeBytes;
        }
        
        TreeInsertTask task;
        task.type = TreeInsertTask::Type::CREATE_REMAINING_PARENT;
        task.totalBytes = unfitted_bytes;
        render_tasks.push_back(task);
        
        for (const auto& item : solver.itemsToSplit) {
            TreeInsertTask child;
            child.type = TreeInsertTask::Type::CREATE_REMAINING_CHILD;
            child.path = item->relativePath;
            child.sizeBytes = item->sizeBytes;
            render_tasks.push_back(child);
            
            if (!item->groupedPaths.empty()) {
                for (const auto& subPath : item->groupedPaths) {
                    TreeInsertTask subChild;
                    subChild.type = TreeInsertTask::Type::CREATE_REMAINING_SUBPATH_GRANDCHILD;
                    subChild.path = subPath;
                    render_tasks.push_back(subChild);
                }
            }
        }
    }

    // 3. Add skipped items if they exist
    if (!solver.skippedFilePaths.empty()) {
        int64_t skipped_bytes = 0;
        std::vector<std::pair<std::string, int64_t>> skipped_files;
        for (const auto& absPath : solver.skippedFilePaths) {
            std::error_code ec;
            int64_t sz = std::filesystem::file_size(absPath, ec);
            if (ec) sz = 0;
            skipped_bytes += sz;
            skipped_files.push_back({absPath, sz});
        }

        TreeInsertTask task;
        task.type = TreeInsertTask::Type::CREATE_SKIPPED_PARENT;
        task.totalBytes = skipped_bytes;
        render_tasks.push_back(task);

        for (const auto& sf : skipped_files) {
            TreeInsertTask child;
            child.type = TreeInsertTask::Type::CREATE_SKIPPED_CHILD;
            child.path = sf.first;
            child.sizeBytes = sf.second;
            render_tasks.push_back(child);
        }
    }

    if (render_tasks.empty()) {
        // Nothing to render
        gtk_spinner_stop(GTK_SPINNER(activity_spinner));
        gtk_label_set_text(GTK_LABEL(time_left_label), "Time Left: Complete");
        set_ui_sensitive(TRUE);
        return;
    }

    // Set UI to insensitive but keep spinner spinning
    set_ui_sensitive(FALSE);
    gtk_spinner_start(GTK_SPINNER(activity_spinner));
    gtk_label_set_text(GTK_LABEL(time_left_label), "Time Left: Rendering results...");

    // Register idle task to render batches
    g_idle_add([](gpointer data) -> gboolean {
        MainWindow* self = static_cast<MainWindow*>(data);
        return self->process_render_batch() ? G_SOURCE_CONTINUE : G_SOURCE_REMOVE;
    }, this);
}

bool MainWindow::process_render_batch() {
    size_t processed = 0;
    const size_t batch_size = 100;
    
    while (render_task_index < render_tasks.size() && processed < batch_size) {
        const auto& task = render_tasks[render_task_index];
        switch (task.type) {
            case TreeInsertTask::Type::CREATE_VOLUME_PARENT: {
                gtk_tree_store_append(tree_store, &current_volume_parent_iter, nullptr);
                char vol_name[128];
                snprintf(vol_name, sizeof(vol_name), "Volume %d (Total: %.2f MB)", task.volumeIndex, (double)task.totalBytes / (1024.0 * 1024.0));
                gtk_tree_store_set(tree_store, &current_volume_parent_iter,
                                   0, vol_name,
                                   1, "",
                                   2, task.statusTag.c_str(),
                                   3, (int)task.type,
                                   4, task.volumeIndex,
                                   5, -1,
                                   6, -1,
                                   -1);
                break;
            }
            case TreeInsertTask::Type::CREATE_FILE_CHILD: {
                gtk_tree_store_append(tree_store, &current_file_child_iter, &current_volume_parent_iter);
                gtk_tree_store_set(tree_store, &current_file_child_iter,
                                   0, task.path.c_str(),
                                   1, std::to_string(task.sizeBytes).c_str(),
                                   2, task.statusTag.c_str(),
                                   3, (int)task.type,
                                   4, task.volumeIndex,
                                   5, task.fileIndex,
                                   6, -1,
                                   -1);
                break;
            }
            case TreeInsertTask::Type::CREATE_SUBPATH_GRANDCHILD: {
                GtkTreeIter subChild;
                gtk_tree_store_append(tree_store, &subChild, &current_file_child_iter);
                gtk_tree_store_set(tree_store, &subChild,
                                   0, task.path.c_str(),
                                   1, "",
                                   2, task.statusTag.c_str(),
                                   3, (int)task.type,
                                   4, task.volumeIndex,
                                   5, task.fileIndex,
                                   6, task.grandchildIndex,
                                   -1);
                break;
            }
            case TreeInsertTask::Type::CREATE_REMAINING_PARENT: {
                gtk_tree_store_append(tree_store, &current_remaining_parent_iter, nullptr);
                char unfitted_label[128];
                snprintf(unfitted_label, sizeof(unfitted_label), "Remaining Items (Total: %.2f MB)", (double)task.totalBytes / (1024.0 * 1024.0));
                gtk_tree_store_set(tree_store, &current_remaining_parent_iter,
                                   0, unfitted_label,
                                   1, "",
                                   2, "Not Fitted",
                                   3, (int)task.type,
                                   4, -1,
                                   5, -1,
                                   6, -1,
                                   -1);
                break;
            }
            case TreeInsertTask::Type::CREATE_REMAINING_CHILD: {
                gtk_tree_store_append(tree_store, &current_remaining_child_iter, &current_remaining_parent_iter);
                gtk_tree_store_set(tree_store, &current_remaining_child_iter,
                                   0, task.path.c_str(),
                                   1, std::to_string(task.sizeBytes).c_str(),
                                   2, "Not Fitted",
                                   3, (int)task.type,
                                   4, -1,
                                   5, -1,
                                   6, -1,
                                   -1);
                break;
            }
            case TreeInsertTask::Type::CREATE_REMAINING_SUBPATH_GRANDCHILD: {
                GtkTreeIter subChild;
                gtk_tree_store_append(tree_store, &subChild, &current_remaining_child_iter);
                gtk_tree_store_set(tree_store, &subChild,
                                   0, task.path.c_str(),
                                   1, "",
                                   2, "Not Fitted",
                                   3, (int)task.type,
                                   4, -1,
                                   5, -1,
                                   6, -1,
                                   -1);
                break;
            }
            case TreeInsertTask::Type::CREATE_SKIPPED_PARENT: {
                gtk_tree_store_append(tree_store, &current_skipped_parent_iter, nullptr);
                char skipped_label[128];
                snprintf(skipped_label, sizeof(skipped_label), "Skipped Items (Total: %.2f MB)", (double)task.totalBytes / (1024.0 * 1024.0));
                gtk_tree_store_set(tree_store, &current_skipped_parent_iter,
                                   0, skipped_label,
                                   1, "",
                                   2, "Skipped",
                                   3, (int)task.type,
                                   4, -1,
                                   5, -1,
                                   6, -1,
                                   -1);
                break;
            }
            case TreeInsertTask::Type::CREATE_SKIPPED_CHILD: {
                gtk_tree_store_append(tree_store, &current_skipped_child_iter, &current_skipped_parent_iter);
                std::filesystem::path p(utf8Path(task.path));
                std::string filename = toUtf8Str(p.filename());
                gtk_tree_store_set(tree_store, &current_skipped_child_iter,
                                   0, filename.c_str(),
                                   1, std::to_string(task.sizeBytes).c_str(),
                                   2, "Skipped",
                                   3, (int)task.type,
                                   4, -1,
                                   5, -1,
                                   6, -1,
                                   -1);
                break;
            }
        }
        render_task_index++;
        processed++;
    }
    
    if (render_task_index >= render_tasks.size()) {
        // Complete
        gtk_spinner_stop(GTK_SPINNER(activity_spinner));
        gtk_label_set_text(GTK_LABEL(time_left_label), "Time Left: Complete");
        gtk_tree_view_expand_all(GTK_TREE_VIEW(tree_view));
        set_ui_sensitive(TRUE);
        return false; // Stop idle loop
    }
    
    return true; // Continue idle loop
}

void MainWindow::solver_finished() {
    // Join background thread securely
    if (solver_thread.joinable()) {
        solver_thread.join();
    }
    
    start_rendering(true, "Fitted");
}

void MainWindow::restore_item(int type, int volIdx, int fileIdx, int gcIdx) {
    const PackedVolume* pVol = nullptr;
    if (volIdx >= 0) {
        for (const auto& vol : solver.packedVolumes) {
            if (vol.volumeIndex == volIdx) {
                pVol = &vol;
                break;
            }
        }
    }
    
    if (!pVol) {
        GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(window),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_OK,
                                               "%s", _T("msg_restore_no_volume", "Volume information not found.").c_str());
        g_signal_connect(dlg, "response", G_CALLBACK(+[](GtkWidget* d, int, gpointer) { gtk_window_destroy(GTK_WINDOW(d)); }), nullptr);
        gtk_window_present(GTK_WINDOW(dlg));
        return;
    }
    
    std::vector<std::pair<std::string, std::string>> filesToRestore;
    
    if (type == 0) { // Volume Parent
        for (size_t f = 0; f < pVol->itemPaths.size(); ++f) {
            if (f < pVol->itemGroupedPaths.size() && !pVol->itemGroupedPaths[f].empty()) {
                const auto& gps = pVol->itemGroupedPaths[f];
                const auto& ogs = pVol->itemOriginalGroupedPaths[f];
                for (size_t i = 0; i < gps.size(); ++i) {
                    std::string rel = gps[i];
                    std::string orig = (i < ogs.size()) ? ogs[i] : "";
                    if (!orig.empty()) {
                        filesToRestore.push_back({rel, orig});
                    }
                }
            } else {
                std::string rel = pVol->itemPaths[f];
                std::string orig = (f < pVol->itemOriginalPaths.size()) ? pVol->itemOriginalPaths[f] : "";
                if (!orig.empty()) {
                    filesToRestore.push_back({rel, orig});
                }
            }
        }
    } else if (type == 1) { // File child
        if (fileIdx >= 0 && fileIdx < (int)pVol->itemPaths.size()) {
            if (fileIdx < (int)pVol->itemGroupedPaths.size() && !pVol->itemGroupedPaths[fileIdx].empty()) {
                const auto& gps = pVol->itemGroupedPaths[fileIdx];
                const auto& ogs = pVol->itemOriginalGroupedPaths[fileIdx];
                for (size_t i = 0; i < gps.size(); ++i) {
                    std::string rel = gps[i];
                    std::string orig = (i < ogs.size()) ? ogs[i] : "";
                    if (!orig.empty()) {
                        filesToRestore.push_back({rel, orig});
                    }
                }
            } else {
                std::string rel = pVol->itemPaths[fileIdx];
                std::string orig = (fileIdx < (int)pVol->itemOriginalPaths.size()) ? pVol->itemOriginalPaths[fileIdx] : "";
                if (!orig.empty()) {
                    filesToRestore.push_back({rel, orig});
                }
            }
        }
    } else if (type == 2) { // Grandchild under group
        if (fileIdx >= 0 && fileIdx < (int)pVol->itemGroupedPaths.size() && gcIdx >= 0 && gcIdx < (int)pVol->itemGroupedPaths[fileIdx].size()) {
            std::string rel = pVol->itemGroupedPaths[fileIdx][gcIdx];
            std::string orig = (fileIdx < (int)pVol->itemOriginalGroupedPaths.size() && gcIdx < (int)pVol->itemOriginalGroupedPaths[fileIdx].size()) ?
                pVol->itemOriginalGroupedPaths[fileIdx][gcIdx] : "";
            if (!orig.empty()) {
                filesToRestore.push_back({rel, orig});
            }
        }
    }
    
    // Filter out files that already exist at their original paths
    std::vector<std::pair<std::string, std::string>> activeFiles;
    for (const auto& pair : filesToRestore) {
        std::filesystem::path targetPath = utf8Path(pair.second);
        std::error_code ec;
        if (std::filesystem::exists(targetPath, ec)) {
            continue;
        }
        activeFiles.push_back(pair);
    }
    
    if (activeFiles.empty()) {
        GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(window),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "%s", _T("msg_restore_all_exist", "All target files already exist. No files were restored.").c_str());
        g_signal_connect(dlg, "response", G_CALLBACK(+[](GtkWidget* d, int, gpointer) { gtk_window_destroy(GTK_WINDOW(d)); }), nullptr);
        gtk_window_present(GTK_WINDOW(dlg));
        return;
    }
    
    // Ask the user for the location of the respective volume folder
    std::string title = _T("select_volume_loc_title", "Select Location for Volume_") + std::to_string(volIdx);
    GtkFileDialog* file_dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(file_dialog, title.c_str());
    
    struct RestoreContext {
        MainWindow* self;
        int volIdx;
        std::vector<std::pair<std::string, std::string>> activeFiles;
    };
    RestoreContext* ctx = new RestoreContext{this, volIdx, std::move(activeFiles)};
    
    gtk_file_dialog_select_folder(file_dialog, GTK_WINDOW(window), nullptr, +[](GObject* source, GAsyncResult* res, gpointer user_data) {
        auto* ctx = static_cast<RestoreContext*>(user_data);
        GError* error = nullptr;
        GFile* file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), res, &error);
        
        if (file) {
            char* path = g_file_get_path(file);
            std::string selectedFolder = path;
            g_free(path);
            g_object_unref(file);
            
            std::filesystem::path volRoot = utf8Path(selectedFolder);
            std::filesystem::path subDir = volRoot / ("Volume_" + std::to_string(ctx->volIdx));
            std::error_code ec;
            if (std::filesystem::is_directory(subDir, ec)) {
                volRoot = subDir;
            }
            
            int successCount = 0;
            int failCount = 0;
            std::string lastError = "";
            
            for (const auto& pair : ctx->activeFiles) {
                try {
                    std::filesystem::path src = volRoot / utf8Path(pair.first);
                    std::filesystem::path dest = utf8Path(pair.second);
                    
                    if (std::filesystem::exists(dest)) {
                        continue; // skip
                    }
                    
                    std::filesystem::create_directories(dest.parent_path());
                    if (std::filesystem::is_directory(src)) {
                        std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
                    } else {
                        std::filesystem::copy_file(src, dest, std::filesystem::copy_options::overwrite_existing);
                    }
                    successCount++;
                } catch (const std::exception& e) {
                    failCount++;
                    lastError = e.what();
                }
            }
            
            if (failCount == 0) {
                std::string successText = _T("msg_restore_success_1", "Successfully restored ") + std::to_string(successCount) + _T("msg_restore_success_2", " item(s).");
                GtkWidget* res_dlg = gtk_message_dialog_new(GTK_WINDOW(ctx->self->window),
                                                           GTK_DIALOG_MODAL,
                                                           GTK_MESSAGE_INFO,
                                                           GTK_BUTTONS_OK,
                                                           "%s", successText.c_str());
                g_signal_connect(res_dlg, "response", G_CALLBACK(+[](GtkWidget* rd, int, gpointer) { gtk_window_destroy(GTK_WINDOW(rd)); }), nullptr);
                gtk_window_present(GTK_WINDOW(res_dlg));
                ctx->self->append_log("Restored " + std::to_string(successCount) + " item(s) to original location.\n", 1);
            } else {
                std::string failText = _T("msg_restore_failed_1", "Restored ") + std::to_string(successCount) + _T("msg_restore_failed_2", " items, but ") + std::to_string(failCount) + _T("msg_restore_failed_3", " failed.\nLast error: ") + lastError;
                GtkWidget* res_dlg = gtk_message_dialog_new(GTK_WINDOW(ctx->self->window),
                                                           GTK_DIALOG_MODAL,
                                                           GTK_MESSAGE_ERROR,
                                                           GTK_BUTTONS_OK,
                                                           "%s", failText.c_str());
                g_signal_connect(res_dlg, "response", G_CALLBACK(+[](GtkWidget* rd, int, gpointer) { gtk_window_destroy(GTK_WINDOW(rd)); }), nullptr);
                gtk_window_present(GTK_WINDOW(res_dlg));
                ctx->self->append_log("Failed to restore items: " + lastError + "\n", 3);
            }
        }
        
        if (error) {
            g_error_free(error);
        }
        delete ctx;
    }, ctx);
}

void MainWindow::start_tutorial() {
    struct TutorialState {
        MainWindow* self;
        GtkWidget* tut_win;
        GtkWidget* title_label;
        GtkWidget* desc_label;
        GtkWidget* prev_btn;
        GtkWidget* next_btn;
        int step;
    };
    
    // Disable action buttons in main window
    gtk_widget_set_sensitive(start_button, FALSE);
    gtk_widget_set_sensitive(test_button, FALSE);
    gtk_widget_set_sensitive(stop_button, FALSE);
    
    // Create modeless window
    GtkWidget* tut_win = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(tut_win), _T("tut_title", "Interactive Tutorial").c_str());
    gtk_window_set_default_size(GTK_WINDOW(tut_win), 460, 200);
    gtk_window_set_transient_for(GTK_WINDOW(tut_win), GTK_WINDOW(window));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(tut_win), TRUE);
    
    // Layout
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(vbox, 16);
    gtk_widget_set_margin_end(vbox, 16);
    gtk_widget_set_margin_top(vbox, 16);
    gtk_widget_set_margin_bottom(vbox, 16);
    gtk_window_set_child(GTK_WINDOW(tut_win), vbox);
    
    GtkWidget* title_label = gtk_label_new(nullptr);
    gtk_widget_set_halign(title_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(vbox), title_label);
    
    GtkWidget* desc_label = gtk_label_new(nullptr);
    gtk_widget_set_halign(desc_label, GTK_ALIGN_START);
    gtk_label_set_wrap(GTK_LABEL(desc_label), TRUE);
    gtk_widget_set_vexpand(desc_label, TRUE);
    gtk_box_append(GTK_BOX(vbox), desc_label);
    
    GtkWidget* btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(vbox), btn_box);
    
    GtkWidget* prev_btn = gtk_button_new_with_label(_T("tut_btn_prev", "Back").c_str());
    gtk_box_append(GTK_BOX(btn_box), prev_btn);
    
    GtkWidget* next_btn = gtk_button_new_with_label(_T("tut_btn_next", "Next").c_str());
    gtk_box_append(GTK_BOX(btn_box), next_btn);
    
    GtkWidget* close_btn = gtk_button_new_with_label(_T("close_btn", "Close").c_str());
    gtk_box_append(GTK_BOX(btn_box), close_btn);
    
    auto* state = new TutorialState{this, tut_win, title_label, desc_label, prev_btn, next_btn, 1};
    
    auto update_step = [](TutorialState* s) {
        if (s->step < 1) s->step = 1;
        if (s->step > 7) s->step = 7;
        
        struct GtkTutorialStepData {
            std::string titleKey;
            std::string defaultTitle;
            std::string textKey;
            std::string defaultText;
        };
        
        GtkTutorialStepData steps[] = {
            { "tut_step1_title", "Welcome to BTTB", "tut_step1_text", "Welcome to the Burn to the Brim interactive tutorial! This guide will walk you through the key features without running any actions. Click Next to begin." },
            { "tut_step2_title", "Source Directories", "tut_step2_text", "Source Folder: Click '+' to manage multiple directories, or enter folder paths separated by semicolons. Files inside these directories will be organized." },
            { "tut_step3_title", "Target Directory", "tut_step3_text", "Target Folder: Specify the destination directory where BTTB will create packed volume directories (e.g., Volume_1, Volume_2)." },
            { "tut_step4_title", "Semantic Prompt", "tut_step4_text", "Semantic Prompt: Enter natural language packing rules (e.g. 'group audio files') to influence organization using neural embeddings." },
            { "tut_step5_title", "Packing Options", "tut_step5_text", "Options: Choose between Move (relocates files) or Symlink (non-destructive virtual links). You can also enable volume spanning and logs." },
            { "tut_step6_title", "TreeView Explorer", "tut_step6_text", "Results Explorer: The tree shows how items are assigned to volumes. Right-click any volume or file to open the context menu and restore it." },
            { "tut_step7_title", "Test and Start", "tut_step7_text", "Test & Start: Click 'Test' to run a safe packing simulation without touching files, or click 'Start' to perform the actual file placement." }
        };
        
        const auto& stepData = steps[s->step - 1];
        
        std::string titleMarkup = "<b>" + std::to_string(s->step) + "/7: " + _T(stepData.titleKey, stepData.defaultTitle) + "</b>";
        gtk_label_set_markup(GTK_LABEL(s->title_label), titleMarkup.c_str());
        gtk_label_set_text(GTK_LABEL(s->desc_label), _T(stepData.textKey, stepData.defaultText).c_str());
        
        gtk_widget_set_sensitive(s->prev_btn, s->step > 1);
        gtk_button_set_label(GTK_BUTTON(s->next_btn), s->step == 7 ? _T("tut_btn_finish", "Finish").c_str() : _T("tut_btn_next", "Next").c_str());
        
        if (s->step == 2) gtk_widget_grab_focus(s->self->source_entry);
        else if (s->step == 3) gtk_widget_grab_focus(s->self->target_entry);
        else if (s->step == 4) gtk_widget_grab_focus(s->self->semantic_entry);
        else if (s->step == 5) gtk_widget_grab_focus(s->self->move_check);
        else if (s->step == 6) gtk_widget_grab_focus(s->self->tree_view);
        else if (s->step == 7) gtk_widget_grab_focus(s->self->start_button);
    };
    
    // Auto-free state on window destroy, and re-enable main window buttons
    g_signal_connect(tut_win, "destroy", G_CALLBACK(+[](GtkWidget* w, gpointer data) {
        auto* state = static_cast<TutorialState*>(data);
        gtk_widget_set_sensitive(state->self->start_button, TRUE);
        gtk_widget_set_sensitive(state->self->test_button, TRUE);
        gtk_widget_set_sensitive(state->self->stop_button, TRUE);
        delete state;
    }), state);
    
    struct ClickData {
        TutorialState* s;
        void (*update_step_fn)(TutorialState*);
    };
    auto* cd = new ClickData{state, update_step};
    
    g_object_set_data_full(G_OBJECT(tut_win), "click_data", cd, [](gpointer data) {
        delete static_cast<ClickData*>(data);
    });
    
    g_signal_connect(prev_btn, "clicked", G_CALLBACK(+[](GtkButton*, gpointer data) {
        auto* cd = static_cast<ClickData*>(data);
        if (cd->s->step > 1) {
            cd->s->step--;
            cd->update_step_fn(cd->s);
        }
    }), cd);
    
    g_signal_connect(next_btn, "clicked", G_CALLBACK(+[](GtkButton*, gpointer data) {
        auto* cd = static_cast<ClickData*>(data);
        if (cd->s->step < 7) {
            cd->s->step++;
            cd->update_step_fn(cd->s);
        } else {
            gtk_window_destroy(GTK_WINDOW(cd->s->tut_win));
        }
    }), cd);
    
    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_window_destroy), tut_win);
    
    update_step(state);
    gtk_window_present(GTK_WINDOW(tut_win));
}

} // namespace bttb
