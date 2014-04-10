netsurf-kos
===========

For setting up the toolchain :
=============================
Download the toolchain folder to /home/yourname/me
now, set up an environment variable MENUETDEV = /home/yourname/me

The toolchain contains all necessary libraries required to make stuff.
Make sure MENUETDEV shell variable is always set.
You can consider putting this in a file like ~/.bashrc so it is initialized each time.

Once this is done, you're all set to make stuff for KOS!

-----------------------------------

FOR BUILDING _netsurf binary : 
==============================

Go into netsurf/netsurf/content/fetchers

make -f make.fetch

This will generate the necessary .o files (like curl.o)

Copy curl.o to netsurf/netsurf/objs/

Now go to netsurf/netsurf/objs
make -f make.all

This will generate the _netsurf binary ready to be used on KolibriOS

-------------------------------------------------------------

NetSurf port for KolibriOS

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
