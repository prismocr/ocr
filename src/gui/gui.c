#include <gtk/gtk.h>
#include "utils/matrix.h"
#include "utils/error.h"
#include "utils/bitmap.h"
#include "imgproc/rotation.h"

GtkWidget *input;

int start_gui(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;

    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "layout/main.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    
    // get pointers to the two labels
    GTK_WIDGET(gtk_builder_get_object(builder, "mybutton"));
    input = GTK_WIDGET(gtk_builder_get_object(builder, "file_input"));

    g_object_unref(builder);

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "./layout/styles.css", NULL);

    GdkScreen *screen = gdk_screen_get_default();

    gtk_style_context_add_provider_for_screen(screen,
                GTK_STYLE_PROVIDER(cssProvider),
                GTK_STYLE_PROVIDER_PRIORITY_USER);
    // css_set(cssProvider, window);
    gtk_widget_show(window);                
    gtk_main();

    return 0;
}

void on_mybutton_clicked()
{
    printf("hello\n");
}

void on_file(GtkFileChooser *filechooserbutton1) {
    printf("file choosen\n");
    gchar *str = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(filechooserbutton1));
    printf("%s\n", str);

    Matrix image;
    // Remove "file://"
    char* path = str + 7;
    printf("%s\n", path);
    exit_on_error(bitmap_load(path, &image));
    image_auto_rotate(&image, 0.01f);

    exit_on_error(bitmap_save("outgui.bmp", &image));

    matrix_free(&image);

    // input->parent_instance.qdata
}

// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}