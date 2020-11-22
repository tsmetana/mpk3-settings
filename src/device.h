#ifndef _DEVICE_H
#define _DEVICE_H

gint device_read_pgm(gint pgm_num);
gint device_write_pgm(gint pgm_num);
guchar *device_dup_buffer(void);
void device_set_buffer_byte(const guint offset, const guchar val);

#endif
