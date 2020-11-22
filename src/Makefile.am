AM_CPPFLAGS = \
	$(GTK_CFLAGS) \
	$(RTMIDI_CFLAGS)
	
AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = mpk3-settings

mpk3_settings_SOURCES = \
	main.c\
	message.h\
	app_win.h\
	app_win.c\
	presets.h\
	presets.c\
	device.h\
	device.c

mpk3_settings_LDFLAGS = 

mpk3_settings_LDADD =  \
	$(GTK_LIBS) \
	$(RTMIDI_LIBS)

