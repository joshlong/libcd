Things I did to get started:
NAUTILUS_DEPS=
 - sudo apt-get install nautilus nautilus-extensions-2.0 nautilus-pastebin nautilus-cd-burner
 - sudo apt-get build-dep nautilus nautilus-extensions-2.0  nautilus-cd-burner
 - sudo apt-get source nautilus-cd-burner


pkg-config --cflags libnautilus-extension gets you the flags you need to build a nautilus extension - not bad! 

