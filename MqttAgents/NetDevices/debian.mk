CC = g++
FLAGS = -O2 -I. -I../lib -I../tinyxml -I/opt/include -g -DNDEBUG
LIB = -lstdc++ -lpthread -lssl -lcrypto -lhttpserver -lmicrohttpd -lmosquittopp -lmosquitto
AGENTLIB = ../lib/*.o
XML = ../tinyxml/*.o
BIN = ../bin
OBJ = agent.o AgentTaskLocal.o DeviceHub.o Device.o Automaton.o Component.o

netdevices:	$(OBJ) main.cpp agent.h AgentTaskLocal.h
	$(CC) $(FLAGS) $(OPTLIB) -o netdevices main.cpp $(OBJ) $(AGENTLIB) $(LIB) $(XML)

%.o:	%.cpp
	$(CC) -c $(FLAGS) $<

install:	netdevices
	cp netdevices $(BIN)/netdevices

clean:
	rm -f *.o netdevices

