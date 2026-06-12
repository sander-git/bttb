#include "ui_settings.hpp"
#include "bttb_locale.hpp"
#include <string>
#include <vector>

namespace bttb {

struct DialogData {
    BttbSolver* solver;
    GtkWidget* media_combo;
    GtkWidget* capacity_entry;
    GtkWidget* cluster_entry;
    GtkWidget* slack_entry;
    GtkWidget* search_time_entry;
    GtkWidget* split_depth_entry;
    GtkWidget* skip_empty_switch;
    GtkWidget* capacity_mb_label;
    GtkWidget* cv_name_entry;
    GtkWidget* conflict_switch;
    GtkWidget* dark_theme_switch;
    GtkWidget* enable_par3_switch;
    GtkWidget* par3_block_size_entry;
    GtkWidget* par3_redundancy_entry;
    GtkWidget* language_combo;

    
    GtkListStore* rule_store;
    GtkWidget* rule_tree_view;
    GtkWidget* pattern_entry;
    GtkWidget* match_files_check;
    GtkWidget* match_folders_check;
    GtkWidget* regex_check;
};

static void on_media_changed(GtkComboBox* combo, gpointer user_data) {
    auto* data = static_cast<DialogData*>(user_data);
    int active = gtk_combo_box_get_active(combo);
    
    if (active == 0) { // CD 650MB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "650 MB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "2048");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 1) { // CD 700MB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "700 MB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "2048");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 2) { // DVD 4.7GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "4.7 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "2048");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 3) { // DVD DL 8.5GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "8.5 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "2048");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 4) { // BD 25GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "25 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "2048");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 5) { // BD DL 50GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "50 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "2048");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 6) { // USB 8GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "8 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "4096");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 7) { // USB 16GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "16 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "4096");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 8) { // USB 32GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "32 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "4096");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 9) { // USB 64GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "64 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "4096");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 10) { // USB 256GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "256 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "4096");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 11) { // USB 512GB
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "512 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "4096");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 12) { // Auto Size
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "Auto");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "2048");
        gtk_widget_set_sensitive(data->capacity_entry, FALSE);
    } else if (active == 13) { // Custom Size
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "64 GB");
        gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), "4096");
        gtk_widget_set_sensitive(data->capacity_entry, TRUE);
    } else if (active >= 14) { // Custom Volumes
        int cvIdx = active - 14;
        if (cvIdx >= 0 && cvIdx < (int)data->solver->customVolumes.size()) {
            const auto& cv = data->solver->customVolumes[cvIdx];
            std::string sizeStr = formatHumanSize(cv.capacityBytes);
            gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), sizeStr.c_str());
            gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), std::to_string(cv.sectorSize).c_str());
            gtk_editable_set_text(GTK_EDITABLE(data->slack_entry), std::to_string(cv.slackBytes).c_str());
            gtk_widget_set_sensitive(data->capacity_entry, FALSE);
        }
    }
}

static void on_add_rule(GtkButton* button, gpointer user_data) {
    auto* data = static_cast<DialogData*>(user_data);
    const char* pattern = gtk_editable_get_text(GTK_EDITABLE(data->pattern_entry));
    if (!pattern || strlen(pattern) == 0) return;
    
    gboolean m_files = gtk_check_button_get_active(GTK_CHECK_BUTTON(data->match_files_check));
    gboolean m_folders = gtk_check_button_get_active(GTK_CHECK_BUTTON(data->match_folders_check));
    gboolean is_regex = gtk_check_button_get_active(GTK_CHECK_BUTTON(data->regex_check));
    
    GtkTreeIter iter;
    gtk_list_store_append(data->rule_store, &iter);
    gtk_list_store_set(data->rule_store, &iter,
                       0, pattern,
                       1, m_files ? "Yes" : "No",
                       2, m_folders ? "Yes" : "No",
                       3, is_regex ? "Regex" : "Glob",
                       -1);
    
    gtk_editable_set_text(GTK_EDITABLE(data->pattern_entry), "");
}

static void on_remove_rule(GtkButton* button, gpointer user_data) {
    auto* data = static_cast<DialogData*>(user_data);
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->rule_tree_view));
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void SettingsDialog::run(GtkWindow* parent, BttbSolver& solver) {
    // Create transient modal window
    GtkWidget* dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), _T("pref_title", "Preferences").c_str());
    gtk_window_set_default_size(GTK_WINDOW(dialog), 600, 800);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    
    DialogData* data = new DialogData();
    data->solver = &solver;
    
    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(main_box, 16);
    gtk_widget_set_margin_end(main_box, 16);
    gtk_widget_set_margin_top(main_box, 16);
    gtk_widget_set_margin_bottom(main_box, 16);
    
    gtk_window_set_child(GTK_WINDOW(dialog), main_box);
    
    // Scrolled window for settings content (so buttons remain anchored at bottom)
    GtkWidget* scrolled_win = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled_win, TRUE);
    gtk_box_append(GTK_BOX(main_box), scrolled_win);
    
    GtkWidget* content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_win), content_box);
    
    // Grid for standard inputs
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_box_append(GTK_BOX(content_box), grid);
    
    // Media size
    GtkWidget* label_media = gtk_label_new(_T("media_size_label", "Media Size:").c_str());
    gtk_widget_set_halign(label_media, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_media, 0, 0, 1, 1);
    
    data->media_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "CD (650 MB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "CD (700 MB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "DVD (4.7 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "DVD DL (8.5 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "BD (25 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "BD DL (50 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "USB (8 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "USB (16 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "USB (32 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "USB (64 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "USB (256 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), "USB (512 GB)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), _T("auto_size", "Auto Size").c_str());
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), _T("custom_size", "Custom Size").c_str());
    for (const auto& cv : solver.customVolumes) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), cv.name.c_str());
    }
    gtk_grid_attach(GTK_GRID(grid), data->media_combo, 1, 0, 1, 1);
    
    // Capacity
    GtkWidget* label_cap = gtk_label_new(_T("capacity_label", "Capacity:").c_str());
    gtk_widget_set_halign(label_cap, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_cap, 0, 1, 1, 1);
    
    data->capacity_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), data->capacity_entry, 1, 1, 1, 1);
    
    // Save Custom Volume name and button
    GtkWidget* save_cv_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_grid_attach(GTK_GRID(grid), save_cv_box, 1, 2, 1, 1);
    
    data->cv_name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->cv_name_entry), _T("custom_vol_name_placeholder", "Custom Volume Name").c_str());
    gtk_box_append(GTK_BOX(save_cv_box), data->cv_name_entry);
    
    GtkWidget* cv_save_btn = gtk_button_new_with_label(_T("save_btn", "Save").c_str());
    gtk_widget_set_tooltip_text(cv_save_btn, _T("save_cv_tooltip", "Save current capacity, sector, and slack as a named custom volume").c_str());
    gtk_box_append(GTK_BOX(save_cv_box), cv_save_btn);
    
    g_signal_connect(cv_save_btn, "clicked", G_CALLBACK(+[](GtkButton* btn, gpointer user_data) {
        auto* data = static_cast<DialogData*>(user_data);
        const char* name = gtk_editable_get_text(GTK_EDITABLE(data->cv_name_entry));
        if (!name || strlen(name) == 0) return;
        
        CustomVolume cv;
        cv.name = name;
        cv.capacityBytes = parseHumanSize(gtk_editable_get_text(GTK_EDITABLE(data->capacity_entry)));
        cv.sectorSize = std::stoll(gtk_editable_get_text(GTK_EDITABLE(data->cluster_entry)));
        cv.slackBytes = std::stoll(gtk_editable_get_text(GTK_EDITABLE(data->slack_entry)));
        
        data->solver->customVolumes.push_back(cv);
        data->solver->saveSettings();
        
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->media_combo), cv.name.c_str());
        
        int newIdx = 14 + data->solver->customVolumes.size() - 1;
        gtk_combo_box_set_active(GTK_COMBO_BOX(data->media_combo), newIdx);
        gtk_editable_set_text(GTK_EDITABLE(data->cv_name_entry), "");
    }), data);
    
    // Capacity MB Dynamic Label
    data->capacity_mb_label = gtk_label_new("(0.00 MB)");
    gtk_widget_set_halign(data->capacity_mb_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), data->capacity_mb_label, 1, 3, 1, 1);
    
    // Cluster
    GtkWidget* label_clus = gtk_label_new(_T("cluster_label", "Cluster Size (Bytes):").c_str());
    gtk_widget_set_halign(label_clus, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_clus, 0, 4, 1, 1);
    
    data->cluster_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), data->cluster_entry, 1, 4, 1, 1);
    
    // Slack
    GtkWidget* label_slack = gtk_label_new(_T("slack_label", "Slack Bytes:").c_str());
    gtk_widget_set_halign(label_slack, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_slack, 0, 5, 1, 1);
    
    data->slack_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), data->slack_entry, 1, 5, 1, 1);
    
    // Search Time
    GtkWidget* label_time = gtk_label_new(_T("search_time_label", "Max Search Time (sec):").c_str());
    gtk_widget_set_halign(label_time, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_time, 0, 6, 1, 1);
    
    data->search_time_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), data->search_time_entry, 1, 6, 1, 1);
    
    // Split Depth
    GtkWidget* label_depth = gtk_label_new(_T("split_depth_label", "Directory Split Depth:").c_str());
    gtk_widget_set_halign(label_depth, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_depth, 0, 7, 1, 1);
    
    data->split_depth_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), data->split_depth_entry, 1, 7, 1, 1);
    
    // Skip empty switch
    GtkWidget* label_empty = gtk_label_new(_T("skip_empty_label", "Skip Empty Files/Dirs:").c_str());
    gtk_widget_set_halign(label_empty, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_empty, 0, 8, 1, 1);
    
    data->skip_empty_switch = gtk_switch_new();
    gtk_widget_set_halign(data->skip_empty_switch, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), data->skip_empty_switch, 1, 8, 1, 1);
 
    // Rule conflict override
    GtkWidget* label_conflict = gtk_label_new(_T("rule_wins_chk", "Rule-based grouping wins:").c_str());
    gtk_widget_set_halign(label_conflict, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_conflict, 0, 9, 1, 1);
 
    data->conflict_switch = gtk_switch_new();
    gtk_widget_set_halign(data->conflict_switch, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), data->conflict_switch, 1, 9, 1, 1);
    
    // Enable Dark Theme switch
    GtkWidget* label_dark = gtk_label_new(_T("dark_theme_chk", "Enable Dark Theme:").c_str());
    gtk_widget_set_halign(label_dark, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_dark, 0, 10, 1, 1);
 
    data->dark_theme_switch = gtk_switch_new();
    gtk_widget_set_halign(data->dark_theme_switch, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), data->dark_theme_switch, 1, 10, 1, 1);
    
    // Enable PAR3 Parity protection switch
    GtkWidget* label_par3 = gtk_label_new(_T("enable_par3_chk", "Enable PAR3 Parity Protection:").c_str());
    gtk_widget_set_halign(label_par3, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_par3, 0, 11, 1, 1);
 
    data->enable_par3_switch = gtk_switch_new();
    gtk_widget_set_halign(data->enable_par3_switch, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), data->enable_par3_switch, 1, 11, 1, 1);
 
    // PAR3 Block Size (Bytes) entry
    GtkWidget* label_par3_block = gtk_label_new(_T("par3_block_size", "PAR3 Block Size (Bytes):").c_str());
    gtk_widget_set_halign(label_par3_block, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_par3_block, 0, 12, 1, 1);
 
    data->par3_block_size_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), data->par3_block_size_entry, 1, 12, 1, 1);
 
    // PAR3 Redundancy Percent entry
    GtkWidget* label_par3_red = gtk_label_new(_T("par3_redundancy", "PAR3 Redundancy Percent (%):").c_str());
    gtk_widget_set_halign(label_par3_red, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_par3_red, 0, 13, 1, 1);
 
    data->par3_redundancy_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), data->par3_redundancy_entry, 1, 13, 1, 1);
    
    // Language selection entry (Row 14)
    GtkWidget* label_lang = gtk_label_new(_T("language_label", "Language:").c_str());
    gtk_widget_set_halign(label_lang, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_lang, 0, 14, 1, 1);
    
    data->language_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "System Default (Auto)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "English");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Deutsch");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Nederlands");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Français");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Español");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "简体中文");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "日本語");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Italiano");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Ελληνικά");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Latina");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Português");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "हिन्दी");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Vuhlkansu (Vulkan)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->language_combo), "Eldarin (Elvish)");
    
    if (solver.language == "auto") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 0);
    else if (solver.language == "en") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 1);
    else if (solver.language == "de") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 2);
    else if (solver.language == "nl") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 3);
    else if (solver.language == "fr") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 4);
    else if (solver.language == "es") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 5);
    else if (solver.language == "zh") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 6);
    else if (solver.language == "ja") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 7);
    else if (solver.language == "it") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 8);
    else if (solver.language == "el") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 9);
    else if (solver.language == "la") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 10);
    else if (solver.language == "pt") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 11);
    else if (solver.language == "hi") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 12);
    else if (solver.language == "vul") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 13);
    else if (solver.language == "elv") gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 14);
    else gtk_combo_box_set_active(GTK_COMBO_BOX(data->language_combo), 0);
    
    gtk_grid_attach(GTK_GRID(grid), data->language_combo, 1, 14, 1, 1);
    
    // Grouping Rules list frame
    GtkWidget* rules_frame = gtk_frame_new("File/Folder Grouping Rules");
    gtk_box_append(GTK_BOX(content_box), rules_frame);
    
    GtkWidget* rules_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(rules_box, 8);
    gtk_widget_set_margin_end(rules_box, 8);
    gtk_widget_set_margin_top(rules_box, 8);
    gtk_widget_set_margin_bottom(rules_box, 8);
    gtk_frame_set_child(GTK_FRAME(rules_frame), rules_box);
    
    // TreeView for rules
    GtkWidget* scroll_win = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll_win, -1, 120);
    gtk_box_append(GTK_BOX(rules_box), scroll_win);
    
    data->rule_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    data->rule_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(data->rule_store));
    g_object_unref(data->rule_store);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll_win), data->rule_tree_view);
    
    // Columns
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(data->rule_tree_view),
                                gtk_tree_view_column_new_with_attributes("Pattern", renderer, "text", 0, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(data->rule_tree_view),
                                gtk_tree_view_column_new_with_attributes("Files", renderer, "text", 1, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(data->rule_tree_view),
                                gtk_tree_view_column_new_with_attributes("Folders", renderer, "text", 2, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(data->rule_tree_view),
                                gtk_tree_view_column_new_with_attributes("Type", renderer, "text", 3, NULL));
    
    // Inputs to add new rule
    GtkWidget* add_rule_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(add_rule_grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(add_rule_grid), 6);
    gtk_box_append(GTK_BOX(rules_box), add_rule_grid);
    
    data->pattern_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(data->pattern_entry), "Rule Pattern (*.mp3 or ^[0-9].*\\..*$)");
    gtk_grid_attach(GTK_GRID(add_rule_grid), data->pattern_entry, 0, 0, 3, 1);
    
    data->match_files_check = gtk_check_button_new_with_label("Match Files");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(data->match_files_check), TRUE);
    gtk_grid_attach(GTK_GRID(add_rule_grid), data->match_files_check, 0, 1, 1, 1);
    
    data->match_folders_check = gtk_check_button_new_with_label("Match Folders");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(data->match_folders_check), TRUE);
    gtk_grid_attach(GTK_GRID(add_rule_grid), data->match_folders_check, 1, 1, 1, 1);
    
    data->regex_check = gtk_check_button_new_with_label("Regex Pattern");
    gtk_grid_attach(GTK_GRID(add_rule_grid), data->regex_check, 2, 1, 1, 1);
    
    GtkWidget* rule_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(rules_box), rule_button_box);
    
    GtkWidget* add_rule_btn = gtk_button_new_with_label("Add Rule");
    g_signal_connect(add_rule_btn, "clicked", G_CALLBACK(on_add_rule), data);
    gtk_box_append(GTK_BOX(rule_button_box), add_rule_btn);
    
    GtkWidget* remove_rule_btn = gtk_button_new_with_label("Remove Selected");
    g_signal_connect(remove_rule_btn, "clicked", G_CALLBACK(on_remove_rule), data);
    gtk_box_append(GTK_BOX(rule_button_box), remove_rule_btn);
    
    // Bottom Action Buttons (OK / Cancel)
    GtkWidget* action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_halign(action_box, GTK_ALIGN_END);
    gtk_box_append(GTK_BOX(main_box), action_box);
    
    GtkWidget* cancel_btn = gtk_button_new_with_label("Cancel");
    gtk_box_append(GTK_BOX(action_box), cancel_btn);
    
    GtkWidget* ok_btn = gtk_button_new_with_label("OK");
    gtk_box_append(GTK_BOX(action_box), ok_btn);
    
    // Set initial human capacity value
    if (solver.mediumInfo.capacityBytes == 681574400) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "650 MB");
    else if (solver.mediumInfo.capacityBytes == 734003200) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "700 MB");
    else if (solver.mediumInfo.capacityBytes == 4700000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "4.7 GB");
    else if (solver.mediumInfo.capacityBytes == 8500000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "8.5 GB");
    else if (solver.mediumInfo.capacityBytes == 25000000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "25 GB");
    else if (solver.mediumInfo.capacityBytes == 50000000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "50 GB");
    else if (solver.mediumInfo.capacityBytes == 8000000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "8 GB");
    else if (solver.mediumInfo.capacityBytes == 16000000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "16 GB");
    else if (solver.mediumInfo.capacityBytes == 32000000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "32 GB");
    else if (solver.mediumInfo.capacityBytes == 64000000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "64 GB");
    else if (solver.mediumInfo.capacityBytes == 256000000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "256 GB");
    else if (solver.mediumInfo.capacityBytes == 512000000000LL) gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), "512 GB");
    else {
        std::string sizeStr = formatHumanSize(solver.mediumInfo.capacityBytes);
        gtk_editable_set_text(GTK_EDITABLE(data->capacity_entry), sizeStr.c_str());
    }
    
    gtk_editable_set_text(GTK_EDITABLE(data->cluster_entry), std::to_string(solver.mediumInfo.sectorSize).c_str());
    gtk_editable_set_text(GTK_EDITABLE(data->slack_entry), std::to_string(solver.mediumInfo.slackBytes).c_str());
    gtk_editable_set_text(GTK_EDITABLE(data->search_time_entry), std::to_string(solver.maxSearchTimeSeconds).c_str());
    gtk_editable_set_text(GTK_EDITABLE(data->split_depth_entry), std::to_string(solver.splitDepth).c_str());
    gtk_switch_set_active(GTK_SWITCH(data->skip_empty_switch), solver.skipEmpty);
    gtk_switch_set_active(GTK_SWITCH(data->conflict_switch), solver.ruleBasedWins);
    gtk_switch_set_active(GTK_SWITCH(data->dark_theme_switch), solver.enableDarkTheme);
    gtk_switch_set_active(GTK_SWITCH(data->enable_par3_switch), solver.enablePar3);
    gtk_editable_set_text(GTK_EDITABLE(data->par3_block_size_entry), std::to_string(solver.par3BlockSize).c_str());
    gtk_editable_set_text(GTK_EDITABLE(data->par3_redundancy_entry), std::to_string(solver.par3RedundancyPercent).c_str());
    
    // Listen for capacity edit changes to show dynamic parsed size in MB
    g_signal_connect(data->capacity_entry, "changed", G_CALLBACK(+[](GtkEditable* editable, gpointer user_data) {
        auto* data = static_cast<DialogData*>(user_data);
        const char* text = gtk_editable_get_text(editable);
        if (text && strcmp(text, "Auto") == 0) {
            gtk_label_set_text(GTK_LABEL(data->capacity_mb_label), "(Auto-Sized)");
            return;
        }
        int64_t bytes = parseHumanSize(text ? text : "");
        double mb = (double)bytes / (1024.0 * 1024.0);
        char buf[128];
        snprintf(buf, sizeof(buf), "(%.2f MB)", mb);
        gtk_label_set_text(GTK_LABEL(data->capacity_mb_label), buf);
    }), data);
    
    // Fill rules store from solver
    for (const auto& rule : solver.groupingRules) {
        GtkTreeIter iter;
        gtk_list_store_append(data->rule_store, &iter);
        gtk_list_store_set(data->rule_store, &iter,
                           0, rule.patternString.c_str(),
                           1, rule.matchFiles ? "Yes" : "No",
                           2, rule.matchFolders ? "Yes" : "No",
                           3, rule.isRegex ? "Regex" : "Glob",
                           -1);
    }
    
    // Select custom/standard size default index
    int combo_index = solver.lastSelectedVolumeIndex;
    int num_items = 14 + (int)solver.customVolumes.size();
    if (combo_index < 0 || combo_index >= num_items) {
        combo_index = 2; // Default to DVD (4.7 GB)
    }
    
    gtk_combo_box_set_active(GTK_COMBO_BOX(data->media_combo), combo_index);
    on_media_changed(GTK_COMBO_BOX(data->media_combo), data);
    
    g_signal_connect(data->media_combo, "changed", G_CALLBACK(on_media_changed), data);
    
    // Connect OK / Cancel signals
    g_signal_connect_swapped(cancel_btn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    
    // Connect OK button to save and exit
    g_signal_connect(ok_btn, "clicked", G_CALLBACK(+[](GtkButton* btn, gpointer user_data) {
        auto* data = static_cast<DialogData*>(user_data);
        
        // Save values using parseHumanSize
        const char* cap_txt = gtk_editable_get_text(GTK_EDITABLE(data->capacity_entry));
        if (cap_txt && strcmp(cap_txt, "Auto") == 0) {
            data->solver->mediumInfo.capacityBytes = 0;
        } else {
            data->solver->mediumInfo.capacityBytes = parseHumanSize(cap_txt);
        }
        data->solver->mediumInfo.sectorSize = std::stoll(gtk_editable_get_text(GTK_EDITABLE(data->cluster_entry)));
        data->solver->mediumInfo.slackBytes = std::stoll(gtk_editable_get_text(GTK_EDITABLE(data->slack_entry)));
        data->solver->maxSearchTimeSeconds = std::stoi(gtk_editable_get_text(GTK_EDITABLE(data->search_time_entry)));
        data->solver->splitDepth = std::stoi(gtk_editable_get_text(GTK_EDITABLE(data->split_depth_entry)));
        data->solver->skipEmpty = gtk_switch_get_active(GTK_SWITCH(data->skip_empty_switch));
        
        data->solver->lastSelectedVolumeIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(data->media_combo));
        data->solver->enableAutoVolume = (data->solver->lastSelectedVolumeIndex == 12);
        data->solver->ruleBasedWins = gtk_switch_get_active(GTK_SWITCH(data->conflict_switch));
        data->solver->enableDarkTheme = gtk_switch_get_active(GTK_SWITCH(data->dark_theme_switch));
        data->solver->enablePar3 = gtk_switch_get_active(GTK_SWITCH(data->enable_par3_switch));
        data->solver->par3BlockSize = std::stoll(gtk_editable_get_text(GTK_EDITABLE(data->par3_block_size_entry)));
        data->solver->par3RedundancyPercent = std::stoi(gtk_editable_get_text(GTK_EDITABLE(data->par3_redundancy_entry)));
        
        int lang_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(data->language_combo));
        std::string new_lang = "auto";
        if (lang_idx == 1) new_lang = "en";
        else if (lang_idx == 2) new_lang = "de";
        else if (lang_idx == 3) new_lang = "nl";
        else if (lang_idx == 4) new_lang = "fr";
        else if (lang_idx == 5) new_lang = "es";
        else if (lang_idx == 6) new_lang = "zh";
        else if (lang_idx == 7) new_lang = "ja";
        else if (lang_idx == 8) new_lang = "it";
        else if (lang_idx == 9) new_lang = "el";
        else if (lang_idx == 10) new_lang = "la";
        else if (lang_idx == 11) new_lang = "pt";
        else if (lang_idx == 12) new_lang = "hi";
        else if (lang_idx == 13) new_lang = "vul";
        else if (lang_idx == 14) new_lang = "elv";
        
        bool lang_changed = (new_lang != data->solver->language);
        if (lang_changed) {
            data->solver->language = new_lang;
            if (new_lang == "auto") {
                BttbLocale::getInstance().load(BttbLocale::getInstance().detectSystemLanguage());
            } else {
                BttbLocale::getInstance().load(new_lang);
            }
        }
        
        // Apply immediately
        GtkSettings *settings = gtk_settings_get_default();
        g_object_set(settings, "gtk-application-prefer-dark-theme", data->solver->enableDarkTheme ? TRUE : FALSE, NULL);
        
        // Persist settings to settings.txt
        data->solver->saveSettings();
        
        // Save rules
        data->solver->groupingRules.clear();
        GtkTreeIter iter;
        gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data->rule_store), &iter);
        while (valid) {
            char* pattern = nullptr;
            char* match_files = nullptr;
            char* match_folders = nullptr;
            char* type = nullptr;
            
            gtk_tree_model_get(GTK_TREE_MODEL(data->rule_store), &iter,
                               0, &pattern,
                               1, &match_files,
                               2, &match_folders,
                               3, &type,
                               -1);
            
            GroupRule rule;
            rule.patternString = pattern;
            rule.matchFiles = (strcmp(match_files, "Yes") == 0);
            rule.matchFolders = (strcmp(match_folders, "Yes") == 0);
            rule.isRegex = (strcmp(type, "Regex") == 0);
            
            if (rule.isRegex) {
                rule.compiledRegex = std::regex(rule.patternString, std::regex_constants::icase);
            } else {
                rule.compiledRegex = globToRegex(rule.patternString);
            }
            
            data->solver->groupingRules.push_back(rule);
            
            g_free(pattern);
            g_free(match_files);
            g_free(match_folders);
            g_free(type);
            
            valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(data->rule_store), &iter);
        }
        
        GtkWidget* t_window = gtk_widget_get_ancestor(GTK_WIDGET(btn), GTK_TYPE_WINDOW);
        if (lang_changed) {
            GtkWidget* msg_dialog = gtk_message_dialog_new(GTK_WINDOW(t_window),
                                                         GTK_DIALOG_MODAL,
                                                         GTK_MESSAGE_INFO,
                                                         GTK_BUTTONS_OK,
                                                         "%s", _T("restart_notice", "Language preferences saved. Please restart Burn to the Brim to apply the changes.").c_str());
            g_signal_connect(msg_dialog, "response", G_CALLBACK(+[](GtkWidget* dlg, int response, gpointer window_to_destroy) {
                gtk_window_destroy(GTK_WINDOW(dlg));
                if (window_to_destroy) {
                    gtk_window_destroy(GTK_WINDOW(window_to_destroy));
                }
            }), t_window);
            gtk_window_present(GTK_WINDOW(msg_dialog));
        } else {
            gtk_window_destroy(GTK_WINDOW(t_window));
        }
    }), data);
    
    // Free dialog data when window destroys
    g_signal_connect(dialog, "destroy", G_CALLBACK(+[](GtkWidget* widget, gpointer user_data) {
        auto* data = static_cast<DialogData*>(user_data);
        delete data;
    }), data);
    
    gtk_widget_set_visible(dialog, TRUE);
}

} // namespace bttb
