# Compilation
CC=gcc
# Directory
OBJ_DIR=obj/
SRC_DIR=src/
BIN_DIR=bin/
DOC_DIR=doc/

ifeq ($(GUI),nogui)
	DEFINE = -DNOGUI
	CFLAGS = $(DEFINE) -Wall
	CLIBS = -pthread
	OBJ_FILES = ${OBJ_DIR}nogui_adapted_functions.o \
							${OBJ_DIR}nogui_main.o \
							${OBJ_DIR}manage_clients.o \
							${OBJ_DIR}manage_protocole.o \
							${OBJ_DIR}manage_services.o \
							${OBJ_DIR}manage_sockets.o \
							${OBJ_DIR}manage_threads.o \
							${OBJ_DIR}main.o
else
	DEFINE = -DGTKGUI
	CFLAGS=$(DEFINE) -Wall `pkg-config gtk+-3.0 --cflags`
	CLIBS = `pkg-config gtk+-3.0 --libs`
	OBJ_FILES = ${OBJ_DIR}gtkgui_adapted_functions.o \
							${OBJ_DIR}gtkgui_create_widgets.o \
							${OBJ_DIR}gtkgui_main.o \
							${OBJ_DIR}manage_clients.o \
							${OBJ_DIR}manage_protocole.o \
							${OBJ_DIR}manage_services.o \
							${OBJ_DIR}manage_sockets.o \
							${OBJ_DIR}manage_threads.o \
							${OBJ_DIR}main.o
endif

# Compilator
${BIN_DIR}diskutim:${OBJ_FILES}
	${CC} $^ -o $@ ${CLIBS}

${OBJ_DIR}%.o:${SRC_DIR}%.c
	${CC} -c $^ -o $@ ${CFLAGS}

${OBJ_DIR}%.o:${SRC_DIR}network/%.c
	${CC} -c $^ -o $@ ${CFLAGS}

${OBJ_DIR}%.o:${SRC_DIR}gui/$(GUI)/%.c
	${CC} -c $^ -o $@ ${CFLAGS}

clean:
	rm ${OBJ_DIR}*.o
	rm -R ${BIN_DIR}*
