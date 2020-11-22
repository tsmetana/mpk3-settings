#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <glib.h>
#include <rtmidi/rtmidi_c.h>
#include "message.h"
#include "device.h"

#define MPK3_DEV_NAME "MPK mini 3"
#define MPK3_APP_PORT_NAME "MPK3 Settings"

#define MIDI_IN_PTR(_dev) ((RtMidiInPtr)(_dev->in))
#define MIDI_OUT_PTR(_dev) ((RtMidiOutPtr)(_dev->out))

#define SYSEX_MSG_BUF_LEN 1024

struct _device_s {
	void *in;
	void *out;
};

typedef struct _device_s device_t;

/* There's only one device... */
static guchar msg_buf[SYSEX_MSG_BUF_LEN] = {0U};
static GMutex msg_buf_mutex;
static GCond msg_buf_cond;

static void dump_buffer_data(const guchar *data, const gsize len) {
	gint i, j;

	g_debug("Message buffer dump:");
	j = 1;
	for (i = 0; i < len; i++, j++) {
		g_print("0x%02x ", data[i]);
		if (j == 8) {
			g_print("\n");
			j = 0;
		}
	}
	g_print("\n");
}


static void midi_in_msg_callback(double time_stamp, const unsigned char *message, size_t len, void *user_data)
{
	/* Ignore things that can't be possibly the settings data */
	if (len != DATA_MSG_LEN) {
		g_debug("Got %zd bytes long event, ignoring", len);
		dump_buffer_data(message, len);
		return;
	}
	if (message[OFF_SYSEX_START] == SYSEX_START &&
			message[OFF_MF_ID] == MANUFACTURER_ID &&
			message[OFF_ADDR] == 0x00 &&
			message[OFF_PROD_ID] == PRODUCT_ID &&
			message[OFF_COMMAND] == CMD_INCOMING_DATA &&
			message[OFF_SYSEX_END] == SYSEX_END) {
		g_debug("Received program %d settings SYSEX message", message[OFF_PGM_NUM]);
		g_mutex_lock(&msg_buf_mutex);
		memcpy(msg_buf, message, len);
		g_cond_broadcast(&msg_buf_cond);
		g_mutex_unlock(&msg_buf_mutex);
	}
}


static gint query_program(device_t *dev, const gint pgm_num)
{
	RtMidiInPtr midi_out = MIDI_OUT_PTR(dev);
	RtMidiInPtr midi_in = MIDI_IN_PTR(dev);
	guchar query_msg[QUERY_MSG_LEN] = {
		SYSEX_START,
		MANUFACTURER_ID,
		MSG_DIRECTION_OUT,
		PRODUCT_ID,
		CMD_QUERY_DATA,
		0x00, /* Message length upper 7 bits */
		0x01, /* Message length lower 7 bits (i.e. there is only one byte, the pgm_num) */
		pgm_num,
		SYSEX_END
	};
	gint64 timeout;
	gint ret = -1;

	if (pgm_num < PGM_NUM_RAM || pgm_num > PGM_NUM_MAX) {
		g_critical("Cannot query program number %d (invalid)", pgm_num);
		return ret;
	}
	
	g_mutex_lock(&msg_buf_mutex);
	memset(msg_buf, 0, SYSEX_MSG_BUF_LEN);

	rtmidi_in_set_callback(midi_in, midi_in_msg_callback, NULL);
	rtmidi_out_send_message(midi_out, query_msg, QUERY_MSG_LEN);
	if (!(midi_out->ok)) {
		g_critical("Error querying program %d: %s", pgm_num, midi_out->msg);
		goto error;
	}
	g_debug("Queried program %d, waiting for response", pgm_num);

	timeout = g_get_monotonic_time() + G_TIME_SPAN_SECOND; /* one second from now */
	while (msg_buf[OFF_SYSEX_START] == 0) {
		if (!g_cond_wait_until(&msg_buf_cond, &msg_buf_mutex, timeout)) {
			g_warning("Timeout waiting for the SYSEX message");
			goto error;
		}
	}
	ret = 0;
	dump_buffer_data(msg_buf, DATA_MSG_LEN);
error:
	g_mutex_unlock(&msg_buf_mutex);
	rtmidi_in_cancel_callback(midi_in);
	
	return ret;
}


static device_t *device_init(void)
{
	device_t *dev;
	guint dev_count;
	const gchar *dev_name;
	RtMidiInPtr midi_in = NULL;
	RtMidiInPtr midi_out = NULL;
	guint i, port_num = 0;
	bool found = false;

	midi_in = rtmidi_in_create_default();
	dev_count = rtmidi_get_port_count(midi_in);
	g_debug("Found %d MIDI devices", dev_count);
	for (i = 0; i < dev_count; i++) {
		dev_name = rtmidi_get_port_name(midi_in, i);
		if ((strncmp(dev_name, MPK3_DEV_NAME, strlen(MPK3_DEV_NAME)) == 0)) {
			port_num = i;
			found = true;
			break;
		}
	}
	rtmidi_close_port(midi_in);
	if (!found) {
		g_critical("No MPKmini MK3 device found");
		rtmidi_in_free(midi_in);
		return NULL;
	}
	g_debug("Using device %d: %s", i, dev_name);
	midi_out = rtmidi_out_create_default();
	rtmidi_open_port(midi_in, port_num, MPK3_APP_PORT_NAME);
	if (!(midi_in->ok))
		g_warning("Error opening input device: %s", midi_in->msg);
	rtmidi_in_ignore_types(midi_in, FALSE, TRUE, TRUE);
	rtmidi_open_port(midi_out, port_num, MPK3_APP_PORT_NAME);
	if (!(midi_out->ok))
		g_warning("Error opening input device: %s", midi_out->msg);

	dev = g_malloc(sizeof(device_t *));
	dev->in = midi_in;
	dev->out = midi_out;
	
	return dev;
}


static void device_close(device_t *dev)
{

	rtmidi_close_port(MIDI_IN_PTR(dev));
	rtmidi_close_port(MIDI_OUT_PTR(dev));
	rtmidi_in_free(MIDI_IN_PTR(dev));
	rtmidi_out_free(MIDI_OUT_PTR(dev));
	free(dev);
	dev = NULL;
}

gint device_read_pgm(gint pgm_num)
{
	device_t *dev = device_init();
	
	if (!dev) {
		g_warning("No device to query");
		return -1;
	}

	return query_program(dev, pgm_num);
	device_close(dev);
}


gint device_write_pgm(gint pgm_num)
{
	device_t *dev = device_init();
	RtMidiInPtr midi_out;
	gint ret = -1;

	if (!dev) {
		g_warning("No device to write to");
		return ret;
	}

	if (pgm_num < PGM_NUM_RAM || pgm_num > PGM_NUM_MAX) {
		g_critical("Invalid program number %d, no write performed", pgm_num);
		device_close(dev);
		return ret;
	}
	g_mutex_lock(&msg_buf_mutex);
	/* These must be set and are always the same */
	msg_buf[OFF_SYSEX_START] = SYSEX_START;
	msg_buf[OFF_MF_ID] = MANUFACTURER_ID;
	msg_buf[OFF_ADDR] = MSG_DIRECTION_OUT;
	msg_buf[OFF_PROD_ID] = PRODUCT_ID;
	msg_buf[OFF_COMMAND] = CMD_WRITE_DATA;
	msg_buf[OFF_MSG_LEN] = (MSG_PAYLOAD_LEN >> 7) & 127;
	msg_buf[OFF_MSG_LEN + 1] = MSG_PAYLOAD_LEN & 127;
	msg_buf[OFF_PGM_NUM] = pgm_num;
	msg_buf[OFF_SYSEX_END] = SYSEX_END;

	dump_buffer_data(msg_buf, DATA_MSG_LEN);
	midi_out = MIDI_OUT_PTR(dev);
	rtmidi_out_send_message(midi_out, msg_buf, DATA_MSG_LEN);
	if (!(midi_out->ok)) {
		g_critical("Error sending data for progream %d", pgm_num);
		goto error;
	}
	ret = 0;
error:
	g_mutex_unlock(&msg_buf_mutex);
	device_close(dev);

	return ret;
}


guchar *device_dup_buffer(void)
{
	guchar *ret = g_malloc(sizeof(guchar) * DATA_MSG_LEN);

	g_mutex_lock(&msg_buf_mutex);
	memcpy(ret, msg_buf, DATA_MSG_LEN);
	g_mutex_unlock(&msg_buf_mutex);

	return ret;
}


void device_set_buffer_byte(const guint offset, const guchar val)
{
	if (offset > OFF_SYSEX_END) {
		g_warning("Not setting byte at offset %u", offset);
		return;
	}
	g_mutex_lock(&msg_buf_mutex);
	msg_buf[offset] = val;
	g_mutex_unlock(&msg_buf_mutex);
}
