#include <gtk/gtk.h>
#include "utils/matrix.h"
#include "utils/error.h"
#include "utils/bitmap.h"
#include "imgproc/rotation.h"
#include "recognition/recognition.h"
#include "neuralnet/network.h"
#include "utils/file.h"

#define UNUSED(x) (void) (x)

GtkWidget *window;

GtkWidget *input;
GtkWidget *picture_btn;
GtkWidget *files_box;
GtkWidget *result_template;
GtkWidget *process_method;

Network net;

int start_gui(int argc, char *argv[]) {
    GtkBuilder *builder;

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
    process_method = GTK_WIDGET(gtk_builder_get_object(builder, "method"));

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

    // Load network
    network_load("./res/networks/main.hex", &net);

    // Run GUI
    gtk_widget_show(window);
    gtk_main();

    while (gtk_events_pending())
        gtk_main_iteration();

    return 0;
}

void copy_data(GtkButton *button, gpointer data) {
    UNUSED(button);
    char *content = (char *) data;
    GtkClipboard *clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clip, content, strlen(content));
    printf("content to copy: %s\n", content);
}

void process_file(GtkButton *button, gpointer data) {
    UNUSED(button);
    char *path = (char *) data;

    ocr(&net, path);

    gint method = gtk_combo_box_get_active(GTK_COMBO_BOX(process_method));
    char *content;

    if (method == 0) {
        content = file_read("./post.txt");
    } else if (method == 1) {
        content = file_read("./outmc.txt");
    } else if (method == 2) {
        content = file_read("./corrector.txt");
    }

    if (!content)
        return;

    printf("content : \n%s\n", content);

    const char format[]
      = "\n<span size=\"large\">Here are the results:</span>\n\n%s";
    GtkWidget *dialog = gtk_message_dialog_new_with_markup(
      GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_OTHER,
      GTK_BUTTONS_CLOSE, format, content);
    GtkStyleContext *dialog_ctx = gtk_widget_get_style_context(dialog);

    gtk_style_context_add_class(dialog_ctx, "dialog");
    GtkWidget *copy_btn = gtk_dialog_add_button(GTK_DIALOG(dialog), "Copy", -1);
    g_signal_connect(copy_btn, "clicked", G_CALLBACK(copy_data),
                     (gpointer) content);

    gtk_window_set_title(GTK_WINDOW(dialog), "Results");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    free(content);
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

    g_signal_connect(button, "clicked", G_CALLBACK(process_file),
                     (gpointer) path);

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