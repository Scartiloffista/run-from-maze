# use "gcc" to compile source files.
CC = gcc
# the linker is also "gcc". It might be something else with other compilers.
LD = gcc
# Compiler flags go here.
CFLAGS = -g -std=c99 -Wall #-I/usr/local/lib/glib-2.0/include -I/usr/local/include/glib-2.0
# Linker flags go here. Currently there aren't any, but if we'll switch to
# code optimization, we might add "-s" here to strip debug info and symbols.
LDFLAGS = -lncurses -lm #-lglib-2.0
# use this command to erase files.
RM = /bin/rm -f
# list of generated object files.
OBJS = main.o compare.o collection.o heap.o maze.o 
# program executable file name.
PROG = maze

# top-level rule, to compile everything.
all: $(PROG)

# rule to link the program
$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(PROG)

# now comes a meta-rule for compiling any "C" source file.
%.o: %.c
	$(CC) $(CFLAGS) -c $<
clean:
	$(RM) $(PROG) $(OBJS)
