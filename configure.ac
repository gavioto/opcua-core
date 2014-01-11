# Author: Alexander Rykovanov 2009
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

AC_INIT([libopcuacore], [0.1.3], [rykovanov.as@gmail.com])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/common/addons_core/addon_manager.cpp])
AM_INIT_AUTOMAKE([-Wall -Werror])

CXXFLAGS="-O0 -g -std=c++0x"

AC_PROG_CXX
AM_PROG_AR
AC_PROG_LIBTOOL

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
#check headers
AC_CHECK_HEADERS([unistd.h])

AC_ARG_WITH([gtest], 
            [AS_HELP_STRING([--with-gtest=DIR], [defines path to gtest root directory])],
            [ 
              AC_SUBST([GTEST_LIB], [-Wl,--whole-archive,$with_gtest/lib/.libs/libgtest.a,--no-whole-archive])
              AC_SUBST([GTEST_MAIN_LIB], [-Wl,--whole-archive,$with_gtest/lib/.libs/libgtest_main.a,--no-whole-archive])
              AC_SUBST([GTEST_INCLUDES], [-I$with_gtest/include])
              AC_MSG_NOTICE([Gtest path: $GTEST_INCLUDES]) 
            ], 
            [ 
              AC_SUBST([GTEST_LIB], [-lgtest])
              AC_SUBST([GTEST_MAIN_LIB], [-lgtest_main])
           ])

AC_ARG_WITH([gmock], 
            [AS_HELP_STRING([--with-gmock=DIR], [defines path to gmock root directory])],
            [ 
              AC_SUBST([GMOCK_LIB], [-Wl,--whole-archive,$with_gmock/lib/.libs/libgmock.a,--no-whole-archive])
              AC_SUBST([GMOCK_INCLUDES], [-I$with_gmock/include])
              AC_MSG_NOTICE([gmock path: $GMOCK_INCLUDES]) 
            ], 
            [ 
              AC_SUBST([GMOCK_LIB], [-lgmock])
           ])

AC_ARG_WITH([uamappings], 
            [AS_HELP_STRING([--with-uamappings=DIR], [defines path to opcua-mappings root directory])],
            [ 
              AC_SUBST([UAMAPPINGS_INCLUDES], [-I$with_uamappings/include])
              AC_MSG_NOTICE([opcuamappings path: $with_uamappings]) 
            ], 
            [
           ])


AC_SUBST([RELEASE_DATE], [$(date -R)])

#################################################################
AC_CONFIG_FILES([Makefile libopcuacore.pc debian/changelog])

AC_OUTPUT

