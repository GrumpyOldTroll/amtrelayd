EXTRA_DIST =
CLEANFILES =
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
AM_MAKEFLAGS = --no-print-directory

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-I${top_srcdir}/src

AM_CFLAGS = ${my_CFLAGS}

AM_LDFLAGS = \
	-Wl,--gc-sections \
	-Wl,--as-needed

SED_PROCESS = \
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(SED) \
	-e 's,@VERSION\@,$(VERSION),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@exec_prefix\@,$(exec_prefix),g' \
	-e 's,@libdir\@,$(libdir),g' \
	-e 's,@includedir\@,$(includedir),g' \
	< $< > $@ || rm $@

%.pc: %.pc.in Makefile
	$(SED_PROCESS)

AMTRELAYD_CURRENT=2
AMTRELAYD_REVISION=0
AMTRELAYD_AGE=2

if ENABLE_DNSSD
DNSSD_LIB=-ldns_sd
else
DNSSD_LIB=
endif

bin_PROGRAMS = src/amtrelayd

src_amtrelayd_SOURCES = \
	src/config.c \
	src/hmac.c \
	src/main.c \
	src/memory.c \
	src/md5.c \
	src/packet_building.c \
	src/pat.c \
	src/prefix.c \
	src/recv.c \
	src/tree.c \
	src/amt.h \
	src/global.h \
	src/hmac.h \
	src/igmp.h \
	src/memory.h \
	src/memory_private.h \
	src/md5.h \
	src/mld6.h \
	src/pat.h \
	src/prefix.h \
	src/relay.h \
	src/tree.h \
	src/utils.h
src_amtrelayd_LDADD = \
	-levent \
	$(DNSSD_LIB)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = src/amtrelayd.pc
EXTRA_DIST += src/amtrelayd.pc.in
CLEANFILES += src/amtrelayd.pc

TESTS = \
	src/test-hmac \
	src/test-patricia

check_PROGRAMS = \
	src/test-hmac \
	src/test-patricia

src_test_hmac_SOURCES = src/tests/test-hmac.c
src_test_hmac_LDADD = src/hmac.o

src_test_patricia_SOURCES = src/tests/test-patricia.c
src_test_patricia_LDADD = \
	src/pat.o \
	src/memory.o

