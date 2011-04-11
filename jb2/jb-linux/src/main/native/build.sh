export PKGS="glib-2.0 libcddb libcdio libavformat gtk+-2.0 vorbis nspr "
export EXTRA_OPTS="-Wall -fPIC -O -I/usr/include/cdio -I/usr/include/nspr -I/usr/include/boost -ggdb"
export CF=" `pkg-config --cflags $PKGS`"
export LF=" -lcdio_paranoia -shared `pkg-config --libs $PKGS` "


CMD="gcc $EXTRA_OPTS $CF $LF  libcd.c -o libcd "
echo $CMD
`$CMD`


