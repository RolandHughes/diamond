#!/bin/sh
#
# -I /usr/lib/cs_lib/include -I /usr/lib/cs_lib/include/QtCore \
cppcheck --enable=all --std=c++17 --platform=native \
         src
