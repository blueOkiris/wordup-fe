// Entry point for Wordup-FE

#include <gtk/gtk.h>

static const char *const g_title = "Wordup FE";
static const size_t g_winWidth = 1280;
static const size_t g_winHeight = 720;

static void appOnActivate(GtkApplication *app, gpointer user_data);
static void btnOnCloseClicked(GtkButton *button, gpointer window);

int main(int argc, char **argv) {
    auto app = gtk_application_new("net.polymath-studio.wordup-fe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(appOnActivate), nullptr);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

// Function to initialize the application
static void appOnActivate(GtkApplication *app, gpointer user_data) {
    // Create a new window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), g_title);
    gtk_window_set_default_size(GTK_WINDOW(window), g_winWidth, g_winHeight);

    // Create a button
    GtkWidget *button = gtk_button_new_with_label("Close");
    g_signal_connect(button, "clicked", G_CALLBACK(btnOnCloseClicked), window);

    // Add the button to the window
    gtk_window_set_child(GTK_WINDOW(window), button);

    // Show the window and all its children
    gtk_widget_set_visible(window, true);
}

static void btnOnCloseClicked(GtkButton *button, gpointer window) {
    gtk_window_close(GTK_WINDOW(window));
}

