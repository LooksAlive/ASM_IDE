#include <stdio.h>

#include "assembly.h"

#include <gtk/gtk.h>



static void on_open_project(GtkWidget* widget, gpointer data);
static void on_new_project(GtkWidget* widget, gpointer data);

#include <gtk/gtk.h>

static Project p;


// Callback function for menu item "Edit"
static void on_menu_edit_activate(GtkMenuButton *menu_item, gpointer user_data) {
    GtkTreeModel *model = GTK_TREE_MODEL(user_data);
    // Implement edit action here
    g_print("Edit action activated\n");
}

// Callback function for menu item "Delete"
static void on_menu_delete_activate(GtkMenuButton *menu_item, gpointer user_data) {
    GtkTreeModel *model = GTK_TREE_MODEL(user_data);
    // Implement delete action here
    g_print("Delete action activated\n");
}

static void on_menu_item_selected(GMenuItem *item, gpointer user_data)
{
    GtkTreeIter *iter = (GtkTreeIter *)user_data;

    const char *item_label = "Edit";//g_menu_item_get_label(item);

    if (strcmp(item_label, "Edit") == 0) {
        // Handle Edit action
    } else if (strcmp(item_label, "Delete") == 0) {
        // Handle Delete action
    }
}

// Callback functions for menu items
void option1_activated(GtkButton *button, GtkTreeView *treeview) {
    g_print("Option 1 activated\n");
}

void option2_activated(GtkButton *button, GtkTreeView *treeview) {
    g_print("Option 2 activated\n");
}

GtkWidget* create_popover_menu(GtkTreeView *treeview) {
    GtkWidget *popover = gtk_popover_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *item1 = gtk_button_new_with_label("Option 1");
    GtkWidget *item2 = gtk_button_new_with_label("Option 2");

    gtk_box_append(GTK_BOX(box), item1);
    gtk_box_append(GTK_BOX(box), item2);

    g_signal_connect(item1, "clicked", G_CALLBACK(option1_activated), treeview);
    g_signal_connect(item2, "clicked", G_CALLBACK(option2_activated), treeview);

    gtk_popover_set_child(GTK_POPOVER(popover), box);
    gtk_widget_show(box);

    return popover;
}
/*
// Function to handle button press events
gboolean on_treeview_button_pressed_event(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {

    g_print("CLICKED\n");

    if (TRUE) { //gtk_gesture_single_get_button(gesture) == GDK_BUTTON_SECONDARY
        GtkWidget *popover = create_popover_menu(tree_view);

        //gtk_popover_set_relative_to(GTK_POPOVER(popover), widget);
        gtk_popover_popup(GTK_POPOVER(popover));
        return GDK_EVENT_STOP;  // Event has been handled
    }

    return GDK_EVENT_PROPAGATE;  // Event has not been handled
}
*/
/*
static void on_treeview_button_pressed_event(GtkGestureClick *gesture,
                                      int n_press,
                                      double x,
                                      double y,
                                      gpointer user_data) {
    if (gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture)) == GDK_BUTTON_SECONDARY) {
        GtkTreeView *tree_view = GTK_TREE_VIEW(user_data);
        GtkTreePath *path = NULL;
        GtkTreeModel *model = NULL;
        GtkTreeIter iter;

        gtk_tree_view_convert_widget_to_tree_coords(tree_view, x, y, &x, &y);
        if (gtk_tree_view_get_path_at_pos(tree_view, x, y, &path, NULL, NULL, NULL)) {
            model = gtk_tree_view_get_model(tree_view);
            if (gtk_tree_model_get_iter(model, &iter, path)) {
                GMenu *menu = g_menu_new();
                gtk_widget_set_parent(GTK_WIDGET(menu), GTK_WIDGET(tree_view));

                // Create menu items
                GMenuItem *menu_item = g_menu_item_new("Edit", "None description");
                g_menu_append_item(menu, menu_item);
                g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_edit_activate), model);

                menu_item = g_menu_item_new("Delete", "None description");
                g_menu_append_item(menu, menu_item);
                g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_delete_activate), model);

                gtk_widget_set_visible(GTK_WIDGET(menu), TRUE);

                // Popup the menu at the pointer position
                //gtk_menu_popup_at_pointer(GTK_MENU(menu), NULL);
                //g_signal_connect(menu, "item-selected", G_CALLBACK(on_menu_item_selected), &iter);
            }
            gtk_tree_path_free(path);
        }
    }
}
*/

static void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_tree_view_get_model(tree_view);

    // Get the iter (row) corresponding to the activated path
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        // Now you have the iter for the activated row, you can access model data as needed
        gchar *item_name;
        gtk_tree_model_get(model, &iter, 0, &item_name, -1); // Assuming column 0 contains item name

        // Example: Print the activated item name
        g_print("Activated item: %s\n", item_name);

        g_free(item_name);
    }
}






GtkWidget* create_right_panel() {
    GtkWidget *treeview;
    GtkTreeStore *treestore;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    GtkTreeIter iter, child;

    // Create a TreeStore model to store hierarchical data
    treestore = gtk_tree_store_new(1, G_TYPE_STRING); // One column for strings

    // Add functions group and items
    gtk_tree_store_append(treestore, &iter, NULL); // Append row to the top level
    gtk_tree_store_set(treestore, &iter, 0, "Functions", -1); // Set data for the row

    gtk_tree_store_append(treestore, &child, &iter); // Append child under "Functions"
    gtk_tree_store_set(treestore, &child, 0, "Function 1", -1); // Set data for the child row

    gtk_tree_store_append(treestore, &child, &iter); // Append another child under "Functions"
    gtk_tree_store_set(treestore, &child, 0, "Function 2", -1); // Set data for the child row

    // Add structures group and items
    gtk_tree_store_append(treestore, &iter, NULL); // Append another row to the top level
    gtk_tree_store_set(treestore, &iter, 0, "Structures", -1); // Set data for the row

    gtk_tree_store_append(treestore, &child, &iter); // Append child under "Structures"
    gtk_tree_store_set(treestore, &child, 0, "Structure 1", -1); // Set data for the child row

    gtk_tree_store_append(treestore, &child, &iter); // Append another child under "Structures"
    gtk_tree_store_set(treestore, &child, 0, "Structure 2", -1); // Set data for the child row

    // Add variables group and items
    gtk_tree_store_append(treestore, &iter, NULL); // Append another row to the top level
    gtk_tree_store_set(treestore, &iter, 0, "Variables", -1); // Set data for the row

    gtk_tree_store_append(treestore, &child, &iter); // Append child under "Variables"
    gtk_tree_store_set(treestore, &child, 0, "Variable 1", -1); // Set data for the child row

    gtk_tree_store_append(treestore, &child, &iter); // Append another child under "Variables"
    gtk_tree_store_set(treestore, &child, 0, "Variable 2", -1); // Set data for the child row

    // Create a TreeView widget
    treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(treestore));

    // Create a column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Items", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    g_signal_connect(treeview, "row-activated", G_CALLBACK(on_row_activated), NULL);
    //g_signal_connect(treeview, "row-activated", G_CALLBACK(on_treeview_button_pressed_event), NULL);

    return treeview;
}

GtkWidget* create_menu_bar(GtkApplication* app, GtkWidget* window) {
    GMenu *menubar = g_menu_new();
    GMenuItem *menu_item_file = g_menu_item_new("File", NULL);
    GMenu *file_menu = g_menu_new();
    GMenuItem *menu_item_open = g_menu_item_new("Open Project", "app.open");
    GMenuItem *menu_item_new = g_menu_item_new("New Project", "app.new");

    g_menu_append_item(file_menu, menu_item_open);
    g_menu_append_item(file_menu, menu_item_new);
    g_object_unref(menu_item_open);
    g_object_unref(menu_item_new);

    g_menu_item_set_submenu(menu_item_file, G_MENU_MODEL(file_menu));
    g_menu_append_item(menubar, menu_item_file);
    g_object_unref(menu_item_file);

    gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menubar));
    gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(window), TRUE);

    return NULL;
}

static GtkWidget* create_info_panel() {
    GtkWidget *info_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Project Info Label
    GtkWidget *info_label = gtk_label_new("Project Info:");
    gtk_box_append(GTK_BOX(info_panel), info_label);

    // File Size
    char file_size_str[50];
    snprintf(file_size_str, sizeof(file_size_str), "File Size: %llu", p.file_size);
    GtkWidget *file_size_label = gtk_label_new(file_size_str);
    gtk_box_append(GTK_BOX(info_panel), file_size_label);

    // Main Function
    char main_function_str[50];
    snprintf(main_function_str, sizeof(main_function_str), "Main Function: %llu", p.main_function);
    GtkWidget *main_function_label = gtk_label_new(main_function_str);
    gtk_box_append(GTK_BOX(info_panel), main_function_label);

    GtkTreeStore *files_store = gtk_tree_store_new(1, G_TYPE_STRING);
    GtkWidget *files_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(files_store));
    GtkTreeViewColumn *files_column = gtk_tree_view_column_new_with_attributes("Project files", gtk_cell_renderer_text_new(), "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(files_treeview), files_column);

    // Add Dynamic Data Files
    if (p.files_data_dynamic_size > 0) {
        GtkTreeIter data_dynamic_parent;
        gtk_tree_store_append(files_store, &data_dynamic_parent, NULL);
        gtk_tree_store_set(files_store, &data_dynamic_parent, 0, "Dynamic Data Files", -1);

        for (size_t i = 0; i < p.files_data_dynamic_size; i++) {
            GtkTreeIter iter;
            gtk_tree_store_append(files_store, &iter, &data_dynamic_parent);
            gtk_tree_store_set(files_store, &iter, 0, p.files_data_dynamic[i], -1);
        }
    }

    // Add BSS Constants Files
    if (p.files_bss_constants_size > 0) {
        GtkTreeIter bss_constants_parent;
        gtk_tree_store_append(files_store, &bss_constants_parent, NULL);
        gtk_tree_store_set(files_store, &bss_constants_parent, 0, "BSS Constants Files", -1);

        for (size_t i = 0; i < p.files_bss_constants_size; i++) {
            GtkTreeIter iter;
            gtk_tree_store_append(files_store, &iter, &bss_constants_parent);
            gtk_tree_store_set(files_store, &iter, 0, p.files_bss_constants[i], -1);
        }
    }

    // Add Functions Files
    if (p.files_functions_size > 0) {
        GtkTreeIter functions_parent;
        gtk_tree_store_append(files_store, &functions_parent, NULL);
        gtk_tree_store_set(files_store, &functions_parent, 0, "Functions Files", -1);

        for (size_t i = 0; i < p.files_functions_size; i++) {
            GtkTreeIter iter;
            gtk_tree_store_append(files_store, &iter, &functions_parent);
            gtk_tree_store_set(files_store, &iter, 0, p.files_functions[i], -1);
        }
    }

    gtk_box_append(GTK_BOX(info_panel), files_treeview);

    // Free Space File
    GtkWidget *free_space_label = gtk_label_new("Free Space File:");
    gtk_box_append(GTK_BOX(info_panel), free_space_label);
    GtkWidget *free_space_file_label = gtk_label_new(p.free_space_file);
    gtk_box_append(GTK_BOX(info_panel), free_space_file_label);

    // Project Directory
    GtkWidget *project_dir_label = gtk_label_new("Project Directory:");
    gtk_box_append(GTK_BOX(info_panel), project_dir_label);
    GtkWidget *project_directory_label = gtk_label_new(p.project_directory);
    gtk_box_append(GTK_BOX(info_panel), project_directory_label);

    return info_panel;
}

static GtkWidget* create_registers_panel() {
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_margin_start(panel, 10);
    gtk_widget_set_margin_end(panel, 10);
    gtk_widget_set_margin_top(panel, 10);
    gtk_widget_set_margin_bottom(panel, 10);

    // Create heading label
    GtkWidget *heading_label = gtk_label_new("Registers");
    gtk_widget_set_halign(heading_label, GTK_ALIGN_START);
    gtk_widget_set_valign(heading_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(heading_label, 5);
    gtk_box_append(GTK_BOX(panel), heading_label);

    // Create a vertical box for the register list
    GtkWidget *register_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(panel), register_list);

    // Add registers L0 to L10
    for (int i = 0; i <= 10; ++i) {
        gchar *register_name = g_strdup_printf("L%d", i);
        GtkWidget *register_label = gtk_label_new(register_name);
        g_free(register_name);
        gtk_widget_set_halign(register_label, GTK_ALIGN_START);
        gtk_widget_set_valign(register_label, GTK_ALIGN_CENTER);
        gtk_box_append(GTK_BOX(register_list), register_label);
    }

    return panel;
}

/*
static void on_row_selected(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
    GtkWidget *panel = GTK_WIDGET(user_data);

    // Clear any existing child widgets
    GList *children = gtk_container_get_children(GTK_CONTAINER(panel));
    g_list_free_full(children, gtk_widget_destroy);

    // Create instruction design panel and add to main panel
    GtkWidget *instruction_panel = create_instruction_design_panel();
    gtk_box_append(GTK_BOX(panel), instruction_panel);

    gtk_widget_show_all(panel);
}

// Function to handle key press events (shortcuts)
static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    static GtkTreeStore *store; // TODO.
    // Check for shortcut keys
    if (event->type == GDK_KEY_PRESS) {
        if (event->keyval == GDK_KEY_Insert) {
            // Handle Insert key (Create)
            GtkTreeIter iter;
            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter, 0, "New Address", 1, "New Opcode", 2, "New Operands", -1);
        } else if (event->keyval == GDK_KEY_Delete) {
            // Handle Delete key
            GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
            GtkTreeModel *model;
            GtkTreeIter iter;
            if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
                gtk_tree_store_remove(store, &iter);
            }
        } else if (event->keyval == GDK_KEY_s && (event->state & GDK_CONTROL_MASK)) {
            // Handle Ctrl+S key (Save)
            g_print("Ctrl+S pressed: Save\n");
        }
    }

    // Propagate the event further
    return FALSE;
}
*/

/*
GtkWidget* create_main_panel() {
    GtkWidget *main_panel = gtk_grid_new();
    gtk_widget_set_hexpand(main_panel, TRUE);
    gtk_widget_set_vexpand(main_panel, TRUE);

    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_grid_attach(GTK_GRID(main_panel), scrolled_window, 1, 0, 1, 1);

    GtkWidget *tree_view = gtk_tree_view_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), tree_view);

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Address", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("Opcode", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("Operands", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    GtkTreeStore *store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));

    GtkTreeIter iter;
    gtk_tree_store_append(store, &iter, NULL);
    gtk_tree_store_set(store, &iter, 0, "0x0000", 1, "MOV", 2, "RAX, RBX", -1);

    return main_panel;
}


static GtkWidget* create_instruction_design_panel() {
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Address
    GtkWidget *address_label = gtk_label_new("Address:");
    gtk_box_append(GTK_BOX(panel), address_label);

    GtkWidget *address_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(address_entry), "Enter address");
    gtk_box_append(GTK_BOX(panel), address_entry);

    // Opcode
    GtkWidget *opcode_label = gtk_label_new("Opcode:");
    gtk_box_append(GTK_BOX(panel), opcode_label);

    GtkWidget *opcode_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(opcode_combo), NULL, "MOV");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(opcode_combo), NULL, "ADD");
    gtk_box_append(GTK_BOX(panel), opcode_combo);

    // Operands
    GtkWidget *operands_label = gtk_label_new("Operands:");
    gtk_box_append(GTK_BOX(panel), operands_label);

    GtkWidget *operands_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(operands_entry), "Enter operands");
    gtk_box_append(GTK_BOX(panel), operands_entry);

    // Save and Delete buttons (optional)
    GtkWidget *save_button = gtk_button_new_with_label("Save");
    gtk_box_append(GTK_BOX(panel), save_button);

    GtkWidget *delete_button = gtk_button_new_with_label("Delete");
    gtk_box_append(GTK_BOX(panel), delete_button);

    return panel;
}
*/



/*
typedef struct {
    char *address;
    char *opcode;
    char *operands;
} InstructionRow;


// Function to add a new row to the list box
void add_row_to_listbox(GtkWidget *list_box, InstructionRow *row) {
    GtkWidget *row_widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_list_box_insert(GTK_LIST_BOX(list_box), row_widget, -1);

    // Address
    GtkWidget *address_label = gtk_label_new(row->address);
    gtk_box_append(GTK_BOX(row_widget), address_label);

    // Opcode (ComboBoxText)
    GtkWidget *opcode_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(opcode_combo), NULL, "MOV");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(opcode_combo), NULL, "ADD");
    gtk_combo_box_set_active_id(GTK_COMBO_BOX_TEXT(opcode_combo), row->opcode);
    gtk_box_append(GTK_BOX(row_widget), opcode_combo);

    // Operands (Entry)
    GtkWidget *operands_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(operands_entry), row->operands);
    gtk_box_append(GTK_BOX(row_widget), operands_entry);
}

// Callback function for adding a new row
void on_add_row_clicked(GtkButton *button, gpointer user_data) {
    static int row_count = 2;  // Start adding rows from row 2 (index 1)
    GtkWidget *list_box = GTK_WIDGET(user_data);

    InstructionRow new_row = {"0x0000", "MOV", "RAX, RBX"};
    add_row_to_listbox(list_box, &new_row);

    row_count++;
}

static GtkWidget* create_main_panel() {
    GtkWidget *main_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_hexpand(main_panel, TRUE);
    gtk_widget_set_vexpand(main_panel, TRUE);

    // Create scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_box_append(GTK_BOX(main_panel), scrolled_window);

    // Create a list box for the code display
    GtkWidget *list_box = gtk_list_box_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), list_box);

    // Add headers (optional)
    GtkWidget *header_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_list_box_insert(GTK_LIST_BOX(list_box), header_row, -1);

    GtkWidget *address_header = gtk_label_new("Address");
    gtk_box_append(GTK_BOX(header_row), address_header);

    GtkWidget *opcode_header = gtk_label_new("Opcode");
    gtk_box_append(GTK_BOX(header_row), opcode_header);

    GtkWidget *operands_header = gtk_label_new("Operands");
    gtk_box_append(GTK_BOX(header_row), operands_header);

    // Add an initial row (optional)
    InstructionRow initial_row = {"0x0000", "MOV", "RAX, RBX"};
    add_row_to_listbox(list_box, &initial_row);
    add_row_to_listbox(list_box, &initial_row);
    add_row_to_listbox(list_box, &initial_row);add_row_to_listbox(list_box, &initial_row);

    // Button to add new rows
    GtkWidget *add_button = gtk_button_new_with_label("Add Row");
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_row_clicked), list_box);
    gtk_box_append(GTK_BOX(main_panel), add_button);

    return main_panel;
}
*/


/*
static GtkWidget* create_main_panel() {
    GtkWidget *main_panel = gtk_grid_new();
    gtk_widget_set_hexpand(main_panel, TRUE);
    gtk_widget_set_vexpand(main_panel, TRUE);

    // Create scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_grid_attach(GTK_GRID(main_panel), scrolled_window, 1, 0, 1, 1);

    // Create tree view
    GtkWidget *tree_view = gtk_tree_view_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), tree_view);

    // Add columns to the tree view
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Column 1: Address
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Address", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Column 2: Opcode (ComboBox)
    GtkListStore *opcode_store = gtk_list_store_new(1, G_TYPE_STRING);
    GtkTreeIter iter;
    gtk_list_store_append(opcode_store, &iter);
    gtk_list_store_set(opcode_store, &iter, 0, "MOV", -1);  // Sample instruction
    gtk_list_store_append(opcode_store, &iter);
    gtk_list_store_set(opcode_store, &iter, 0, "ADD", -1);  // Another sample instruction

    renderer = gtk_cell_renderer_combo_new();
    g_object_set(renderer, "editable", TRUE, "model", opcode_store, "text-column", 0, NULL);
    column = gtk_tree_view_column_new_with_attributes("Opcode", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Column 3: Operands (Text Entry)
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Operands", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Create a tree store for the data model
    GtkTreeStore *store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));

    // Add an initial row (optional)
    GtkTreeIter tree_iter;
    gtk_tree_store_append(store, &tree_iter, NULL);
    gtk_tree_store_set(store, &tree_iter, 0, "0x0000", 1, "MOV", 2, "RAX, RBX", -1);

    // Allow adding new rows dynamically
    // You can implement a button or menu action to trigger this

    // Pack tree view into main panel
    gtk_grid_attach(GTK_GRID(main_panel), tree_view, 0, 0, 1, 1);

    return main_panel;
}
*/








typedef struct {
    GtkWidget *address_entry;
    GtkWidget *opcode_entry;
    GtkWidget *operands_box;
} InstructionRowWidgets;

// Function to add an operand entry to a row
void add_operand_entry(GtkWidget *operands_box) {
    GtkWidget *operand_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_append(GTK_BOX(operands_box), operand_container);

    GtkWidget *operand_entry = gtk_entry_new();
    gtk_box_append(GTK_BOX(operand_container), operand_entry);

    GtkWidget *remove_operand_button = gtk_button_new_with_label("-");
    g_signal_connect_swapped(remove_operand_button, "clicked", G_CALLBACK(gtk_widget_unparent), operand_container);
    gtk_box_append(GTK_BOX(operand_container), remove_operand_button);
}

// Function to add a new row to the list box
void add_row_to_listbox(GtkWidget *list_box) {
    GtkWidget *row_widget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list_box));

    if (selected_row) {
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row_widget, gtk_list_box_row_get_index(selected_row) + 1);
    } else {
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row_widget, -1);
    }

    //gtk_list_box_insert(GTK_LIST_BOX(list_box), row_widget, -1);

    // Address
    GtkWidget *address_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(address_entry), "0x0000");
    gtk_box_append(GTK_BOX(row_widget), address_entry);

    // Opcode (Entry)
    GtkWidget *opcode_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(opcode_entry), "MOV");
    gtk_box_append(GTK_BOX(row_widget), opcode_entry);

    // Operands (Box)
    GtkWidget *operands_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    add_operand_entry(operands_box); // Add initial operand entry
    gtk_box_append(GTK_BOX(row_widget), operands_box);

    // Add Operand Button
    GtkWidget *add_operand_button = gtk_button_new_with_label("+");
    g_signal_connect_swapped(add_operand_button, "clicked", G_CALLBACK(add_operand_entry), operands_box);
    gtk_box_append(GTK_BOX(row_widget), add_operand_button);
}

// Callback function for adding a new row
void on_add_row_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *list_box = GTK_WIDGET(user_data);
    add_row_to_listbox(list_box);
    gtk_widget_show(list_box);  // Show the newly added row
}

// Callback function for removing a selected row
void on_remove_row_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *list_box = GTK_WIDGET(user_data);
    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(GTK_LIST_BOX(list_box));
    if (selected_row) {
        gtk_list_box_remove(GTK_LIST_BOX(list_box), GTK_WIDGET(selected_row));
    }
}

// Function to create the main panel with the list box and buttons
static GtkWidget* create_main_panel() {
    GtkWidget *main_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_hexpand(main_panel, TRUE);
    gtk_widget_set_vexpand(main_panel, TRUE);

    // Create scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_box_append(GTK_BOX(main_panel), scrolled_window);

    // Create a list box for the code display
    GtkWidget *list_box = gtk_list_box_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), list_box);

    // Add headers
    GtkWidget *header_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_list_box_insert(GTK_LIST_BOX(list_box), header_row, -1);

    GtkWidget *address_header = gtk_label_new("Address");
    gtk_widget_set_margin_start(address_header, 60);
    gtk_widget_set_margin_end(address_header, 60);
    gtk_widget_set_margin_top(address_header, 5);
    gtk_widget_set_margin_bottom(address_header, 5);
    gtk_box_append(GTK_BOX(header_row), address_header);

    GtkWidget *opcode_header = gtk_label_new("Opcode");
    gtk_widget_set_margin_start(opcode_header, 60);
    gtk_widget_set_margin_end(opcode_header, 60);
    gtk_widget_set_margin_top(opcode_header, 5);
    gtk_widget_set_margin_bottom(opcode_header, 5);
    gtk_box_append(GTK_BOX(header_row), opcode_header);

    GtkWidget *operands_header = gtk_label_new("Operands");
    gtk_widget_set_margin_start(operands_header, 60);
    gtk_widget_set_margin_end(operands_header, 60);
    gtk_widget_set_margin_top(operands_header, 5);
    gtk_widget_set_margin_bottom(operands_header, 5);
    gtk_box_append(GTK_BOX(header_row), operands_header);

    // Add an initial row
    add_row_to_listbox(list_box);

    // Button to add new rows
    GtkWidget *add_button = gtk_button_new_with_label("Add Row");
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_row_clicked), list_box);
    gtk_box_append(GTK_BOX(main_panel), add_button);

    // Button to remove selected row
    GtkWidget *remove_button = gtk_button_new_with_label("Remove Selected Row");
    g_signal_connect(remove_button, "clicked", G_CALLBACK(on_remove_row_clicked), list_box);
    gtk_box_append(GTK_BOX(main_panel), remove_button);

    return main_panel;
}

// Callback function for handling keyboard shortcuts
gboolean on_key_press_event(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
    GtkWidget *list_box = GTK_WIDGET(user_data);

    if ((state & GDK_CONTROL_MASK) && keyval == GDK_KEY_n) {
        on_add_row_clicked(NULL, list_box);
        return TRUE;
    } else if ((state & GDK_CONTROL_MASK) && keyval == GDK_KEY_d) {
        on_remove_row_clicked(NULL, list_box);
        return TRUE;
    }
    return FALSE;
}



static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Assembly IDE");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    create_menu_bar(app, window);


    GtkWidget *info_panel = create_info_panel();
    gtk_grid_attach(GTK_GRID(grid), info_panel, 0, 1, 2, 1);

    GtkWidget *main_panel = create_main_panel();
    gtk_grid_attach(GTK_GRID(grid), main_panel, 1, 2, 1, 1);

    GtkWidget* register_panel = create_registers_panel();
    gtk_grid_attach(GTK_GRID(grid), register_panel, 2, 2, 1, 1);


    GtkEventController *controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-pressed", G_CALLBACK(on_key_press_event), main_panel);
    gtk_widget_add_controller(window, controller);

    //GtkWidget *idesign_panel = create_instruction_design_panel();
    //gtk_grid_attach(GTK_GRID(grid), idesign_panel, 1, 3, 1, 1);

    GtkWidget *right_panel = create_right_panel();
    gtk_grid_attach(GTK_GRID(grid), right_panel, 0, 2, 1, 1);

    GtkWidget* bf = gtk_button_new_with_label("New Function");
    GtkWidget* sf = gtk_button_new_with_label("New Structure");
    GtkWidget* vf = gtk_button_new_with_label("New Variable");
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(box), bf);
    gtk_box_append(GTK_BOX(box), sf);
    gtk_box_append(GTK_BOX(box), vf);

    gtk_grid_attach(GTK_GRID(grid), box, 0, 3, 1, 1);


    gtk_widget_show(window);
}

int main(int argc, char** argv)
{
    printf("Hello World!\n");

    //Project p;
    //init_project(&p, "test");
    //clean_project(&p);

    deserialize_project(&p, "test/project.bin");

    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    clean_project(&p);

    return status;
}
