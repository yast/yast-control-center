#!/bin/sh
set -e

g++ -DHAVE_CONFIG_H -I. -I. -I.. -I/usr/lib/qt2/include -I/usr/X11R6/include     -g -fno-exceptions -fno-rtti -fno-check-new -Wall -pedantic -W -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -Wno-long-long -fno-builtin  -c yastmodules.cpp
g++ -DHAVE_CONFIG_H -I. -I. -I.. -I/usr/lib/qt2/include -I/usr/X11R6/include     -g -fno-exceptions -fno-rtti -fno-check-new -Wall -pedantic -W -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -Wno-long-long -fno-builtin  -c testmain.cpp
/bin/sh ../libtool --mode=link g++  -g -fno-exceptions -fno-rtti -fno-check-new -Wall -pedantic -W -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -Wno-long-long -fno-builtin   -o testmain -L/usr/X11R6/lib -L/usr/lib/qt2/lib   yastmodules.o testmain.o  -lqt -lXext -lX11
