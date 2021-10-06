LUAI 	= -I/usr/local/include/lua5.4
CFLAGS 	= -Iinclude $(LUAI)
LFLAGS 	= -llua -shared -fpic
MAIN 	=$(src/main.c)
SRC 	= $(filter-out $(MAIN),$(wildcard src/*.c lib/*/*.c))
OBJ 	= $(patsubst %.c,%.o,$(SRC) )


sqlite3.so: $(OBJ)
	$(CC) $(LFLAGS) -o sqlite3.so $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o  $@

.PHONY: clean
clean:
	$(RM) $(OBJ)