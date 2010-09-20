
lib_LTLIBRARIES=libfcitx-sunpinyin.la
libfcitx_sunpinyin_la_SOURCES= \
                             eim.cpp eim.h \
							 handler.cpp handler.h \
							 $(NULL)

libfcitx_sunpinyin_la_LDFLAGS= -version-number 0:1:0 -no-undefined

INCLUDES = \
	@FCITX_CFLAGS@ \
	@SUNPINYIN_CFLAGS@ \
	$(NULL)

libfcitx_sunpinyin_la_LIBADD = \
	@FCITX_LIBS@ \
	@SUNPINYIN_LIBS@ \
	$(NULL)
