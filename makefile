# left side of rule = $@
# right side of rule = $^
# first item in dependencies list = $<
CC=gcc
CFLAGS= -g
PARSER_DEPS = lexer.h parser.h
OBJ = lexer.o parser.o


%.o: parser/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

hud: hud.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o hud

clean:
	rm -f *.o hud parser/lexer.h.gch
	rm -r hud.dYSM
