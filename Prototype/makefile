
CFLAGS = -pedantic
LFLAGS = -ISDL2\include\SDL2 -LSDL2\lib -lmingw32 -lSDL2main -lSDL2 -lm
# List all the files here
OBJS = main.o init.o game.o world_generation.o game_objects.o AI.o UI.o
PROG = Prototype
CXX = gcc

# top-level rule to create the program.
all: $(PROG)

# compiling other source files.
%.o: src/%.c src/%.h src/generic.h
	$(CXX) $(CFLAGS) $(LFLAGS) -c -s $<

# linking the program.
$(PROG): $(OBJS)
	$(CXX) $(OBJS) -o $(PROG) $(LFLAGS)

# cleaning everything that can be automatically recreated with "make".
clean:
	rm $(PROG) *.o