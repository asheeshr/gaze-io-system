CXXFLAGS= -std=c++11 `pkg-config --cflags opencv` -lX11
LIBS = `pkg-config --libs opencv`"-lX11"

OBJECTS= support.o \
	 facedetect.o \
	 featuredetect.o \
	 gazeestimate.o \
	 emulatedriver.o \
	 main.o \
	 gui.o 

LINK_TARGET=gios

vpath %.h ./src
vpath %.cpp ./src
vpath %.o ./bin

%.o: %.cpp
	g++ $(CXXFLAGS) -o ./bin/$@ -c $<


$(LINK_TARGET): $(OBJECTS)
	g++ $(LIBS) $(CXXFLAGS) -o $@ $^
