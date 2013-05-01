# cylindrix makefile

#PLATFORM = $(shell uname)
PLATFORM = Browser
DEBUG = false

# SDL Mac OS X
ifeq ($(PLATFORM),Darwin)
GCC = g++
CFLAGS = -Wall -D GLCYLINDRIX -D DARWIN `sdl-config --cflags`
LFLAGS = -framework Cocoa -framework OpenGL -framework GLUT -framework OpenAL -lstdc++ -lmpg123 -lyaml `sdl-config --libs`
TARGET = ../cylindrix
endif

# SDL Linux
ifeq ($(PLATFORM),Linux)
GCC = g++
CFLAGS = -Wall -D GLCYLINDRIX
LFLAGS = -lGL -lGLU -lSDL -lc -lmpg123 -lopenal -lyaml
TARGET = ../cylindrix
endif

# SDL Microsoft Windows using MINGW
ifeq ($(PLATFORM),MinGW)
GCC = g++
CFLAGS = -Wall -D GLCYLINDRIX -I/msys/1.0/include -I/msys/1.0/local/include -I/code/OpenAL-1.1/include
LFLAGS = -lmingw32 -lSDLmain -lSDL -lOpenGL32 -lGlu32 -lmpg123 -lopenal32 -lyaml -L/msys/1.0/lib -L/msys/1.0/local/lib -L/code/OpenAL-1.1/libs/Win32
TARGET = ../cylindrix
endif

# Browser build using emscripten
ifeq ($(PLATFORM),Browser)
GCC = ~/code/emscripten/emcc
CFLAGS = -Wall -D GLCYLINDRIX -D EMSCRIPTEN -I../libyaml/include
LFLAGS = -lGL -lGLU -lSDL -lc -lyaml --preload-file ../pcx_data/microfnt.pcx
TARGET = ../cylindrix.html
endif

# debug symbols
ifeq ($(DEBUG),true)
CFLAGS += -g
else
CFLAGS += -O3
endif

OBJ = ai.o ai_move.o ai_util.o base.o clipping.o collide.o commands.o \
	config.o do_state.o energy.o events.o explode.o fli.o fx.o gameloop.o \
	glcylindrix.o hud.o jonsb.o keys.o level.o main.o menu.o motor.o \
	movement.o object.o omega.o path.o pcx.o prim.o project.o pylon.o \
	radar.o states.o stats.o stub.o tanks.o text.o \
	util.o voices.o system.o sound.o

# Link with SDLMain when on MacOSX
ifeq ($(PLATFORM),Darwin)
EXTRA_OBJ = SDLMain.o
else
EXTRA_OBJ =
endif

# TODO: fix dependencies
DEPENDS = ai.h ai_move.h ai_util.h base.h clipping.h collide.h commands.h \
	config.h do_state.h energy.h events.h explode.h fli.h fx.h gameloop.h \
	glcylindrix.h hud.h jonsb.h keys.h level.h main.h menu.h motor.h \
	movement.h object.h omega.h path.h pcx.h prim.h project.h pylon.h \
	radar.h serial.h states.h stats.h stub.h tanks.h text.h types.h \
	util.h voices.h system/system.h

cylindrix: $(OBJ) $(EXTRA_OBJ)
	$(GCC) $(OBJ) $(EXTRA_OBJ) -o $(TARGET) $(LFLAGS)

%.o : %.cpp
	$(GCC) $(CFLAGS) -c $<

$(OBJ) $(EXTRA_OBJ) : $(DEPENDS)


# SDL system files
system.o: system/sdl/system.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

ifeq ($(PLATFORM),Browser)
# no sound
sound.o : system/glut/sound.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<
else
# OpenAL sound
sound.o : system/openal/sound.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<
endif

# Mac OS X system files
ifeq ($(PLATFORM),Darwin)
SDLMain.o : system/darwin/SDLMain.m $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<
endif

.PHONY : clean tags
clean :
	-rm -f $(OBJ) ../cylindrix

tags :
	etags `find . -name "*.cpp" -o -name "*.h"`