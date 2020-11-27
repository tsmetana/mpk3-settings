#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <gtk/gtk.h>

#include "message.h"
#include "presets.h"
#include "device.h"

#define PRESETS_ARRAY_LAST 0 /* This is safe since offset 0 is always SYSEX start byte */

/* Static table maitaining relationship between UI elements and the coreesponding message buffer byte */
static preset_t presets[] = {
	{OFF_PGM_NUM, 1, NULL},
	{OFF_PGM_NAME, 16, NULL},
	{OFF_PAD_MIDI_CH, 1, NULL},
	{OFF_AFTERTOUCH, 1, NULL},
	{OFF_KEYBED_CH, 1, NULL},
	{OFF_KEYBED_OCTAVE, 1, NULL},
	{OFF_ARP_SWITCH, 1, NULL},
	{OFF_ARP_MODE, 1, NULL},
	{OFF_ARP_DIVISION, 1, NULL},
	{OFF_CLK_SOURCE, 1, NULL},
	{OFF_LATCH, 1, NULL},
	{OFF_ARP_SWING, 1, NULL},
	{OFF_TEMPO_TAPS, 1, NULL},
	{OFF_TEMPO_BPM, 2, NULL},
	{OFF_ARP_OCTAVE, 1, NULL},
	{OFF_JOY_HORIZ_MODE, 1, NULL},
	{OFF_JOY_HORIZ_POSITIVE_CH, 1, NULL},
	{OFF_JOY_HORIZ_NEGATIVE_CH, 1, NULL},
	{OFF_JOY_VERT_MODE, 1, NULL},
	{OFF_JOY_VERT_POSITIVE_CH, 1, NULL},
	{OFF_JOY_VERT_NEGATIVE_CH, 1, NULL},
	{OFF_PAD_1_NOTE, 1, NULL},
	{OFF_PAD_1_PC, 1, NULL},
	{OFF_PAD_1_CC, 1, NULL},
	{OFF_PAD_2_NOTE, 1, NULL},
	{OFF_PAD_2_PC, 1, NULL},
	{OFF_PAD_2_CC, 1, NULL},
	{OFF_PAD_3_NOTE, 1, NULL},
	{OFF_PAD_3_PC, 1, NULL},
	{OFF_PAD_3_CC, 1, NULL},
	{OFF_PAD_4_NOTE, 1, NULL},
	{OFF_PAD_4_PC, 1, NULL},
	{OFF_PAD_4_CC, 1, NULL},
	{OFF_PAD_5_NOTE, 1, NULL},
	{OFF_PAD_5_PC, 1, NULL},
	{OFF_PAD_5_CC, 1, NULL},
	{OFF_PAD_6_NOTE, 1, NULL},
	{OFF_PAD_6_PC, 1, NULL},
	{OFF_PAD_6_CC, 1, NULL},
	{OFF_PAD_7_NOTE, 1, NULL},
	{OFF_PAD_7_PC, 1, NULL},
	{OFF_PAD_7_CC, 1, NULL},
	{OFF_PAD_8_NOTE, 1, NULL},
	{OFF_PAD_8_PC, 1, NULL},
	{OFF_PAD_8_CC, 1, NULL},
	{OFF_PAD_9_NOTE, 1, NULL},
	{OFF_PAD_9_PC, 1, NULL},
	{OFF_PAD_9_CC, 1, NULL},
	{OFF_PAD_10_NOTE, 1, NULL},
	{OFF_PAD_10_PC, 1, NULL},
	{OFF_PAD_10_CC, 1, NULL},
	{OFF_PAD_11_NOTE, 1, NULL},
	{OFF_PAD_11_PC, 1, NULL},
	{OFF_PAD_11_CC, 1, NULL},
	{OFF_PAD_12_NOTE, 1, NULL},
	{OFF_PAD_12_PC, 1, NULL},
	{OFF_PAD_12_CC, 1, NULL},
	{OFF_PAD_13_NOTE, 1, NULL},
	{OFF_PAD_13_PC, 1, NULL},
	{OFF_PAD_13_CC, 1, NULL},
	{OFF_PAD_14_NOTE, 1, NULL},
	{OFF_PAD_14_PC, 1, NULL},
	{OFF_PAD_14_CC, 1, NULL},
	{OFF_PAD_15_NOTE, 1, NULL},
	{OFF_PAD_15_PC, 1, NULL},
	{OFF_PAD_15_CC, 1, NULL},
	{OFF_PAD_16_NOTE, 1, NULL},
	{OFF_PAD_16_PC, 1, NULL},
	{OFF_PAD_16_CC, 1, NULL},
	{OFF_KNOB_1_MODE, 1, NULL},
	{OFF_KNOB_1_CC, 1, NULL},
	{OFF_KNOB_1_MIN, 1, NULL},
	{OFF_KNOB_1_MAX, 1, NULL},
	{OFF_KNOB_1_NAME, 16, NULL},
	{OFF_KNOB_2_MODE, 1, NULL},
	{OFF_KNOB_2_CC, 1, NULL},
	{OFF_KNOB_2_MIN, 1, NULL},
	{OFF_KNOB_2_MAX, 1, NULL},
	{OFF_KNOB_2_NAME, 16, NULL},
	{OFF_KNOB_3_MODE, 1, NULL},
	{OFF_KNOB_3_CC, 1, NULL},
	{OFF_KNOB_3_MIN, 1, NULL},
	{OFF_KNOB_3_MAX, 1, NULL},
	{OFF_KNOB_3_NAME, 16, NULL},
	{OFF_KNOB_4_MODE, 1, NULL},
	{OFF_KNOB_4_CC, 1, NULL},
	{OFF_KNOB_4_MIN, 1, NULL},
	{OFF_KNOB_4_MAX, 1, NULL},
	{OFF_KNOB_4_NAME, 16, NULL},
	{OFF_KNOB_5_MODE, 1, NULL},
	{OFF_KNOB_5_CC, 1, NULL},
	{OFF_KNOB_5_MIN, 1, NULL},
	{OFF_KNOB_5_MAX, 1, NULL},
	{OFF_KNOB_5_NAME, 16, NULL},
	{OFF_KNOB_6_MODE, 1, NULL},
	{OFF_KNOB_6_CC, 1, NULL},
	{OFF_KNOB_6_MIN, 1, NULL},
	{OFF_KNOB_6_MAX, 1, NULL},
	{OFF_KNOB_6_NAME, 16, NULL},
	{OFF_KNOB_7_MODE, 1, NULL},
	{OFF_KNOB_7_CC, 1, NULL},
	{OFF_KNOB_7_MIN, 1, NULL},
	{OFF_KNOB_7_MAX, 1, NULL},
	{OFF_KNOB_7_NAME, 16, NULL},
	{OFF_KNOB_8_MODE, 1, NULL},
	{OFF_KNOB_8_CC, 1, NULL},
	{OFF_KNOB_8_MIN, 1, NULL},
	{OFF_KNOB_8_MAX, 1, NULL},
	{OFF_KNOB_8_NAME, 16, NULL},
	{OFF_TRANSPOSE, 1, NULL},
	{PRESETS_ARRAY_LAST, 0, NULL},
};


static preset_t *get_preset(guint offset_num)
{
	preset_t *p = NULL;
	int i;

	for (i = 0; i < PRESETS_NUM; i++) {
		if (presets[i].offset == offset_num) {
			p = &(presets[i]);
			break;
		}
	}

	return p;
}


void presets_sync_buf_from_ui(void)
{
	gint i, j, val;
	gchar *str;
	guint offset;

	for (i = 0; i < PRESETS_NUM; i++) {
		offset = presets[i].offset;
		if (GTK_IS_COMBO_BOX(presets[i].ui)) {
			val = g_ascii_strtoll(gtk_combo_box_get_active_id(GTK_COMBO_BOX(presets[i].ui)), NULL, 10);
			device_set_buffer_byte(offset, val);
		} else if (GTK_IS_SPIN_BUTTON(presets[i].ui)) {
			val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(presets[i].ui));
			switch (offset) {
				case OFF_KEYBED_OCTAVE:
					device_set_buffer_byte(offset, val + 4);
					break;
				case OFF_PAD_MIDI_CH:
				case OFF_KEYBED_CH:
				case OFF_ARP_OCTAVE:
					device_set_buffer_byte(offset, val - 1);
					break;
				case OFF_ARP_SWING:
					device_set_buffer_byte(offset, val - ARP_SWING_BASE);
					break;
				case OFF_TRANSPOSE:
					device_set_buffer_byte(offset, val + 12);
					break;
				case OFF_TEMPO_BPM:
					device_set_buffer_byte(offset, (val >> 7) & 127);  /* 7 upper bits */
					device_set_buffer_byte(offset + 1, val & 127);     /* 7 lower bits */
					break;
				default:
					device_set_buffer_byte(offset, val);
					break;
			}
		} else if (GTK_IS_ENTRY(presets[i].ui)) {
			str = g_strnfill(NAME_STR_LEN, '\0');
			/* No need for NULL-terminator */
			strncpy(str, gtk_entry_get_text(GTK_ENTRY(presets[i].ui)), NAME_STR_LEN);
			for (j = 0; j < NAME_STR_LEN; j++)
					device_set_buffer_byte(offset + j, str[j]);
			g_free(str);
		} else if (GTK_IS_SWITCH(presets[i].ui)) {
			val = gtk_switch_get_state(GTK_SWITCH(presets[i].ui));
			device_set_buffer_byte(offset, val);
		}
	}
}


void presets_sync_ui_from_buf(void)
{
	guchar *buf = device_dup_buffer();
	gchar *str;
	gint i, val;
	guint offset;

	for (i = 0; i < PRESETS_NUM; i++) {
		offset = presets[i].offset;
		if (GTK_IS_COMBO_BOX(presets[i].ui)) {
			str = g_strdup_printf("%d", buf[offset]);
			if (!gtk_combo_box_set_active_id(GTK_COMBO_BOX(presets[i].ui), str))
				g_warning("Error setting preset %d combo box to %s", i, str);
			g_free(str);
		} else if (GTK_IS_SPIN_BUTTON(presets[i].ui)) {
			switch (offset) {
				case OFF_KEYBED_OCTAVE:
					val = (gint)buf[offset] - 4;
					break;
				case OFF_PAD_MIDI_CH:
				case OFF_KEYBED_CH:
				case OFF_ARP_OCTAVE:
					val = (gint)buf[offset] + 1;
					break;
				case OFF_ARP_SWING:
					val = (gint)buf[offset] + ARP_SWING_BASE;
					break;
				case OFF_TRANSPOSE:
					val = (gint)buf[offset] - 12;
					break;
				case OFF_TEMPO_BPM:
					val = (gint)((buf[offset] << 7) | buf[offset + 1]);
					break;
				default:
					val = (gint)buf[offset];
					break;
			}
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(presets[i].ui), (gdouble)val);
		} else if (GTK_IS_ENTRY(presets[i].ui)) {
			str = g_strndup((const gchar*)&buf[offset], NAME_STR_LEN);
			gtk_entry_set_text(GTK_ENTRY(presets[i].ui), str);
			g_free(str);
		} else if (GTK_IS_SWITCH(presets[i].ui)) {
			gtk_switch_set_state(GTK_SWITCH(presets[i].ui), (gboolean)buf[offset]);
		}
	}
	g_free(buf);
}


GtkWidget *preset_get_ui_element(guint preset_offset)
{
	preset_t *p = get_preset(preset_offset);
	if (p) {
		g_debug("Found UI element for preset offset %d", preset_offset);
		return p->ui;
	} else {
		g_warning("Could not find UI element for preset: no preset with offset %d found", preset_offset);
		return NULL;
	}
}


void preset_set_ui_element(guint preset_offset, GtkWidget *widget)
{
	preset_t *p = get_preset(preset_offset);

	if (p) {
		g_debug("Found UI element for preset offset %d", preset_offset);
		p->ui = widget;
	} else {
		g_warning("Could not assign UI element to preset: no preset with offset %d found", preset_offset);
	}
}


gsize preset_get_length(guint preset_offset)
{
	gsize len = 0;
	int i;

	for (i = 0; i < PRESETS_NUM; i++) {
		if (presets[i].offset == preset_offset) {
			len = presets[i].len;
			break;
		}
	}

	return len;
}
