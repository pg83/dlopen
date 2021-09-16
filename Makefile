all: libdl.a

libdl.a: dlfcn.o
	ar q libdl.a dlfcn.o

dlfcn.o: dlfcn.cpp dlfcn.h
	$(CC) -std=c++2a $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -c -o dlfcn.o dlfcn.cpp

install: all
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/include
	cp libdl.a $(PREFIX)/bin
	cp dlfcn.h $(PREFIX)/include
