#!/bin/sh
make dist
tar -xvf libopcuacore-0.1.2.tar.gz
cd libopcuacore-0.1.2
dh_make -y -l -c lgpl3 -e rykovanov.as@gmail.com -f ../libopcuacore-0.1.2.tar.gz
dpkg-buildpackage -rfakeroot

