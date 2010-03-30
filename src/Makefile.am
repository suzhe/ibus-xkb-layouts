libexec_PROGRAMS = ibus-engine-xkb-layouts

ibus_engine_xkb_layouts_SOURCES = \
	main.c \
	engine.c \
	engine.h \
	xkbutil.c \
	xkbutil.h \
	$(NULL)
ibus_engine_xkb_layouts_CFLAGS = \
	@IBUS_CFLAGS@ \
	@LIBXKLAVIER_CFLAGS@ \
	$(NULL)
ibus_engine_xkb_layouts_LDFLAGS = \
	@IBUS_LIBS@ \
	@LIBXKLAVIER_LIBS@ \
	$(NULL)

component_DATA = \
	xkb-layouts.xml \
	$(NULL)
componentdir = @datadir@/ibus/component

EXTRA_DIST = \
	xkb-layouts.xml.in \
	$(NULL)

CLEANFILES = \
	xkb-layouts.xml \
	$(NULL)

xkb-layouts.xml: xkb-layouts.xml.in
	$(AM_V_GEN) \
	( \
		libexecdir=${libexecdir}; \
		pkgdatadir=${pkgdatadir}; \
		s=`cat $<`; \
		eval "echo \"$${s}\""; \
	) > $@

