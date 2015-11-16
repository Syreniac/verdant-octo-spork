# Please run the clean command detailed at the bottom before running: make -f makefile_lab

CFLAGS = -pedantic -Wall -Wextra -Wno-unused-parameter
SDL_FLAGS = -ISDL2_x32/include -LSDL2_x32/lib
LFLAGS = $(SDL_FLAGS) -lmingw32 -lSDL2main -lSDL2 -lm

OBJS = modularUI.o modularUIBackend.o 
PROG = modularUI
CXX = gcc

# top-level rule to create the program.
all: $(PROG)

# compiling other source files.
%.o: %.c %.h
	$(CXX) $(CFLAGS) $(LFLAGS) -c -s $<

# linking the program. | $(BIN) makes the bin/ directory if it does not yet exist.
$(PROG): $(OBJS)
	$(CXX) $(OBJS) -o $(PROG) $(LFLAGS)
	
clean:
	del *.o