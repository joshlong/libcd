VPATH=..
OBJECTS=discid.o MacIoCtlDisc.o
CPPFLAGS=-Wall -W -I .. -DVERSIONINFO=\"powerpc-gcc-macosx\"

discid : $(OBJECTS)
	g++ -o discid $(OBJECTS) $(LIBS)

clean:
	rm $(OBJECTS) discid