#include <gtk/gtk.h>
#include <vte/vte.h>


static void on_window_destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

// Función callback para manejar el evento 'activate' de la aplicación
static void on_app_activate(GApplication *app, gpointer data)
{
    // Crear la ventana principal
    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "Terminal");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Crear un widget de texto para mostrar la salida de la terminal
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);

    // Crear un desplazamiento para el widget de texto
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), text_view);
    
        // Crear un contenedor de pestañas
    GtkWidget *notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // Crear una nueva pestaña con una vista de terminal
    GtkWidget *terminal = vte_terminal_new();
    vte_terminal_spawn_sync(VTE_TERMINAL(terminal),
                            VTE_PTY_DEFAULT,
                            NULL,  // Directorio de trabajo
                            (char *[]){"/bin/bash", NULL}, // Comando a ejecutar
                            NULL, NULL, NULL,
                            NULL, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), terminal, gtk_label_new("Terminal"));


    // Obtener el buffer de texto del widget de texto
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Conectar el buffer de texto a una tubería para capturar la salida de la terminal
    GIOChannel *channel;
    GError *error = NULL;
    g_spawn_close_pid(G_SPAWN_DO_NOT_REAP_CHILD, NULL);
    gboolean result = g_spawn_async_with_pipes(NULL, // Ruta del ejecutable de la terminal
                                              argv, // Argumentos de la terminal
                                              NULL, // Variables de entorno
                                              G_SPAWN_SEARCH_PATH,
                                              NULL, // Función para redirigir la entrada (NULL en este ejemplo)
                                              &channel, // Canal de salida para capturar la salida de la terminal
                                              NULL, // Canal de entrada (NULL en este ejemplo)
                                              NULL, // Canal de error (NULL en este ejemplo)
                                              NULL, // PID del proceso (no se necesita en este ejemplo)
                                              &error);

    if (result) {
        // Leer la salida de la terminal línea por línea y mostrarla en el widget de texto
        gchar *line = NULL;
        gsize length = 0;
        while (g_io_channel_read_line(channel, &line, &length, NULL, NULL) == G_IO_STATUS_NORMAL) {
            gtk_text_buffer_insert_at_cursor(buffer, line, -1);
            g_free(line);
            line = NULL;
        }

        // Cerrar el canal de salida
        g_io_channel_shutdown(channel, TRUE, NULL);

        // Liberar el canal de salida
        g_io_channel_unref(channel);
    } else {
        // Mostrar un mensaje de error si no se puede ejecutar la terminal
        g_print("Error: %s\n", error->message);
        g_error_free(error);
    }

    // Agregar el desplazamiento a la ventana principal
    gtk_container_add(GTK_CONTAINER(window), scroll);

    // Conectar el evento 'destroy' de la ventana principal a la función callback
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Mostrar todos los widgets
    gtk_widget_show_all(window);
}


int main(int argc, char **argv)
{
    // Crear una nueva aplicación GTK
    GtkApplication *app = gtk_application_new("com.example.terminal", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);

    // Ejecutar la aplicación
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Liberar la memoria de la aplicación
    g_object_unref(app);

    return status;
}
