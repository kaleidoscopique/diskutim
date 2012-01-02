# Compilation
CFLAGS=-Wall
CLIBS = -lpthread -lgcrypt
CC=gcc
# Directory
OBJ_DIR=obj/
SRC_DIR=src/
BIN_DIR=bin/
DOC_DIR=doc/
# Files
OBJ_FILES = 	${OBJ_DIR}main_serveur.o \
							${OBJ_DIR}manage_clients.o \
							${OBJ_DIR}manage_protocole.o \
							${OBJ_DIR}manage_services.o \
							${OBJ_DIR}manage_sockets.o \
							${OBJ_DIR}manage_auth.o \
							${OBJ_DIR}manage_threads.o \
							${OBJ_DIR}TAD_element.o \
							${OBJ_DIR}TAD_tabDyn.o

# Compilator
${BIN_DIR}serveur.out:${OBJ_FILES}
	${CC} $^ -o $@ ${CLIBS}

${OBJ_DIR}%.o:${SRC_DIR}%.c
	${CC} -c $^ -o $@ ${CFLAGS}

clean:
	rm ${OBJ_DIR}*.o
	rm ${BIN_DIR}*.out
