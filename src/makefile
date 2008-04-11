# glcylindrix makefile

GCC = g++

# SDL Linux
#LFLAGS = -lGL -lglut -lSDL
#CFLAGS = -Wall -D GLCYLINDRIX -O

# SDL Mac OS X
CFLAGS = -Wall -D GLCYLINDRIX -O -D DARWIN -I /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/OpenGL.framework/Headers -I /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/GLUT.framework/Headers -I /Library/Frameworks/SDL.framework/Headers
LFLAGS = -framework Cocoa -framework OpenGL -framework GLUT -framework SDL -framework OpenAL -lstdc++ -lmpg123

OBJ = ai.o ai_move.o ai_util.o base.o clipping.o collide.o commands.o \
	config.o do_state.o energy.o events.o explode.o fli.o fx.o gameloop.o \
	glcylindrix.o hud.o jonsb.o keys.o level.o main.o menu.o motor.o \
	movement.o object.o omega.o path.o pcx.o prim.o project.o pylon.o \
	radar.o states.o stats.o stub.o tanks.o text.o \
	util.o voices.o system.o sound.o SDLMain.o

# TODO: fix dependencies
DEPENDS = ai.h ai_move.h ai_util.h base.h clipping.h collide.h commands.h \
	config.h do_state.h energy.h events.h explode.h fli.h fx.h gameloop.h \
	glcylindrix.h hud.h jonsb.h keys.h level.h main.h menu.h motor.h \
	movement.h object.h omega.h path.h pcx.h prim.h project.h pylon.h \
	radar.h serial.h states.h stats.h stub.h tanks.h text.h types.h \
	util.h voices.h system/system.h

glcylindrix: $(OBJ)
	$(GCC) $(OBJ) -o ../glcylindrix $(LFLAGS)


ai.o: ai.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

ai_move.o: ai_move.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

ai_util.o: ai_util.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

base.o: base.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

clipping.o: clipping.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

collide.o: collide.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

commands.o: commands.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

config.o: config.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

do_state.o: do_state.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

energy.o: energy.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

events.o: events.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

explode.o: explode.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

fli.o: fli.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

fx.o: fx.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

gameloop.o: gameloop.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

glcylindrix.o: glcylindrix.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

hud.o: hud.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

jonsb.o: jonsb.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

keys.o: keys.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

level.o: level.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

main.o: main.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

menu.o: menu.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

motor.o: motor.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

movement.o: movement.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

object.o: object.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

omega.o: omega.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

path.o: path.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

pcx.o: pcx.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

prim.o: prim.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

project.o: project.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

pylon.o: pylon.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

radar.o: radar.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

states.o: states.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

stats.o: stats.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

stub.o: stub.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

tanks.o: tanks.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

text.o: text.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

util.o: util.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

voices.o: voices.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

# GLUT system files
#system.o: system/glut/system.cpp $(DEPENDS)
#	$(GCC) $(CFLAGS) -c $<
#
#sound.o : system/glut/sound.cpp $(DEPENDS)
#	$(GCC) $(CFLAGS) -c $<

# SDL system files
system.o: system/sdl/system.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

# OpenAL sound
sound.o : system/openal/sound.cpp $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

# Mac OS X system files
SDLMain.o : system/darwin/SDLMain.m $(DEPENDS)
	$(GCC) $(CFLAGS) -c $<

clean:
	rm $(OBJ) ../glcylindrix
