# Makefile for KTH's code.

include Makefile.inc

C_SOURCES = src/extra/coldet/*.c
SOURCES = src/*.cpp src/extra/*.cpp src/extra/coldet/*.cpp

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard $(SOURCES))) $(patsubst %.c, %.o, $(wildcard $(C_SOURCES)))
DEPENDS = $(patsubst %.cpp, %.d, $(wildcard $(SOURCES)))

SDL_LIB = -lSDL2 
GLUT_LIB = -lGLEW

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
LIBS = $(SDL_LIB) $(GLUT_LIB) -lGL
endif
ifeq ($(UNAME), Darwin)
LIBS = $(SDL_LIB) $(GLUT_LIB) -framework OpenGL -framework Cocoa -framework IOKit
endif

all:	main

main:	$(DEPENDS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LIBS) -o $@

%.d: %.cpp
	@$(CXX) -M -MT "$*.o $@" $(CPPFLAGS) $<  > $@
	@echo Generating new dependencies for $<

run:
	./main

clean:
	rm -f $(OBJECTS) $(DEPENDS) main *.pyc

-include $(SOURCES:.cpp=.d)

