CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` 
CMAKE_C_FLAGS = `/D HAVE_DSHOW /D HAVE_VIDEOINPUT`
CC=gcc

FILES=facedetect.c featuredetect.c gazeestimate.c
OBJECTS=facedetect.o #featuredetect.o gazeestimate.o
PROGRAM=gios


build: ./bin/$(OBJECTS)
	cd ./bin && $(CC) -o $(PROGRAM) $(OBJECTS)
	mv ./bin/$(PROGRAM) ../

$(OBJECTS) : %.o: ./src/%.c
	cd ./bin && gcc $(CFLAGS) $(LIBS) -c ../$< -o $@

clean:
	rm -rf *.o *.exe

cleandir: clean
	rm -rf *~ *\#

rebuild: clean build