MAKEFLAGS= -j3
CXXFLAGS= -std=c++11 `pkg-config --cflags opencv` -lX11 -lXext -lXrender -lGL
LIBS = `pkg-config --libs opencv`

OBJECTS= support.o \
	 facedetect.o \
	 featuredetect.o \
	 gazeestimate.o \
	 emulatedriver.o \
	 main.o \
	 gui.o \
	 guipointer.o	

LINK_TARGET=gios

vpath %.h ./src
vpath %.cpp ./src
vpath %.o ./bin

%.o: %.cpp
	g++ $(CXXFLAGS) -o ./bin/$@ -c $<


$(LINK_TARGET): $(OBJECTS)
	g++ -o $@ $^ $(LIBS) $(CXXFLAGS)

debug: CXXFLAGS+= -g
debug: $(LINK_TARGET)

warning: CXXFLAGS+= -Wall
warning: $(LINK_TARGET)

optimize: CXXFLAGS+= -O3
optimize: $(LINK_TARGET)

prepare:
	if [ ! -d ./bin ]; then mkdir ./bin; fi
	if [ ! -d ./data ]; then mkdir ./data; fi

clean:
	find -maxdepth 2 -name *.o -delete
	find -maxdepth 1 -name gios -delete
