# Yoichi Kawasaki <yokawasa@gmail.com>
pkgplugindir=@MYSQL_PLUGINDIR@
INCLUDES= -I$(top_builddir)/include -I$(top_srcdir)/include @MYSQL_SRC_INCLUDES@
#noinst_LTLIBRARIES= mypluglib.la
pkgplugin_LTLIBRARIES= is_mallinfo.la
is_mallinfo_la_SOURCES= is_mallinfo.cc
is_mallinfo_la_LDFLAGS= -module -rpath $(pkgplugindir)
is_mallinfo_la_CXXFLAGS= $(AM_CFLAGS) -DMYSQL_DYNAMIC_PLUGIN
is_mallinfo_la_CFLAGS= $(AM_CFLAGS) -DMYSQL_DYNAMIC_PLUGIN


