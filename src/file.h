#ifndef _FILE_H
#define _FILE_H

gboolean file_read(const gchar *filename, GError **err);
gboolean file_write(const gchar *filename, GError **err);

#endif
