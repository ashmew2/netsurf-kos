netsurf-kos
===========

Sourcerer initially forked from netsurf git tree rev number 43fe4490d3152ccecd7fd8065c09faaa893fb09e

-----------------------------------------------------
Build instructions and environment setup.
-----------------------------------------------------

Please install netsurf-buildsystem from either your official distro repos or use : https://github.com/pcwalton/netsurf-buildsystem

============================================
Notes for building libraries inside netsurf/
============================================
You need to run the makefiles in the directories where you make changes.
Then, go one directory up (cd ..) . Use the makefile to generate a .a library.
This .a library should be put in your $MENUETDEV/lib/ directory.
This will ensure that the new updated library is used by make.all in netsurf/netsurf/objs.

==========================
PREREQUISITES:
==========================

You need :  
    GNU Make
    GCC (GNU C COMPILER)
    pkg-config
    
=============================
For setting up the toolchain:
=============================

The toolchain is contained in the "me" directory (courtesy, SoUrcerer)

Download the toolchain folder to /home/yourname/me
now, set up an environment variable MENUETDEV = /home/yourname/me

The toolchain contains all necessary libraries required to make stuff.
Make sure MENUETDEV shell variable is always set.
You can consider putting this in a file like ~/.bashrc so it is initialized each time.

Once this is done, you're all set to make stuff for KOS!

==============================
FOR BUILDING _netsurf binary : 
==============================

Go into netsurf/netsurf/content/fetchers

make -f make.fetch

This will generate the necessary .o files (like curl.o)

Copy curl.o to netsurf/netsurf/objs/

Now go to netsurf/netsurf/objs
make -f make.all

This will generate the _netsurf binary ready to be used on KolibriOS

========================================
IMPORTANT Instruction for res directory:
========================================

There is also a res directory on the repository (courtesy,  SoUrcerer).

The generated _netsurf binary needs to be placed outside the res folder in order to run properly.

For example, if you are running KolibriOS and currently at /usbhd0/1/ , these are the paths :

_netsurf binary : /usbhd0/1/_netsurf
res directory   : /usbhd0/1/res/

The res directory contains resources such as fonts which help Netsurf run.

-------------------------------------------------------------
===========================
NetSurf port for KolibriOS:
===========================

The netsurf directory is in kolibrios/contrib/network/

This is the current state of this repo :

Built the following libraries with MENUETLIBC : 

      libwapcaplet
      libparserutils
      libcss
      libhubbub
      libdom
      libnsbmp
      libnsgif

Some work has been carried out on getting libhttp working with this (thanks to 
hidnplayr!).

The Netsurf window now reacts to keyboard and mouse events.

Current status : GET seems to work properly. POST needs some work and needs to be linked with the C code using http.obj ASM library. Also, since we don't have a port for libcurl on KolibriOS, libcurl and Netsurf code need to be decoupled.

------------------------------------------------------------
