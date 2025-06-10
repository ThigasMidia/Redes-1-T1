CC = gcc
CFLAGS = -g -Wall 

INCLUDE = ./include
LIB = ./lib
OBJ = ./obj

all: obj servidor cliente

servidor: $(OBJ)/servidor.o
	$(CC) -o servidor $(OBJSER)
	
cliente: $(OBJ)/cliente.o
	$(CC) -o cliente $(OBJCLI)
    
    
OBJCLI = $(OBJ)/cliente.o $(OBJ)/clientFunc.o $(OBJ)/front.o $(OBJ)/shared.o
OBJSER = $(OBJ)/servidor.o $(OBJ)/serverFunc.o $(OBJ)/shared.o

obj:
	mkdir $(OBJ)

$(OBJ)/servidor.o: servidor.c $(OBJ)/serverFunc.o 
	$(CC) -c $(CFLAGS) $< -I $(INCLUDE) -o $(OBJ)/servidor.o

$(OBJ)/serverFunc.o: $(LIB)/serverFunc.c $(INCLUDE)/serverFunc.h $(OBJ)/shared.o
	$(CC) -c $(CFLAGS) $< -I $(INCLUDE) -o $(OBJ)/serverFunc.o	 

$(OBJ)/cliente.o: cliente.c $(OBJ)/clientFunc.o $(OBJ)/front.o
	$(CC) -c $(CFLAGS) $< -I $(INCLUDE) -o $(OBJ)/cliente.o

$(OBJ)/clientFunc.o: $(LIB)/clientFunc.c $(INCLUDE)/clientFunc.h $(OBJ)/shared.o
	$(CC) -c $(CFLAGS) $< -I $(INCLUDE) -o $(OBJ)/clientFunc.o	
	
$(OBJ)/front.o: $(LIB)/front.c $(INCLUDE)/front.h 
	$(CC) -c $(CFLAGS) $< -I $(INCLUDE) -o $(OBJ)/front.o

$(OBJ)/shared.o: $(LIB)/shared.c $(INCLUDE)/shared.h 
	$(CC) -c $(CFLAGS) $< -I $(INCLUDE) -o $(OBJ)/shared.o


clean:
	rm -rf $(OBJ)

purge: clean
	rm -f servidor cliente 
