#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

LIBS = -lboost_system -lboost_filesystem -lxml2 -lpthread -ldl $(UAMAPPINGS_LIBS)
COMMON_INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I$(top_srcdir)/include -I/usr/include/libxml2 $(UAMAPPINGS_INCLUDES)

opcincludedir = $(includedir)/opc
opcuaincludedir = $(opcincludedir)/ua

opcuainclude_HEADERS = \
  include/opc/ua/subscriptions.h \
  include/opc/ua/view.h \
  include/opc/ua/connection_listener.h \
  include/opc/ua/server.h \
  include/opc/ua/attributes.h \
  include/opc/ua/endpoints.h \
  include/opc/ua/errors.h \
  include/opc/ua/socket_channel.h

commondir = $(opcincludedir)/common
common_HEADERS = \
                  include/opc/common/application.h \
                  include/opc/common/class_pointers.h \
                  include/opc/common/errors.h \
                  include/opc/common/exception.h \
                  include/opc/common/interface.h \
                  include/opc/common/modules.h \
                  include/opc/common/object_id.h \
                  include/opc/common/thread.h \
                  include/opc/common/uri_facade.h \
                  include/opc/common/value.h

addonsdir = $(commondir)/addons_core
addons_HEADERS = \
                  include/opc/common/addons_core/addon.h \
                  include/opc/common/addons_core/addon_manager.h \
                  include/opc/common/addons_core/addon_parameters.h \
                  include/opc/common/addons_core/config_file.h \
                  include/opc/common/addons_core/dynamic_addon.h \
                  include/opc/common/addons_core/dynamic_addon_factory.h \
                  include/opc/common/addons_core/errors.h

lib_LTLIBRARIES = libopcuacore.la
libopcuacore_la_SOURCES = \
                  src/common/application.cpp \
                  src/common/object_id.cpp \
                  src/common/thread.cpp \
                  src/common/addons_core/addon_manager.cpp \
                  src/common/addons_core/config_file.cpp \
                  src/common/addons_core/errors_addon_manager.cpp \
                  src/common/addons_core/dynamic_addon_factory.cpp \
                  src/common/addons_core/dynamic_library.cpp \
                  src/common/addons_core/dynamic_library.h \
                  src/common/value.cpp \
                  src/common/exception.cpp \
                  src/common/common_errors.cpp \
                  src/node.cpp \
                  src/opcua_errors.cpp \
                  src/socket_channel.cpp

libopcuacore_la_CPPFLAGS = $(COMMON_INCLUDES)

test_dynamic_addon.o: tests/test_dynamic_addon.cpp
	$(CC) -std=c++0x $(COMMON_INCLUDES) -fPIC -g -c -Wall tests/test_dynamic_addon.cpp 


libtest_dynamic_addon.so: test_dynamic_addon.o
	$(CC) -std=c++0x -g -Wall -ldl -shared -o libtest_dynamic_addon.so test_dynamic_addon.o

all-local: libtest_dynamic_addon.so

CLEANFILES = libtest_dynamic_addon.so test_dynamic_addon.o test_config.xml

TESTS = common_gtest common_test  

check_PROGRAMS = $(TESTS)

common_test_SOURCES = \
                  tests/common/error_test.cpp \
                  tests/common/main.cpp \
                  tests/common/value_test.cpp


common_test_CPPFLAGS = $(COMMON_INCLUDES) $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_test_LDADD = libopcuacore.la
common_test_LDFLAGS = -lcppunit

common_gtest_SOURCES = \
  tests/test_addon_manager.cpp \
  tests/test_config_file.cpp \
  tests/test_dynamic_addon.cpp \
  tests/test_dynamic_addon_factory.cpp \
  tests/test_dynamic_addon.h \
  tests/test_dynamic_addon_id.h \
  tests/test_uri.cpp \
  tests/common/thread_test.cpp

common_gtest_CPPFLAGS =  $(COMMON_INCLUDES) $(GTEST_INCLUDES) $(GMOCK_INCLUDES)
common_gtest_LDADD = libopcuacore.la
common_gtest_LDFLAGS = $(GTEST_LIB) $(GTEST_MAIN_LIB) $(GMOCK_LIB) -no-undefined

EXTRA_DIST = \
  tests/configs/test.xml \
  debian make_deb.sh

pkgsharedir = $(libdir)/pkgconfig
pkgshare_DATA = libopcuacore.pc

pkgconfigdir = $(datarootdir)/pkgconfig
pkgconfig_DATA = libopcuacore.pc

clean-local:
	rm -rvf libopcuacore*$(VERSION)*

dist-deb: dist
	VERSION=$(VERSION) $(top_srcdir)/make_deb.sh

