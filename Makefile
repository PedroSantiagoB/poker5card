#---------------------------------------------------------------------
# Arquivo	: Makefile
# Conteúdo	: compilar o programa poker.c
# Autor		: Pedro Gomes Santiago Pires Beltrão (pedrogomespego@gmail.com)

#---------------------------------------------------------------------
# Opções	: make - compila tudo
#		: make clean - remove objetos e executável
#---------------------------------------------------------------------

CC = gcc
LIBS = -lm
SRC = src
OBJ = obj
INC = include
BIN = bin
OBJS = $(OBJ)/poker.o
HDRS = $(INC)/poker.h $(INC)/msgassert.h
CFLAGS = -pg -Wall -c -I$(INC)

EXE = $(BIN)/tp1

perf: $(EXE)
	$(EXE)

$(BIN)/tp1: $(OBJS)
	$(CC) -pg -o $(BIN)/tp1 $(OBJS) $(LIBS)

$(OBJ)/poker.o: $(HDRS) $(SRC)/poker.c
	$(CC) $(CFLAGS) -o $(OBJ)/poker.o $(SRC)/poker.c 
	
clean:
	rm -f $(EXE) $(OBJS) gmon.out