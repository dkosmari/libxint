# tests/Makefile.am

$(info TERM="$(TERM)")

AM_DEFAULT_SOURCE_EXT = .cpp


SUBDIRS = \
	catch2 \
	utils


AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-DXINT_LIMB_SIZE=8


AM_CXXFLAGS = -Wall -Wextra


check_PROGRAMS = \
	addition \
	constructors \
	division \
	multiplication \
	serialization \
	shifting \
	stdlib \
	subtraction


TESTS = $(check_PROGRAMS)


LDADD = \
	catch2/libcatch2.a \
	utils/libutils.a
