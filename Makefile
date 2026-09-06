# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = drw.c dwm.c keymaps.c
OBJ = ${SRC:.c=.o}

UTIL_SRC = util.c tree.c
UTIL_OBJ = ${UTIL_SRC:.c=.o}

TEST_SRC = test/move_node.c test/dwm_stubs.c test/navigate_tree.c test/auto_add.c
TEST_OBJ = ${TEST_SRC:.c=.o}

CRITERION_CFLAGS = $(shell pkg-config --cflags criterion)
CRITERION_LIBS = $(shell pkg-config --libs criterion)

${TEST_OBJ}: CFLAGS += ${CRITERION_CFLAGS}

all: dwm

compile_commands:
	bear --output compile_commands.json -- make clean all

# Creates a rule for building .o files from .c files
%.o: %.c
	${CC} -c ${CFLAGS} -o $@ $<

# Tells make that every OBJ file depends on config.h and config.mk, 
# so if either of those files change, the .o files will be rebuilt
${OBJ}: config.h config.mk

config.h: config.def.h 
	cp $< $@

clean:
	rm -f dwm ${OBJ}
	rm -f tests ${TEST_OBJ} ${UTIL_OBJ}

tests: ${TEST_OBJ} ${UTIL_OBJ}
	${CC} -o $@ $^ ${LDFLAGS} ${CRITERION_LIBS}

dwm: ${OBJ} ${UTIL_OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwm ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwm
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < dwm.1 > ${DESTDIR}${MANPREFIX}/man1/dwm.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/dwm.1

.PHONY: all clean compile_commands
