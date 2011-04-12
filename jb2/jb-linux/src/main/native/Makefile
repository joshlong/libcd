#/*
# * Copyright 2011 Josh Long
# *
# * This library is free software; you can redistribute it and/or
# * modify it under the terms of the GNU Library General Public
# * License as published by the Free Software Foundation; either
# * version 2 of the License, or (at your option) any later version.
# *
# * This library is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# * Library General Public License for more details.
# *
# * You should have received a copy of the GNU Library General Public
# * License along with this library; if not, write to the
# * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# * Boston, MA 02111-1307, USA.
# */

# sudo apt-get install libcdio libcddb2 libmpeg2 libcddb2-dev libcdio-cdda0 libcdio-paranoia0 libavformat libboost-thread1.40.0 libboost-signals1.40.0 libboost-math1.40.0 libboost-date-time1.40.0 libboost-all-dev libavcodec x264 ogg usbutils libavutil imlib2 vorbisenc libdv nspr
# sudo apt-get install libcdio-cdda0 libcdio-cdda-dev libcdio-paranoia0 libcdio-paranoia-dev libcddb2 libcddb2-dev libcdio-cdda0 libcdio-paranoia0  libboost-thread1.40.0 libboost-signals1.40.0 libboost-math1.40.0 libboost-date-time1.40.0 libboost-all-dev x264 usbutils libnspr4-0d 
# sudo apt-get install vorbis-tools flac cuetools shntool cdparanoia abcde mplayer id3v2 lame monkeys-audio dir2ogg
#  -shared 



DEBUG := -ggdb
EXTRA_OPTS :=   -Wall -fPIC -O  -I/usr/include/nspr -I/usr/include/boost  -lcdio_paranoia $(DEBUG)
PKGS := glib-2.0 libcddb libcdio libavformat gtk+-2.0 vorbis nspr flac ogg
CFLAGS := $(EXTRA_OPTS) $(shell pkg-config --cflags $(PKGS))
LDFLAGS := $(shell pkg-config --libs $(PKGS))  

C_FILES := discid/sha1.c discid/base64.c discid/disc_linux.c discid/disc.c flac_encode.c libjukebox.c main.c

all: $(C_FILES)
	gcc  $(CFLAGS)  $(LDFLAGS)  -o libcd $(C_FILES)


clean:
	rm -rf *o libcd
