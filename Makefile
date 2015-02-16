CFLAGS = `pkg-config --cflags opencv`"-lX11"
LIBS = `pkg-config --libs opencv`"-lX11" 
CMAKE_C_FLAGS = `/D HAVE_DSHOW /D HAVE_VIDEOINPUT`
CC=g++

FILES=main.cpp facedetect.cpp featuredetect.cpp gazeestimate.cpp support.c
OBJECTS=featuredetect.o facedetect.o main.o gazeestimate.o support.o
PROGRAM=gios


build:  $(OBJECTS) #featuredetect.o facedetect.o main.o gazeestimate.o support.o #$(OBJECTS)
	@echo "Linking object files (did it compile again?)"
	cd ./bin && $(CC) -o $(PROGRAM) $(OBJECTS) $(CFLAGS) $(LIBS)
	mv ./bin/$(PROGRAM) ./$(PROGRAM)

featuredetect.o:  ./src/featuredetect.cpp
	echo "compiling featuredetect.cpp"
	$(CC) $(CFLAGS) -c ./src/featuredetect.cpp 

facedetect.o:  ./src/facedetect.cpp
	echo "compiling facedetect.cpp"
	$(CC) $(CFLAGS) -c ./src/facedetect.cpp

main.o:  ./src/main.cpp
	echo "main.cpp"
	$(CC) $(CFLAGS) -c ./src/main.cpp

gazeestimate.o:  ./src/gazeestimate.cpp
	echo "gazeestimate.cpp"
	$(CC) $(CFLAGS) -c ./src/gazeestimate.cpp

support.o:  ./src/support.cpp
	echo "support.cpp"
	$(CC) $(CFLAGS) -c ./src/support.cpp

#$(OBJECTS) : %.o: ./src/%.cpp
#	@echo "Compiling files"
#	cd ./bin && $(CC) $(CFLAGS) -c ../$< -o $@

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
