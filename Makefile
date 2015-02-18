CFLAGS = -std=c++11 `pkg-config --cflags opencv`"-lX11"
LIBS = `pkg-config --libs opencv`"-lX11" 
CMAKE_C_FLAGS = `/D HAVE_DSHOW /D HAVE_VIDEOINPUT`
CC=g++

FILES=main.cpp facedetect.cpp featuredetect.cpp gazeestimate.cpp support.cpp
OBJECTS=support.o gazeestimate.o featuredetect.o facedetect.o main.o
PROGRAM=gios


build:  $(OBJECTS) #featuredetect.o facedetect.o main.o gazeestimate.o support.o #$(OBJECTS)
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
