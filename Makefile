CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` 
CMAKE_C_FLAGS = `/D HAVE_DSHOW /D HAVE_VIDEOINPUT`
CC=g++

FILES=main.cpp facedetect.cpp featuredetect.cpp gazeestimate.cpp support.c
OBJECTS=featuredetect.o facedetect.o main.o gazeestimate.o support.o
PROGRAM=gios


build:  $(OBJECTS)
	@echo "Linking object files (did it compile again?)"
	cd ./bin && $(CC) -o $(PROGRAM) $(OBJECTS) $(CFLAGS) $(LIBS)
	mv ./bin/$(PROGRAM) ./$(PROGRAM)

$(OBJECTS) : %.o: ./src/%.cpp
	@echo "Compiling files"
	cd ./bin && $(CC) $(CFLAGS) -c ../$< -o $@

profile: CC += -pg
profile: build

prepare:
	if [ ! -d ./bin ]; then mkdir ./bin; fi
	if [ ! -d ./data ]; then mkdir ./data; fi

clean:
	rm -rf *.o *.exe

cleandir: clean
	rm -rf *~ *\#

rebuild: clean build