### makefile for simple programs
###########################################
INC=
LIB=

CC=g++
CC_FLAG=-Wall

PRG=lzw
OBJ=*.o

$(PRG):$(OBJ)
	$(CC) $(OBJ) $(INC) $(LIB) -o $(PRG)
.SUFFIXES: .c .o .cpp
.cpp.o:
	$(CC) $(INC) -c $*.cpp $(CC_FLAG)
.PRONY:clean
clean:
	rm -f $(OBJ) $(PRG)
