#!/bin/sh
tar -xvf libopcuacore-${VERSION}.tar.gz
cd libopcuacore-${VERSION}
dpkg-buildpackage -rfakeroot

