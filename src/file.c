#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <gtk/gtk.h>
#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "message.h"
#include "device.h"
#include "file.h"
#include "presets.h"

#define FILE_SIZE         2873
#define F_HEADER_LEN        12
#define F_BASE_CHUNK_LEN     4
#define F_CHUNK_MAX        108 /* 0x6c */
#define F_CHUNK_META_LEN   (6 * F_BASE_CHUNK_LEN)

/* No idea what these mean but they're always the same */
#define F_MAGIC_1 0x01
#define F_MAGIC_2 0x0e

#define INVALID_FMT_ERR_IF(_cond) \
	if (_cond) {\
		*err = g_error_new(G_FILE_ERROR, -1, "%s: Invalid file format", filename);\
		goto error;\
	}

gboolean file_read(const gchar *filename, GError **err)
{
	GStatBuf st;
	GError *orig_err;
	gint errno_save;
	gchar file_buf[FILE_SIZE];
	gsize data_len;
	guchar *chunk_ptr;
	guchar chunk_size;
	guchar chunk_size_2;
	guchar chunk_num;
	guchar chunk_num_prev = 0;
	gint offset;
	gint i;
	gint fr;

	if (g_stat(filename, &st) < 0) {
		errno_save = errno;
		*err = g_error_new(G_FILE_ERROR, errno_save, "Error accessing %s", filename);
		return FALSE;
	}
	if (st.st_size != FILE_SIZE) {
		*err = g_error_new(G_FILE_ERROR, -1, "%s: Wrong file size. Expected %d, got %zd", filename, FILE_SIZE, st.st_size);
		return FALSE;
	}
	fr = g_open(filename, O_CLOEXEC, 0);
	if (fr < 0) {
		errno_save = errno;
		*err = g_error_new(G_FILE_ERROR, errno_save, "%s: Error opening file: %s", filename, strerror(errno_save));
		return FALSE;
	}
	if ((data_len = read(fr, (void *)file_buf, FILE_SIZE)) < 0) {
		errno_save = errno;
		*err = g_error_new(G_FILE_ERROR, errno_save, "%s: Error reading file: %s", filename, strerror(errno_save));
		return FALSE;
	}
	if (!g_close(fr, &orig_err)) {
		g_propagate_error(err, orig_err);
		return FALSE;
	}
	chunk_ptr = (guchar *)file_buf;

	if (chunk_ptr[0] != 0x01) {
		*err = g_error_new(G_FILE_ERROR, -1, "%s: Invalid file format, first byte %02x, size %zd", filename, chunk_ptr[0], data_len);
		return FALSE;
	}
	
	chunk_ptr += F_HEADER_LEN;
	/* The mpkmini3 files store the values in the same order as in the SYSEX
	 * however interleave them with lots of superfluous data. Program name is
	 * the first value that is stored in the file -- the previous SYSEX bytes
	 * make sense only for communication with the device. */
	offset = OFF_PGM_NAME;
	do {
		chunk_size = *chunk_ptr;
		chunk_ptr += 2 * F_BASE_CHUNK_LEN;
		chunk_num = *chunk_ptr;
		INVALID_FMT_ERR_IF(chunk_num < chunk_num_prev);
		data_len = chunk_size - F_CHUNK_META_LEN;
		chunk_ptr += F_BASE_CHUNK_LEN;
		chunk_size_2 = *chunk_ptr;
		/* Moved 3 chunks so far */
		INVALID_FMT_ERR_IF((chunk_size - chunk_size_2) != 3 * F_BASE_CHUNK_LEN);
		chunk_ptr += F_BASE_CHUNK_LEN;
		INVALID_FMT_ERR_IF(*chunk_ptr != F_MAGIC_1);
		chunk_ptr += F_BASE_CHUNK_LEN;
		INVALID_FMT_ERR_IF(*chunk_ptr != F_MAGIC_2);
		chunk_ptr += F_BASE_CHUNK_LEN;
		for (i = 0; i < data_len; i++) {
			INVALID_FMT_ERR_IF(!device_set_buffer_byte(offset, chunk_ptr[i]));
			offset++;
		}
		chunk_ptr += data_len;
	} while (chunk_num < F_CHUNK_MAX);

	return TRUE;
error:
	return FALSE;
}


gboolean file_write(const gchar *filename, GError **err)
{
	GError *orig_err;
	gint errno_save;
	gchar file_buf[FILE_SIZE];
	gssize data_len;
	guchar *chunk_ptr;
	guchar chunk_size;
	guchar chunk_size_2;
	guchar chunk_num = 0;
	gint offset;
	gint i;
	gint fw;
	guchar *buf = device_dup_buffer();
	gboolean ret = FALSE;

	/* construct the whole contens of the file in file_buf */
	memset(file_buf, 0, FILE_SIZE);
	chunk_ptr = (guchar *)file_buf;
	*chunk_ptr = 0x01;
	chunk_ptr += F_HEADER_LEN;
	offset = OFF_PGM_NAME;
	while (chunk_num <= F_CHUNK_MAX) {
		data_len = preset_get_length(offset);
		chunk_size = data_len + F_CHUNK_META_LEN;
		*chunk_ptr = chunk_size;
		chunk_ptr += 2 * F_BASE_CHUNK_LEN;
		*chunk_ptr = chunk_num++;
		chunk_ptr += F_BASE_CHUNK_LEN;
		chunk_size_2 = chunk_size - 3 * F_BASE_CHUNK_LEN;
		*chunk_ptr = chunk_size_2;
		chunk_ptr += F_BASE_CHUNK_LEN;
		*chunk_ptr = F_MAGIC_1;
		chunk_ptr += F_BASE_CHUNK_LEN;
		*chunk_ptr = F_MAGIC_2;
		chunk_ptr += F_BASE_CHUNK_LEN;
		if (offset >= OFF_SYSEX_END) {
			*err = g_error_new(G_FILE_ERROR, -1, "%s: Error writing file: internal error", filename);
			goto error;
		}
		for (i = 0; i < data_len; i++) {
			chunk_ptr[i] = buf[offset++];
		}
		chunk_ptr += data_len;
	}
	g_free(buf);

	/* now write the file_buf into the file */
	fw = g_open(filename, O_CLOEXEC | O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (fw < 0) {
		errno_save = errno;
		*err = g_error_new(G_FILE_ERROR, errno_save, "%s: Error opening file: %s", filename, strerror(errno_save));
		return ret;
	}
	data_len = write(fw, (const void*)file_buf, FILE_SIZE);
	if (data_len < 0) {
		errno_save = errno;
		*err = g_error_new(G_FILE_ERROR, errno_save, "%s: Error writing file: %s", filename, strerror(errno_save));
		goto error;
	}
	if (data_len != FILE_SIZE) {
		*err = g_error_new(G_FILE_ERROR, -1, "%s: Error writing file: file size does not match", filename);
		goto error;
	}
	if (!g_close(fw, &orig_err)) {
		g_propagate_error(err, orig_err);
		return FALSE;
	}
	ret = TRUE;

error:
	close(fw);
	return ret;
}
