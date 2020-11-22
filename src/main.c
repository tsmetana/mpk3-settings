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
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

int main(int argc, char ** argv)
{
	GtkWidget *app_win;
	
	gtk_init(&argc, &argv);
	if (device_read_pgm(PGM_NUM_RAM) < 0) {
		show_error();
		return -1;
	}
	app_win = app_win_create();
	gtk_widget_show_all(app_win);
	gtk_main();

	return 0;
}
