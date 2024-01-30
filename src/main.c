#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <gtk/gtk.h>

#include "app_win.h"
#include "message.h"
#include "device.h"

void show_error(void)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(NULL,
			GTK_RESPONSE_NONE,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE,
			"Error: No suitable device found.");
	gtk_window_set_title(GTK_WINDOW(dialog), "MPKmini MK3 Settings");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

static void
activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;

	window = app_win_create(app);
	gtk_widget_show_all(window);
	gtk_window_present(GTK_WINDOW(window));
}


int main(int argc, char ** argv)
{
	GtkApplication *app;
	int status;

	gtk_init(&argc, &argv);
	if (device_init() < 0) {
		show_error();
		return -1;
	}
	if (device_read_pgm(PGM_NUM_RAM) < 0) {
		show_error();
		return -1;
	}
	app = gtk_application_new(APP_ID_DOMAIN, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	device_close();

	return status;
}
