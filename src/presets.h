#ifndef PRESETS_H
#define PRESETS_H

enum _preset_type_e {
	PRESET_TYPE_STR,
	PRESET_TYPE_ENUM,
	PRESET_TYPE_INT
};

typedef enum _preset_type_e preset_type_t;

struct _preset_s {
	guint offset;
	GtkWidget *ui;
};

typedef struct _preset_s preset_t;

/* Syncs the values from UI elements to the buffer */
void presets_sync_buf_from_ui(void);

/* Syncs the UI from the buffer value */
void presets_sync_ui_from_buf(void);

/* Gets the UI element of the preset for a given message offset value */
GtkWidget *preset_get_ui_element(guint preset_offset);

/* Associate the UI widget with the given preset offset */
void preset_set_ui_element(guint preset_offset, GtkWidget *widget);

#endif
