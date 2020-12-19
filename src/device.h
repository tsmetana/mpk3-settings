#ifndef _DEVICE_H
#define _DEVICE_H

void device_init(void);
void device_close(void);
gint device_read_pgm(gint pgm_num);
gint device_write_pgm(gint pgm_num);
guchar *device_dup_buffer(void);
gboolean device_set_buffer_byte(const guint offset, const guchar val);

#endif
