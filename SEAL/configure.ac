AC_INIT(SEAL, 2.1)
AC_LANG(C++)
AC_PROG_CXX

AC_ARG_WITH(intrin, [AS_HELP_STRING([--without-intrin], [do not use intrinsics])])

ac_has_intrin=no
ac_has___builtin_clzll=no
ac_has__addcarry_u64=no
ac_has__subborrow_u64=no
ac_has_mbmi2=no
ac_has__mulx_u64=no

if test "$with_intrin" != no
then
	AC_CHECK_HEADERS([x86intrin.h], [ac_has_intrin=yes], [])
	if test "$ac_has_intrin" == yes
	then
		AC_DEFINE([ENABLE_INTRIN])
		

		AC_MSG_CHECKING([for __builtin_clzll])
		AC_COMPILE_IFELSE(
			[AC_LANG_PROGRAM([], [__builtin_clzll(0)])],
			[ac_has___builtin_clzll=yes], [])
		if test "$ac_has___builtin_clzll" == yes
		then
			AC_MSG_RESULT([yes])
			AC_DEFINE([ENABLE___BUILTIN_CLZLL])
		else
			AC_MSG_RESULT([no])
		fi

		AC_MSG_CHECKING([for _addcarry_u64])
		AC_RUN_IFELSE(
			[AC_LANG_PROGRAM([#include <x86intrin.h>]
					[unsigned long long a;], [_addcarry_u64(0,0,0,&a)])],
			[ac_has__addcarry_u64=yes], [])
		if test "$ac_has__addcarry_u64" == yes
		then
			AC_MSG_RESULT([yes])
			AC_DEFINE([ENABLE__ADDCARRY_U64])
		else
			AC_MSG_RESULT([no])
		fi

		AC_MSG_CHECKING([for _subborrow_u64])
		AC_RUN_IFELSE(
			[AC_LANG_PROGRAM([#include <x86intrin.h>]
					[unsigned long long a;], [_subborrow_u64(0,0,0,&a)])],
			[ac_has__subborrow_u64=yes], [])
		if test "$ac_has__subborrow_u64" == yes
		then
			AC_MSG_RESULT([yes])
			AC_DEFINE([ENABLE__SUBBORROW_U64])
		else
			AC_MSG_RESULT([no])
		fi

		ac_saved_cxxflags="$CXXFLAGS"
		CXXFLAGS="$CXXFLAGS -mbmi2"
		AC_MSG_CHECKING([if $CXX supports -mbmi2])
		AC_COMPILE_IFELSE(
			[AC_LANG_PROGRAM([])],
			[ac_has_mbmi2=yes], [])
		if test "$ac_has_mbmi2" == yes
		then
			AC_MSG_RESULT([yes])
		else
			AC_MSG_RESULT([no])
			CXXFLAGS="$ac_saved_cxxflags"
		fi

		if test "$ac_has_mbmi2" == yes
		then
			AC_MSG_CHECKING([for _mulx_u64])
			AC_RUN_IFELSE(
				[AC_LANG_PROGRAM([#include <x86intrin.h>]
						[unsigned long long a;], [_mulx_u64(0,0,&a)])],
				[ac_has__mulx_u64=yes], [])
			if test "$ac_has__mulx_u64" == yes
			then
				AC_MSG_RESULT([yes])
				AC_DEFINE([ENABLE__MULX_U64])
			else
				AC_MSG_RESULT([no])
			fi
		fi
	fi
fi

AC_CONFIG_HEADERS([config.h])

AC_OUTPUT(Makefile)
