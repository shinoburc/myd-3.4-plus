bin_PROGRAMS = myd

if USE_XSELECTION
myd_SOURCES = myd.c myd.h main.c getsel.c getsel.h \
	term.c term.h \
	edit.c edit.h
myd_LDADD = ${X_LIBS} ${X_PRE_LIBS} -lX11 ${X_EXTRA_LIBS}
else
myd_SOURCES = myd.c myd.h main.c \
	term.c term.h \
	edit.c edit.h
myd_LDADD = ${X_LIBS} ${X_PRE_LIBS} ${X_EXTRA_LIBS}
endif

AM_CFLAGS = ${X_CFLAGS}
AM_CPPFLAGS = -DPKGDATADIR=\"${pkgdatadir}\"
