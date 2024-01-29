AM_CPPFLAGS = \
	$(GTK_CFLAGS) \
	$(RTMIDI_CFLAGS)
	
AM_CFLAGS =\
	 -Wall\
	 -g\
	 -fPIC

bin_PROGRAMS = mpk3-settings

mpk3_settings_SOURCES = \
	main.c\
	message.h\
	app_win.h\
	app_win.c\
	presets.h\
	presets.c\
	device.h\
	device.c\
	file.h\
	file.c

mpk3_settings_LDFLAGS = \
	-pie -Wl,--as-needed

mpk3_settings_LDADD =  \
	$(GTK_LIBS) \
	$(RTMIDI_LIBS)

