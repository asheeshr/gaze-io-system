CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` 
CMAKE_C_FLAGS = `/D HAVE_DSHOW /D HAVE_VIDEOINPUT`
CC=gcc

FILES=main.c facedetect.c featuredetect.c gazeestimate.c
OBJECTS=main.o #facedetect.o #featuredetect.o gazeestimate.o
PROGRAM=gios


build:  $(OBJECTS)
	echo "Linking object files (did it compile again?)"
	cd ./bin && $(CC) -o $(PROGRAM) $(OBJECTS) $(CFLAGS) $(LIBS)
	mv ./bin/$(PROGRAM) ../$(PROGRAM)

$(OBJECTS) : %.o: ./src/%.c
	echo "Compiling files"
	cd ./bin && gcc $(CFLAGS) -c ../$< -o $@

prepare:
	if [ ! -d ./bin ]; then mkdir ./bin; fi
	if [ ! -d ./data ]; then mkdir ./data; fi

clean:
	rm -rf *.o *.exe

cleandir: clean
	rm -rf *~ *\#

rebuild: clean build