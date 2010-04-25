# cylindrix makefile

# TODO: determine platform..
PLATFORM = $(shell uname)
#PLATFORM = MACOSX
#PLATFORM = LINUX
#PLATFORM = MINGW

# SDL Mac OS X
ifeq ($(PLATFORM),Darwin)
GCC = g++
CFLAGS = -Wall -D GLCYLINDRIX -O3 -D DARWIN -I /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/OpenGL.framework/Headers -I /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/GLUT.framework/Headers -I /Library/Frameworks/SDL.framework/Headers
LFLAGS = -framework Cocoa -framework OpenGL -framework GLUT -framework SDL -framework OpenAL -lstdc++ -lmpg123
endif

# SDL Linux
ifeq ($(PLATFORM),Linux)
GCC = g++
#CFLAGS = -Wall -D GLCYLINDRIX -O3
CFLAGS = -Wall -D GLCYLINDRIX -g
LFLAGS = -lGL -lGLU -lSDL -lc -lmpg123 -lopenal
endif

# SDL Microsoft Windows using MINGW
ifeq ($(PLATFORM),MinGW)
GCC = g++
CFLAGS = -Wall -D GLCYLINDRIX -O3 -I/msys/1.0/include -I/msys/1.0/local/include -I/code/OpenAL-1.1/include
LFLAGS = -lmingw32 -lSDLmain -lSDL -lOpenGL32 -lGlu32 -lmpg123 -lopenal32 -L/msys/1.0/lib -L/msys/1.0/local/lib -L/code/OpenAL-1.1/libs/Win32
endif

OBJ = ai.o ai_move.o ai_util.o base.o clipping.o collide.o commands.o \
	config.o do_state.o energy.o events.o explode.o fli.o fx.o gameloop.o \
	glcylindrix.o hud.o jonsb.o keys.o level.o main.o menu.o motor.o \
	movement.o object.o omega.o path.o pcx.o prim.o project.o pylon.o \
	radar.o states.o stats.o stub.o tanks.o text.o \
	util.o voices.o system.o sound.o record.o

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
	radar.h serial.h states.h stats.h stub.h tanks.h text.h types.h record.h \
	util.h voices.h system/system.h

cylindrix: $(OBJ) $(EXTRA_OBJ)
	$(GCC) $(OBJ) $(EXTRA_OBJ) -o ../cylindrix $(LFLAGS)

%.o : %.cpp
	$(GCC) $(CFLAGS) -c $<

$(OBJ) $(EXTRA_OBJ) : $(DEPENDS)

# SDL system files
system.o: system/sdl/system.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

# OpenAL sound
sound.o : system/openal/sound.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

# Mac OS X system files
ifeq ($(PLATFORM),MACOSX)
SDLMain.o : system/darwin/SDLMain.m $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<
endif

.PHONY : clean tags
clean :
	-rm -f $(OBJ) ../cylindrix

tags :
	etags.emacs `find . -name "*.cpp" -o -name "*.h"`