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

 

  
DEBUG := -ggdb

EXTRA_OPTS :=  -Wall -fPIC -O  -I/usr/include/nspr -I/usr/include/boost   $(DEBUG) 
PKGS := glib-2.0 libcddb libcdio    nspr flac ogg
CFLAGS := $(EXTRA_OPTS) $(shell pkg-config --cflags $(PKGS))
#-framework CoreFoundation -framework Foundation  -framework IOKit
LDFLAGS :=  $(shell pkg-config --libs $(PKGS)) 

C_FILES := discid/MacIoCtlDisc.cpp discid/discid.cpp 

all: $(C_FILES)
	g++ $(LDFLAGS)  $(CFLAGS)  $(LDFLAGS)  -o libcd $(C_FILES)


clean:
	rm -rf *o libcd
