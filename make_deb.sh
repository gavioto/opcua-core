#!/bin/sh
tar -xvf libopcuacore-${VERSION}.tar.gz
cd libopcuacore-${VERSION}
dh_make -y -l -c lgpl3 -e rykovanov.as@gmail.com -f ../libopcuacore-${VERSION}.tar.gz
dpkg-buildpackage -rfakeroot -nc -F ${DPKG_BUILDPACKAGE_OPTS}

