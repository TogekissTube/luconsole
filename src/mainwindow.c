#include <gtk/gtk.h>
#include <vte/vte.h>


static void on_window_destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}


static void on_app_activate(GApplication *app, gpointer data)
{

    GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "Terminal");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);

 
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), text_view);

    GtkWidget *notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_container_add(GTK_CONTAINER(window), notebook);

    GtkWidget *terminal = vte_terminal_new();
    vte_terminal_spawn_sync(VTE_TERMINAL(terminal),
                            VTE_PTY_DEFAULT,
                            NULL,  
                            (char *[]){"/bin/bash", NULL}, 
                            NULL, NULL, NULL,
                            NULL, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), terminal, gtk_label_new("Terminal"));


 
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    GIOChannel *channel;
    GError *error = NULL;
    g_spawn_close_pid(G_SPAWN_DO_NOT_REAP_CHILD, NULL);
    gboolean result = g_spawn_async_with_pipes(NULL, 
                                              argv, 
                                              NULL, 
                                              G_SPAWN_SEARCH_PATH,
                                              NULL, 
                                              &channel, 
                                              NULL, 
                                              NULL, 
                                              NULL, 
                                              &error);

 
    
           if (result) {gchar *line = NULL;
        gsize length = 0;
        while (g_io_channel_read_line(channel, &line, &length, NULL, NULL) == G_IO_STATUS_NORMAL) {
            gtk_text_buffer_insert_at_cursor(buffer, line, -1);
            g_free(line);
            line = NULL;
        }

        g_io_channel_shutdown(channel, TRUE, NULL);


        g_io_channel_unref(channel);
    } else {
       
        g_print("Error: %s\n", error->message);
        g_error_free(error);
    }

    gtk_container_add(GTK_CONTAINER(window), scroll);

    
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Mostrar todos los widgets
    gtk_widget_show_all(window);
}


int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new("com.example.terminal", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
