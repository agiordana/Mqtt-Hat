CC = g++
FLAGS = -O2 -I. -I../lib -I../tinyxml -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd -lmosquittopp -lmosquitto
AGENTLIB = ../lib/*.o
XML = ../tinyxml/*.o
BIN = ../bin
OBJ = agent.o AgentTaskLocal.o Gpio.o

bbgpio:	$(OBJ) main.cpp agent.h AgentTaskLocal.h
	$(CC) $(FLAGS) $(OPTLIB) -o bbgpio main.cpp $(OBJ) $(AGENTLIB) $(LIB) $(XML)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

install:	bbgpio
	cp bbgpio $(BIN)/bbgpio

clean:
	rm -f *.o bbgpio

