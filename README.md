netsurf-kos
===========

-----------------------------------------------------
Build instructions and environment setup.
-----------------------------------------------------

Please install netsurf-buildsystem from either your official distro repos or use : https://github.com/pcwalton/netsurf-buildsystem

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

The address bar doesn't react to mouse/keyboard events yet.
------------------------------------------------------------
