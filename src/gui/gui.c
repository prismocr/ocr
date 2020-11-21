#include <gtk/gtk.h>
#include "utils/matrix.h"
#include "utils/error.h"
#include "utils/bitmap.h"
#include "imgproc/rotation.h"

GtkWidget *input;
GtkWidget *picture_btn;
GtkWidget *files_box;
GtkWidget *result_template;

int start_gui(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;

    gtk_init(&argc, &argv);
    builder = gtk_builder_new();

    // Load layout
    gtk_builder_add_from_file(builder, "./res/layout/main.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_CENTER);
    // Connect signals (events)
    gtk_builder_connect_signals(builder, NULL);

    // Set title
    gtk_window_set_title(GTK_WINDOW(window), "Prism");

    // Load icon
    GdkPixbuf *icon = gdk_pixbuf_new_from_file_at_size(
      "./res/icons/prism_icon.png", 128, 128, NULL);
    gtk_window_set_icon(GTK_WINDOW(window), icon);

    // Retrieve widget pointers
    input = GTK_WIDGET(gtk_builder_get_object(builder, "file_input"));
    picture_btn = GTK_WIDGET(gtk_builder_get_object(builder, "picture_btn"));
    files_box = GTK_WIDGET(gtk_builder_get_object(builder, "files_box"));
    result_template = GTK_WIDGET(gtk_builder_get_object(builder, "Result"));

    // Unref builder
    g_object_unref(builder);

    // Load CSS
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "./res/styles/styles.css",
                                    NULL);

    // Inject CSS
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
                                              GTK_STYLE_PROVIDER(cssProvider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Run GUI
    gtk_widget_show(window);
    gtk_main();

    return 0;
}

void add_file_entry(const gchar *path) {
    GtkWidget *result = gtk_box_new(0, 0);
    GtkStyleContext *result_ctx = gtk_widget_get_style_context(result);
    GtkWidget *label = gtk_label_new(path);
    GtkWidget *button = gtk_button_new_with_label("View results");
    GtkStyleContext *button_ctx = gtk_widget_get_style_context(button);

    gtk_widget_set_size_request(result, 550, -1);
    gtk_widget_set_halign(result, GTK_ALIGN_FILL);
    gtk_widget_set_valign(result, GTK_ALIGN_FILL);
    gtk_style_context_add_class(result_ctx, "result");

    gtk_style_context_add_class(button_ctx, "results");
    gtk_widget_set_halign(button, GTK_ALIGN_END);
    gtk_widget_set_valign(button, GTK_ALIGN_END);

    g_object_set(button, "expand", TRUE, NULL);

    gtk_container_add(GTK_CONTAINER(result), label);
    gtk_container_add(GTK_CONTAINER(result), button);
    gtk_container_add(GTK_CONTAINER(files_box), result);

    gtk_widget_show(label);
    gtk_widget_show(button);
    gtk_widget_show(result);
}

void on_file(GtkFileChooser *filechooserbutton1) {
    gchar *str = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(filechooserbutton1));
    // Remove "file://"
    char *path = str + 7;
    add_file_entry(path);

    // TODO: process image
}

// called when window is closed
void on_window_main_destroy() {
    gtk_main_quit();
}