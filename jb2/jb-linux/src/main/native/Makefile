# sudo apt-get install libcdio libcddb2 libmpeg2 libcddb2-dev libcdio-cdda0 libcdio-paranoia0 libavformat libboost-thread1.40.0 libboost-signals1.40.0 libboost-math1.40.0 libboost-date-time1.40.0 libboost-all-dev libavcodec x264 ogg usbutils libavutil imlib2 vorbisenc libdv nspr

#sudo apt-get install libcdio-cdda0 libcdio-cdda-dev libcdio-paranoia0 libcdio-paranoia-dev libcddb2 libcddb2-dev libcdio-cdda0 libcdio-paranoia0  libboost-thread1.40.0 libboost-signals1.40.0 libboost-math1.40.0 libboost-date-time1.40.0 libboost-all-dev x264 usbutils libnspr4-0d 

#  -shared 
EXTRA_OPTS :=   -Wall -fPIC -O  -I/usr/include/nspr -I/usr/include/boost -ggdb -lcdio_paranoia
PKGS := glib-2.0 libcddb libcdio libavformat gtk+-2.0 vorbis nspr
CFLAGS := $(EXTRA_OPTS) $(shell pkg-config --cflags $(PKGS))
LDFLAGS := $(shell pkg-config --libs $(PKGS))  

C_FILES := discid/sha1.c discid/base64.c discid/disc_linux.c discid/disc.c libcd.c main.c

all: $(C_FILES)
	gcc  $(CFLAGS)  $(LDFLAGS)  -o libcd $(C_FILES)


clean:
	rm -rf *o libcd
