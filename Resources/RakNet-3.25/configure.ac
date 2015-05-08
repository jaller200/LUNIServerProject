AC_INIT(raknet, 3.2)
AC_CONFIG_AUX_DIR(autotools/build)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR(Source/RakPeer.cpp)

AM_INIT_AUTOMAKE(foreign tar-ustar no-define)

AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

AC_CONFIG_HEADERS(Source/config.h)

AC_LANG(C++)
AC_PROG_CXX

ACX_PTHREAD(,AC_MSG_ERROR([cannot use pthread]))

AC_ARG_ENABLE([autopatcher],AS_HELP_STRING([--enable-autopatcher],
	      [build autopatcher (default=no)]))

if test "x$enable_autopatcher" = "xyes"; then
	distcheck_args="$distcheck_args --enable-autopatcher"
	AC_CHECK_LIB([bz2], [BZ2_bzWrite],,AC_MSG_ERROR(libbz2 is needed to build autopatcher))
	AX_LIB_POSTGRESQL
	if test "$found_postgresql" != "yes" -o "$want_postgresql" != "yes"; then
		AC_MSG_ERROR([PostgreSQL library (libpg) is required to build autopatcher])
	fi
	AC_CONFIG_FILES(autotools/RakNet-AutoPatcher.pc)
	pkgconfig_add_data="$pkgconfig_add_data autotools/RakNet-AutoPatcher.pc"
fi

AC_ARG_ENABLE([rakvoice],AS_HELP_STRING([--enable-rakvoice],
	      [build RakVoice (default=no)]))

if test "x$enable_rakvoice" = "xyes"; then
	XIPH_PATH_SPEEX(,AC_MSG_ERROR([speex is required to build RakVoice]))
	distcheck_args="$distcheck_args --enable-rakvoice"
	AC_CONFIG_FILES(autotools/RakNet-RakVoice.pc)
	pkgconfig_add_data="$pkgconfig_add_data autotools/RakNet-RakVoice.pc"
fi

AM_CONDITIONAL([ENABLE_AUTOPATCHER], [test x$enable_autopatcher = xyes])
AM_CONDITIONAL([ENABLE_RAKVOICE], [test x$enable_rakvoice = xyes])

AC_SUBST(distcheck_args)
AC_SUBST(pkgconfig_add_data)

# Files where we want to substitude variables (as version)
# AC_CONFIG_FILES(readme.txt autotools/RakNet.pc)

AC_CONFIG_FILES(Makefile Source/Makefile DependentExtensions/Makefile)

AC_OUTPUT
