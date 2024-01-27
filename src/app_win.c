#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <gtk/gtk.h>

#include "app_win.h"
#include "message.h"
#include "presets.h"
#include "device.h"
#include "file.h"

#define JOY_UI_HORIZONTAL 0
#define JOY_UI_VERTICAL   1
#define PAD_BANK_NUM      2

#define NOTES_NUM 11
static const gchar *NOTE_NAME[] = {
	"C", "C#", "D","D#", "E", "F", "G", "G#", "A", "Bb", "B"
};

static const gchar *TIME_DIV_NAME[] = {
	[ARP_DIV_1_4] = "1/4",
	[ARP_DIV_1_4T] = "1/4T",
	[ARP_DIV_1_8] = "1/8",
	[ARP_DIV_1_8T] = "1/8T",
	[ARP_DIV_1_16] = "1/16",
	[ARP_DIV_1_16T] = "1/16T",
	[ARP_DIV_1_32] = "1/32",
	[ARP_DIV_1_32T] = "1/32T",
};

static const gchar *ARP_MODE_NAME[] = {
	[ARP_MODE_UP] = "Up",
	[ARP_MODE_DOWN] = "Down",
	[ARP_MODE_EXCL] = "Excl",
	[ARP_MODE_INCL] = "Incl",
	[ARP_MODE_ORDER] = "Order",
	[ARP_MODE_RAND] = "Rand",
};

struct _joystick_mode_ui_s {
	GtkWidget *combo;
	GtkWidget *pos_icon;
	GtkWidget *neg_icon;
	GtkWidget *cc_label;
	GtkWidget *pos_entry;
	GtkWidget *neg_entry;
};

typedef struct _joystick_mode_ui_s joystick_mode_ui_t;

enum _file_dialog_e {
	FILE_OPEN_DIALOG,
	FILE_SAVE_DIALOG
};

typedef enum _file_dialog_e file_dialog_t;


static gchar *file_choose_dialog(GtkWidget *app_win, file_dialog_t dlg_type)
{
	GtkFileChooserNative *dialog;
	GtkFileChooserAction action;
	const gchar *title;
	const gchar *btn_label;
	gchar *filename = NULL;
	GtkFileFilter *filter_all = gtk_file_filter_new();
	GtkFileFilter *filter_mpk = gtk_file_filter_new();

	if (dlg_type == FILE_OPEN_DIALOG) {
		action = GTK_FILE_CHOOSER_ACTION_OPEN;
		title = "Open File";
		btn_label = "Open";
	} else {
		action = GTK_FILE_CHOOSER_ACTION_SAVE;
		title = "Save File";
		btn_label = "Save";
	}
	dialog = gtk_file_chooser_native_new(title, GTK_WINDOW(app_win), action,
			btn_label, "Cancel");
	if (dlg_type == FILE_SAVE_DIALOG)
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
	else
		gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), FALSE);

	gtk_file_filter_add_pattern(filter_mpk, "*.mpkmini3");
	gtk_file_filter_set_name(filter_mpk, "mpkmini3 files");
	gtk_file_filter_add_pattern(filter_all, "*");
	gtk_file_filter_set_name(filter_all, "All files");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter_mpk);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter_all);

	if ((gtk_native_dialog_run(GTK_NATIVE_DIALOG(dialog))) == GTK_RESPONSE_ACCEPT)
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	g_object_unref(dialog);

	return filename;
}


static void on_open_menu_item_activate(GtkWidget *menu_item, gpointer user_data)
{
	gchar *filename = file_choose_dialog(GTK_WIDGET(user_data), FILE_OPEN_DIALOG);
	GError *err;

	if (filename) {
		g_debug("Reading %s", filename);
		if (!file_read(filename, &err)) {
			g_critical("Read error: %s", err->message);
			g_error_free(err);
		} else {
			/* Sync the UI from the buffer if the data were read OK */
			presets_sync_ui_from_buf();
		}
		g_free(filename);
	}
}


static void on_save_menu_item_activate(GtkWidget *menu_item, gpointer user_data)
{
	gchar *filename = file_choose_dialog(GTK_WIDGET(user_data), FILE_SAVE_DIALOG);
	GError *err;

	if (filename) {
		presets_sync_buf_from_ui();
		g_debug("Writing %s", filename);
		if (!file_write(filename, &err)) {
			g_critical("Write error: %s", err->message);
			g_error_free(err);
		}
		g_free(filename);
	}
}


static void on_exit_menu_item_activate(GtkWidget *menu_item, gpointer user_data)
{
	gtk_main_quit();
}


static gint get_selected_pgm_num(GtkComboBox *combo)
{
	const gchar *selected_val_str;

	if (!(selected_val_str = gtk_combo_box_get_active_id(combo))) {
		g_warning("No valid program selected");
		return -1;
	}
	return g_ascii_strtoll(selected_val_str, NULL, 10);
}


static void on_read_pgm_button_click(GtkWidget *button, gpointer user_data)
{
	gint pgm_num = get_selected_pgm_num(GTK_COMBO_BOX(user_data));
	
	if (pgm_num < 0)
		return;
	g_debug("Read: Selected program number: %d", pgm_num);
	if (device_read_pgm(pgm_num) < 0)
		return;
	presets_sync_ui_from_buf();
}


static void on_write_pgm_button_click(GtkWidget *button, gpointer user_data)
{
	gint pgm_num = get_selected_pgm_num(GTK_COMBO_BOX(user_data));
	
	if (pgm_num < 0)
		return;
	g_debug("Write: Selected program number: %d", pgm_num);
	presets_sync_buf_from_ui();
	device_write_pgm(pgm_num);
}


static GtkWidget *file_menu_create(GtkWidget *main_window)
{
	GtkWidget *file_menu_item = gtk_menu_item_new_with_label("File");
	GtkWidget *file_menu = gtk_menu_new();
	GtkWidget *open_file_menu_item = gtk_menu_item_new_with_label("Open ...");
	GtkWidget *save_file_menu_item = gtk_menu_item_new_with_label("Save as ...");
	GtkWidget *exit_file_menu_item = gtk_menu_item_new_with_label("Exit");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_file_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_file_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), gtk_separator_menu_item_new());
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), exit_file_menu_item);

	g_signal_connect((gpointer)open_file_menu_item, "activate", G_CALLBACK(on_open_menu_item_activate), main_window);
	g_signal_connect((gpointer)save_file_menu_item, "activate", G_CALLBACK(on_save_menu_item_activate), main_window);
	g_signal_connect((gpointer)exit_file_menu_item, "activate", G_CALLBACK(on_exit_menu_item_activate), NULL);
	
	return file_menu_item;
}


static GtkWidget *toolbar_create(void)
{
	GtkWidget *toolbar_vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget *pgm_selector = gtk_combo_box_text_new();
	GtkWidget *read_pgm_button = gtk_button_new_from_icon_name("folder-download-symbolic", GTK_ICON_SIZE_BUTTON);
	GtkWidget *write_pgm_button = gtk_button_new_from_icon_name("send-to-symbolic", GTK_ICON_SIZE_BUTTON);
	GtkWidget *pgm_name_label = gtk_label_new("Program name:");
	GtkWidget *pgm_name_entry = gtk_entry_new();
	gchar pgm_label[] = "Program #";
	gchar pgm_id[] = "#";
	gint i;

	gtk_widget_set_tooltip_text(read_pgm_button, "Read settings from the device");
	gtk_widget_set_tooltip_text(write_pgm_button, "Write current settings to the device");
	gtk_widget_set_halign(pgm_name_label, GTK_ALIGN_END);
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(pgm_selector), "0", "RAM");
	for (i = 1; i <= PGM_NUM_MAX; i++) {
		pgm_id[0] = pgm_label[strlen(pgm_label) - 1] = '0' + i;
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(pgm_selector), pgm_id, pgm_label);
	}
	gtk_entry_set_max_length(GTK_ENTRY(pgm_name_entry), NAME_STR_LEN);
	gtk_combo_box_set_active_id(GTK_COMBO_BOX(pgm_selector), "0");
	gtk_box_pack_start(GTK_BOX(toolbar_vbox), pgm_selector, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar_vbox), read_pgm_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar_vbox), write_pgm_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar_vbox), pgm_name_label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(toolbar_vbox), pgm_name_entry, FALSE, FALSE, 0);

	preset_set_ui_element(OFF_PGM_NUM, pgm_selector);
	preset_set_ui_element(OFF_PGM_NAME, pgm_name_entry);
	
	g_signal_connect((gpointer)read_pgm_button, "clicked", G_CALLBACK(on_read_pgm_button_click), pgm_selector);
	g_signal_connect((gpointer)write_pgm_button, "clicked", G_CALLBACK(on_write_pgm_button_click), pgm_selector);

	return toolbar_vbox;
}


static void on_joystick_frame_destroy(GtkWidget *frame, gpointer user_data)
{
	joystick_mode_ui_t **ui = (joystick_mode_ui_t **)user_data;

	g_free(ui[JOY_UI_HORIZONTAL]);
	g_free(ui[JOY_UI_VERTICAL]);
	g_free(ui);
}


static void on_joystick_mode_combo_changed(GtkWidget *combo, gpointer user_data)
{
	joystick_mode_ui_t *ui = (joystick_mode_ui_t *)user_data;

	switch (gtk_combo_box_get_active(GTK_COMBO_BOX(combo))) {
		case 0:
			gtk_widget_hide(ui->pos_icon);
			gtk_widget_hide(ui->neg_icon);
			gtk_widget_hide(ui->cc_label);
			gtk_widget_hide(ui->pos_entry);
			gtk_widget_hide(ui->neg_entry);
			break;
		case 1:
			gtk_widget_hide(ui->pos_icon);
			gtk_widget_hide(ui->neg_icon);
			gtk_widget_show(ui->cc_label);
			gtk_widget_show(ui->neg_entry);
			gtk_widget_hide(ui->pos_entry);
			break;
		case 2:
			gtk_widget_show(ui->pos_icon);
			gtk_widget_show(ui->neg_icon);
			gtk_widget_hide(ui->cc_label);
			gtk_widget_show(ui->pos_entry);
			gtk_widget_show(ui->neg_entry);
			break;
		default:
			g_critical("Unkonwn joystick mode selected");
			break;
	}
}


static GtkWidget *joystick_ui_create(void)
{
	GtkWidget *frame = gtk_frame_new("Joystick");
	GtkWidget *grid = gtk_grid_new();
	GtkWidget *h_mode_combo = gtk_combo_box_text_new();
	GtkWidget *h_pos_entry = gtk_spin_button_new_with_range(CHANNEL_MIN, CHANNEL_MAX, 1.0);
	GtkWidget *h_pos_icon = gtk_image_new_from_icon_name("go-next", GTK_ICON_SIZE_MENU);
	GtkWidget *h_label = gtk_label_new("CC");
	GtkWidget *h_neg_entry = gtk_spin_button_new_with_range(CHANNEL_MIN, CHANNEL_MAX, 1.0);
	GtkWidget *h_neg_icon = gtk_image_new_from_icon_name("go-previous", GTK_ICON_SIZE_MENU);
	GtkWidget *v_mode_combo = gtk_combo_box_text_new();
	GtkWidget *v_pos_entry = gtk_spin_button_new_with_range(CHANNEL_MIN, CHANNEL_MAX, 1.0);
	GtkWidget *v_pos_icon = gtk_image_new_from_icon_name("go-up", GTK_ICON_SIZE_MENU);
	GtkWidget *v_label = gtk_label_new("CC");
	GtkWidget *v_neg_entry = gtk_spin_button_new_with_range(CHANNEL_MIN, CHANNEL_MAX, 1.0);
	GtkWidget *v_neg_icon = gtk_image_new_from_icon_name("go-down", GTK_ICON_SIZE_MENU);
	joystick_mode_ui_t **ui = g_malloc(sizeof(joystick_mode_ui_t **));
	
	ui[JOY_UI_HORIZONTAL] = g_malloc(sizeof(joystick_mode_ui_t));
	ui[JOY_UI_VERTICAL] = g_malloc(sizeof(joystick_mode_ui_t));

	ui[JOY_UI_HORIZONTAL]->combo = h_mode_combo;
	ui[JOY_UI_HORIZONTAL]->pos_icon = h_pos_icon;
	ui[JOY_UI_HORIZONTAL]->neg_icon = h_neg_icon;
	ui[JOY_UI_HORIZONTAL]->cc_label = h_label;
	ui[JOY_UI_HORIZONTAL]->pos_entry = h_pos_entry;
	ui[JOY_UI_HORIZONTAL]->neg_entry = h_neg_entry;
	ui[JOY_UI_VERTICAL]->combo = v_mode_combo;
	ui[JOY_UI_VERTICAL]->pos_icon = v_pos_icon;
	ui[JOY_UI_VERTICAL]->neg_icon = v_neg_icon;
	ui[JOY_UI_VERTICAL]->cc_label = v_label;
	ui[JOY_UI_VERTICAL]->pos_entry = v_pos_entry;
	ui[JOY_UI_VERTICAL]->neg_entry = v_neg_entry;

	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
	g_signal_connect((gpointer)frame, "destroy", G_CALLBACK(on_joystick_frame_destroy), ui);
	g_signal_connect((gpointer)h_mode_combo, "changed", G_CALLBACK(on_joystick_mode_combo_changed), ui[JOY_UI_HORIZONTAL]);
	g_signal_connect((gpointer)v_mode_combo, "changed", G_CALLBACK(on_joystick_mode_combo_changed), ui[JOY_UI_VERTICAL]);

	gtk_grid_attach(GTK_GRID(grid), h_mode_combo, 0, 0, 4, 1);
	gtk_grid_attach(GTK_GRID(grid), h_neg_icon,   0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), h_label,      0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), h_neg_entry,  1, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(grid), h_pos_icon,   0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), h_pos_entry,  1, 2, 2, 1);
	gtk_grid_attach(GTK_GRID(grid), v_mode_combo, 0, 3, 4, 1);
	gtk_grid_attach(GTK_GRID(grid), v_pos_icon,   0, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), v_label,      0, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), v_pos_entry,  1, 4, 2, 1);
	gtk_grid_attach(GTK_GRID(grid), v_neg_icon,   0, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), v_neg_entry,  1, 5, 2, 1);

	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(h_mode_combo), "0", "Pitchbend");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(h_mode_combo), "1", "Single CC");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(h_mode_combo), "2", "Dual CC");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(v_mode_combo), "0", "Pitchbend");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(v_mode_combo), "1", "Single CC");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(v_mode_combo), "2", "Dual CC");
	
	gtk_container_add(GTK_CONTAINER(frame), grid);

	preset_set_ui_element(OFF_JOY_HORIZ_MODE, h_mode_combo);
	preset_set_ui_element(OFF_JOY_HORIZ_POSITIVE_CH, h_pos_entry);
	preset_set_ui_element(OFF_JOY_HORIZ_NEGATIVE_CH, h_neg_entry);
	preset_set_ui_element(OFF_JOY_VERT_MODE, v_mode_combo);
	preset_set_ui_element(OFF_JOY_VERT_POSITIVE_CH, v_pos_entry);
	preset_set_ui_element(OFF_JOY_VERT_NEGATIVE_CH, v_neg_entry);
	
	return frame;
}


static void on_note_entry_value_changed(GtkWidget *spin, gpointer user_data)
{
	GtkLabel *note_label = GTK_LABEL(user_data);
	gint note_number = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));

	gtk_label_set_text(note_label, NOTE_NAME[note_number % NOTES_NUM]);
}

static GtkWidget *single_pad_ui_create(gint bank_num, gint pad_num)
{
	GtkWidget *frame;
	gchar *pad_name;
	gint base_offset = OFF_PAD_SETTINGS_START + (pad_num + bank_num * PADS_PER_BANK) * PAD_SETTINGS_LEN;
	GtkWidget *grid = gtk_grid_new();
	GtkWidget *note_entry = gtk_spin_button_new_with_range(0.0, 127.0, 1.0); /* FIXME: magic numbers */
	GtkWidget *note_label = gtk_label_new("C");
	GtkWidget *note_entry_label = gtk_label_new("Note");
	GtkWidget *cc_label = gtk_label_new("CC");
	GtkWidget *pc_label = gtk_label_new("PC");
	GtkWidget *cc_entry = gtk_spin_button_new_with_range(CHANNEL_MIN, CHANNEL_MAX, 1.0);
	GtkWidget *pc_entry = gtk_spin_button_new_with_range(CHANNEL_MIN, CHANNEL_MAX, 1.0);
	
	pad_name = g_strdup_printf("Pad %d", pad_num + 1);
	g_debug("Setting pad %s, bank %d, offset %d", pad_name, bank_num, base_offset);
	frame = gtk_frame_new(pad_name);
	g_free(pad_name);

	gtk_widget_set_halign(note_label, GTK_ALIGN_END);
	gtk_widget_set_halign(cc_label, GTK_ALIGN_END);
	gtk_widget_set_halign(pc_label, GTK_ALIGN_END);
	
	gtk_grid_attach(GTK_GRID(grid), note_entry_label, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), note_entry,       1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), note_label,       1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), cc_label,         0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), cc_entry,         1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), pc_label,         0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), pc_entry,         1, 3, 1, 1);

	g_signal_connect((gpointer)note_entry, "value-changed", G_CALLBACK(on_note_entry_value_changed), note_label);

	preset_set_ui_element(base_offset++, note_entry);
	preset_set_ui_element(base_offset++, pc_entry);
	preset_set_ui_element(base_offset, cc_entry);
	
	gtk_container_add(GTK_CONTAINER(frame), grid);

	return frame;
}


static GtkWidget *pads_ui_create(void)
{
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *frame;
	GtkWidget *grid;
	const gchar *pad_name[PAD_BANK_NUM] = {"Bank A", "Bank B"};
	GtkWidget *pad;
	gint bank_num, pad_num;
	gint row, col;

	for (bank_num = 0; bank_num < PAD_BANK_NUM; bank_num++) {
		frame = gtk_frame_new(pad_name[bank_num]);
		grid = gtk_grid_new();
		pad_num = 0;
		for (row = 1; row >= 0; row--) {
			for (col = 0; col < PADS_PER_BANK / 2; col++) {
				pad = single_pad_ui_create(bank_num, pad_num++);
				gtk_grid_attach(GTK_GRID(grid), pad, col, row, 1, 1);
			}
		}
		gtk_container_add(GTK_CONTAINER(frame), grid);
		gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);
	}

	return vbox;
}


static GtkWidget *single_knob_ui_create(gint knob_num)
{
	GtkWidget *frame;
	GtkWidget *grid = gtk_grid_new();
	gchar *knob_name;
	gint base_offset = OFF_KNOB_SETTINGS_START + knob_num * KNOB_SETTINGS_LEN;
	GtkWidget *knob_label_entry = gtk_entry_new();
	GtkWidget *cc_label = gtk_label_new("CC");
	GtkWidget *cc_entry = gtk_spin_button_new_with_range(CHANNEL_MIN, CHANNEL_MAX, 1.0);
	GtkWidget *hi_label = gtk_label_new("HI");
	GtkWidget *hi_entry = gtk_spin_button_new_with_range(0.0, 127.0, 1.0); /* FIXME: magic numbers */
	GtkWidget *lo_label = gtk_label_new("LO");
	GtkWidget *lo_entry = gtk_spin_button_new_with_range(0.0, 127.0, 1.0); /* FIXME: magic numbers */
	GtkWidget *mode_combo = gtk_combo_box_text_new();

	gtk_entry_set_max_length(GTK_ENTRY(knob_label_entry), NAME_STR_LEN);
	knob_name = g_strdup_printf("K%d", knob_num + 1);
	g_debug("Setting knob %s, offset %d", knob_name, base_offset);
	frame = gtk_frame_new(knob_name);
	g_free(knob_name);
	
	gtk_grid_attach(GTK_GRID(grid), knob_label_entry, 0, 0, 2, 1);
	gtk_grid_attach(GTK_GRID(grid), cc_label, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), cc_entry, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), hi_label, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), hi_entry, 1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), lo_label, 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), lo_entry, 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), mode_combo, 0, 4, 2, 1);
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(mode_combo), "0", "Absolute");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(mode_combo), "1", "Relative");
	
	preset_set_ui_element(base_offset++, mode_combo);
	preset_set_ui_element(base_offset++, cc_entry);
	preset_set_ui_element(base_offset++, lo_entry);
	preset_set_ui_element(base_offset++, hi_entry);
	preset_set_ui_element(base_offset, knob_label_entry);
	
	gtk_container_add(GTK_CONTAINER(frame), grid);

	return frame;
}


static GtkWidget *knobs_ui_create(void)
{
	GtkWidget *grid = gtk_grid_new();
	GtkWidget *knob;
	gint row, col, knob_num = 0;

	for (row = 0; row < 2; row++) {
		for (col = 0; col < (KNOBS_NUM / 2); col++) {
			knob = single_knob_ui_create(knob_num++);
			gtk_grid_attach(GTK_GRID(grid), knob, col, row, 1, 1);
		}
	}
	
	return grid;
}


static GtkWidget *channels_ui_create(void)
{
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget *pad_ch_label = gtk_label_new("Pad MIDI channel");
	GtkWidget *aftertouch_label = gtk_label_new("Aftertouch");
	GtkWidget *keybed_ch_label = gtk_label_new("Keybed / Controls MIDI channel");
	GtkWidget *pad_ch_entry = gtk_spin_button_new_with_range(0.0, 16.0, 1.0); /* FIXME: magic numbers */
	GtkWidget *keybed_ch_entry = gtk_spin_button_new_with_range(0.0, 16.0, 1.0); /* FIXME: magic numbers */
	GtkWidget *aftertouch_combo = gtk_combo_box_text_new();

	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(aftertouch_combo), "0", "Off");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(aftertouch_combo), "1", "Channel");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(aftertouch_combo), "2", "Polyphonic");
	gtk_widget_set_halign(aftertouch_label, GTK_ALIGN_END);
	
	gtk_box_pack_start(GTK_BOX(hbox), pad_ch_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), pad_ch_entry, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), aftertouch_label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), aftertouch_combo, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(hbox), keybed_ch_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), keybed_ch_entry, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	preset_set_ui_element(OFF_PAD_MIDI_CH, pad_ch_entry);
	preset_set_ui_element(OFF_AFTERTOUCH, aftertouch_combo);
	preset_set_ui_element(OFF_KEYBED_CH, keybed_ch_entry);
	
	return vbox;
}


static GtkWidget *keyboard_ui_create(void)
{
	GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget *transpose_label = gtk_label_new("Transpose");
	GtkWidget *octave_label = gtk_label_new("Octave");
	GtkWidget *transpose_entry = gtk_spin_button_new_with_range(-12.0, 12.0, 1.0); /* FIXME: magic numbers */
	GtkWidget *octave_entry = gtk_spin_button_new_with_range(-4.0, 4.0, 1.0); /* FIXME: magic numbers */

	gtk_box_pack_start(GTK_BOX(hbox), transpose_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), transpose_entry, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), octave_label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), octave_entry, FALSE, FALSE, 0);
	gtk_widget_set_halign(octave_label, GTK_ALIGN_END);
	gtk_adjustment_set_value(gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(transpose_entry)), 0.0);
	gtk_adjustment_set_value(gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(octave_entry)), 0.0);
	
	preset_set_ui_element(OFF_TRANSPOSE, transpose_entry);
	preset_set_ui_element(OFF_KEYBED_OCTAVE, octave_entry);

	return hbox;
}


static GtkWidget *arpeggiator_ui_create(void)
{
	GtkWidget *frame = gtk_frame_new("Arpeggiator");
	GtkWidget *grid = gtk_grid_new();
	GtkWidget *arp_switch_label = gtk_label_new("On / Off");
	GtkWidget *arp_switch = gtk_switch_new();
	gtk_widget_set_halign(arp_switch, GTK_ALIGN_START);
	gtk_widget_set_valign(arp_switch, GTK_ALIGN_CENTER);
	GtkWidget *tempo_taps_label = gtk_label_new("Tempo taps");
	GtkWidget *tempo_label = gtk_label_new("Tempo");
	GtkWidget *time_div_label = gtk_label_new("Time div");
	GtkWidget *clock_label = gtk_label_new("Clock");
	GtkWidget *latch_label = gtk_label_new("Latch");
	GtkWidget *octave_label = gtk_label_new("Octave");
	GtkWidget *mode_label = gtk_label_new("Mode");
	GtkWidget *swing_label = gtk_label_new("Swing");
	GtkWidget *tempo_taps_entry = gtk_spin_button_new_with_range(TEMPO_TAPS_MIN, TEMPO_TAPS_MAX, 1.0);
	GtkWidget *tempo_entry = gtk_spin_button_new_with_range(BPM_MIN, BPM_MAX, 1.0);
	GtkWidget *swing_entry = gtk_spin_button_new_with_range(ARP_SWING_BASE, ARP_SWING_BASE + ARP_SWING_MAX, 1.0);
	GtkWidget *octave_entry = gtk_spin_button_new_with_range(ARP_OCTAVE_MIN + 1.0, ARP_OCTAVE_MAX + 1.0, 1.0);
	GtkWidget *time_div_combo = gtk_combo_box_text_new();
	GtkWidget *clock_combo = gtk_combo_box_text_new();
	GtkWidget *mode_combo = gtk_combo_box_text_new();
	GtkWidget *latch_combo = gtk_combo_box_text_new();
	gint i;

	for (i = ARP_DIV_1_4; i<= ARP_DIV_1_32T; i++) {
		gchar *text_val = g_strdup_printf("%d", i);
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(time_div_combo), text_val, TIME_DIV_NAME[i]);
		g_free(text_val);
	}
	for (i = ARP_MODE_UP; i<= ARP_MODE_RAND; i++) {
		gchar *text_val = g_strdup_printf("%d", i);
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(mode_combo), text_val, ARP_MODE_NAME[i]);
		g_free(text_val);
	}
	
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(clock_combo), "0", "Internal");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(clock_combo), "1", "External");
	
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(latch_combo), "0", "Off");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(latch_combo), "1", "On");
	
	gtk_grid_attach(GTK_GRID(grid), arp_switch_label, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), arp_switch,       1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), time_div_label,   2, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), time_div_combo,   3, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), octave_label,     4, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), octave_entry,     5, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), tempo_taps_label, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), tempo_taps_entry, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), clock_label,      2, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), clock_combo,      3, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), mode_label,       4, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), mode_combo,       5, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), tempo_label,      0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), tempo_entry,      1, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), latch_label,      2, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), latch_combo,      3, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), swing_label,      4, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), swing_entry,      5, 2, 1, 1);

	preset_set_ui_element(OFF_ARP_SWITCH, arp_switch);
	preset_set_ui_element(OFF_ARP_MODE, mode_combo);
	preset_set_ui_element(OFF_ARP_DIVISION, time_div_combo);
	preset_set_ui_element(OFF_CLK_SOURCE, clock_combo);
	preset_set_ui_element(OFF_LATCH, latch_combo);
	preset_set_ui_element(OFF_ARP_SWING, swing_entry);
	preset_set_ui_element(OFF_TEMPO_TAPS, tempo_taps_entry);
	preset_set_ui_element(OFF_TEMPO_BPM, tempo_entry);
	preset_set_ui_element(OFF_ARP_OCTAVE, octave_entry);

	gtk_container_add(GTK_CONTAINER(frame), grid);

	return frame;
}


static GtkWidget *left_box_create(void)
{
	return joystick_ui_create();
}


static GtkWidget *middle_box_create(void)
{
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	gtk_box_pack_start(GTK_BOX(vbox), pads_ui_create(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), channels_ui_create(), FALSE, FALSE, 0);

	return vbox;
}


static GtkWidget *right_box_create(void)
{
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	gtk_box_pack_start(GTK_BOX(vbox), knobs_ui_create(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), keyboard_ui_create(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), arpeggiator_ui_create(), FALSE, FALSE, 0);

	return vbox;
}


static void on_app_win_realize(GtkWidget *win, gpointer user_data)
{
	if (device_read_pgm(PGM_NUM_RAM) < 0)
		return;
	presets_sync_ui_from_buf();
}


GtkWidget *app_win_create(GtkApplication *app)
{
	GtkWidget *main_window;
	GtkWidget *main_vbox;
	GtkWidget *main_menu;
	GtkWidget *main_hbox;

	main_window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(main_window), "MPKmini MK3 Settings");

	main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_spacing(GTK_BOX(main_vbox), 5);
	
	main_menu = gtk_menu_bar_new();
	gtk_container_add(GTK_CONTAINER(main_menu), file_menu_create(main_window));
	gtk_box_pack_start(GTK_BOX(main_vbox), main_menu, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), toolbar_create(), FALSE, FALSE, 0);

	main_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_spacing(GTK_BOX(main_hbox), 5);
	gtk_box_pack_start(GTK_BOX(main_hbox), left_box_create(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_hbox), middle_box_create(), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_hbox), right_box_create(), FALSE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(main_vbox), main_hbox, FALSE, FALSE, 0);
	
	gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
	
	g_signal_connect((gpointer)main_window, "realize", G_CALLBACK(on_app_win_realize), NULL);

	return main_window;
}
