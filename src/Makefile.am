SUBDIRS = resample shout libmpeg portaudio @GUI_DIRS@

INCLUDES = -Ishout -Iportaudio -I/sw/include -I/usr/X11R6/include

CFLAGS = @GUI_FLAGS@ @OGG_CFLAGS@ @VORBIS_CFLAGS@

bin_PROGRAMS = muse

muse_SOURCES =  \
    muse.cpp        \
    jmixer.cpp      \
    audioproc.cpp       \
    inchannels.cpp      \
    decoder.cpp		\
    dec_mp3.cpp		\
    dec_ogg.cpp		\
    dec_snd.cpp		\
    dec_jack.cpp	\
    outchannels.cpp     \
    out_vorbis.cpp	\
    out_lame.cpp	\
    pipe.cpp        \
    shouter.cpp     \
    linklist.cpp        \
    playlist.cpp        \
    jutils.cpp      \
    httpstream.cpp  \
    gui.cpp \
    dev_sound.cpp

muse_DEPENDENCIES = \
	@GUI_DEPS@ \
	resample/libresample.a \
	libmpeg/libmpeg.a \
	shout/libshout.a \
	portaudio/libportaudio.a

muse_LDADD = \
	$(top_srcdir)/src/resample/libresample.a \
	$(top_srcdir)/src/libmpeg/libmpeg.a \
	$(top_srcdir)/src/shout/libshout.a \
	$(top_srcdir)/src/portaudio/libportaudio.a \
	-lpthread \
	@M_LIBS@ \
	@LAME_LIBS@ \
	@OGG_LIBS@ \
	@VORBIS_LIBS@ \
	@GUI_LIBS@ \
	@SNDFILE_LIBS@

