bin_PROGRAMS = myd

myd_SOURCES = myd.c myd.h main.c getsel.c getsel.h \
	term.c term.h \
	edit.c edit.h

myd_LDADD = ${X_LIBS} ${X_PRE_LIBS} -lX11 ${X_EXTRA_LIBS}

AM_CFLAGS = ${X_CFLAGS}
AM_CPPFLAGS = -DPKGDATADIR=\"${pkgdatadir}\"
